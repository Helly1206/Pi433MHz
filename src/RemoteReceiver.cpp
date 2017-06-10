/* 
 * RemoteSwitch library v2.2.1 (20120314) made by Randy Simons http://randysimons.nl/
 * See RemoteReceiver.h for details.
 *
 * License: GPLv3. See license.txt
 *
 * Ported to Raspberry PI (using PIGPIO) by helly
 * GPIO needs to be initialised in calling function .... 
 * v1.0.0 (20161224)
 *
 */

#include <stdint.h>
#include <pigpiod_if2.h>
//#include <iostream>
#include "RemoteReceiver.h"

/************
* RemoteReceiver
************/
//int RemoteReceiver::_pi;
volatile short RemoteReceiver::_state;
unsigned short RemoteReceiver::_minRepeats;
RemoteReceiverCallBack RemoteReceiver::_callback;
bool RemoteReceiver::_inCallback = false;
bool RemoteReceiver::_enabled = false;

RemoteReceiver::RemoteReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack fcallback) {
	_pi = pi;
	_minRepeats = minRepeats;
	_callback = fcallback;	
	int retval = 0;
	
	enable();
	retval = set_mode(pi, pin, PI_INPUT); // Set GPIOxx as input.

	if (!retval) {
		retval = set_pull_up_down(pi, pin, PI_PUD_OFF); // no pull up/ pull down
	}

	if (pin >= 0 && !retval) {
		retval = callback(pi, pin, EITHER_EDGE, interruptHandler);
	}

	_initok = retval? false:true;
}

void RemoteReceiver::enable() {
	if (_initok) {
		_state = -1;
		_enabled = true;
	}
}

void RemoteReceiver::disable() {
	if (_initok) {
		_enabled = false;
	}
}


void RemoteReceiver::interruptHandler(int /*pi*/, unsigned int /*gpio*/, unsigned int /*level*/, unsigned int tick) {
	if (!_enabled) {
		return;
	}
	
	static unsigned int period;				// Calculated duration of 1 period
	static unsigned short receivedBit;		// Contains "bit" currently receiving
	static unsigned long receivedCode;		// Contains received code
	static unsigned long previousCode;		// Contains previous received code
	static unsigned short repeats = 0;		// The number of times the an identical code is received in a row.
	static unsigned long lastChange = 0;	// Timestamp of previous edge
	static unsigned int min1Period, max1Period, min3Period, max3Period;

	unsigned int duration=tick-lastChange; // Duration = Time between edges
	lastChange=tick;

	if (_state==-1) { // Waiting for sync-signal		
		if (duration>3720) { // =31*120 minimal time between two edges before decoding starts.
			// Sync signal received.. Preparing for decoding			
			period=duration/31;
			receivedCode=previousCode=repeats=0;

			// Allow for large error-margin. ElCheapo-hardware :( 
			min1Period=period*4/10; // void floating point math; saves memory.
			max1Period=period*16/10;
			min3Period=period*23/10;
			max3Period=period*37/10;
			//std::cout << "sync:" << period << "," << level << std::endl;
		} 
		else {
			return;
		}
	} else if (_state<48) { // Decoding message
		// bit part durations can ONLY be 1 or 3 periods.
		if (duration>=min1Period && duration<=max1Period) {
			//std::cout << "1p:" << duration << "," << level << std::endl;
			//bitClear(receivedBit,_state%4);
			receivedBit &= ~((unsigned short)1 << (_state%4)); // Note: this sets the bits in reversed order! Correct order would be: 3-(_state%4), but that's overhead we don't want.
		} 
		else if (duration>=min3Period && duration<=max3Period) {
			//bitSet(receivedBit,_state%4);
			//std::cout << "3p:" << duration << "," << level << std::endl;
			receivedBit |= ((unsigned short)1 << (_state%4)); // Note: this sets the bits in reversed order!
		} 
		else { // Otherwise the entire sequence is invalid
			//std::cout << "inv:" << duration << "," << level << std::endl;
			_state=-1;
			return;
		}

		if ((_state%4)==3) { // Last bit part?
			// Shift
			receivedCode*=3;
			// Decode bit.
			if (receivedBit==0x000A) {  // short long short long == B0101, but bits are set in reversed order, so compare to B1010
				// bit "0" received
				receivedCode+=0; // I hope the optimizer handles this ;)
			} 
			else if (receivedBit==0x0005) { // long short long short == B101, but bits are set in reversed order, so compare to B0101
				// bit "1" received
				receivedCode+=1;
			} 
			else if (receivedBit==0x0006) { // short long long short. Reversed too, but makes no difference.
				// bit "f" received
				receivedCode+=2;
			} 
			else {
				// Bit was rubbish. Abort.
				_state=-1;
				return;
			}      
		}    
	} else if (_state==48) { // Waiting for sync bit part 1
		// Must be 1 period.
		if (duration<min1Period || duration>max1Period) {
			_state=-1;
			return;
		}
	} else { // Waiting for sync bit part 2 
		// Must be 31 periods.
		if (duration<period*25 || duration>period*36) {
			_state=-1;
			return;
		}

		// receivedCode is a valid code!
		//std::cout << "Code: " << receivedCode << "," << previousCode << std::endl;
		
		if (receivedCode!=previousCode) {
			repeats=0;
			previousCode=receivedCode;
		}
		
		repeats++;
		
		if (repeats>=_minRepeats) {
			if (!_inCallback) {
				_inCallback = true;
				(_callback)(receivedCode, period);
				_inCallback = false;
			}
			// Reset after callback.
			_state=-1;
			return;			
		}
		
		// Reset for next round
		receivedCode = 0;
		_state=0; // no need to wait for another sync-bit!    
		return;
	}

	_state++;
	return;
}

bool RemoteReceiver::isReceiving(int waitMillis) {
	if (_initok) {	
		unsigned long startTime=(get_current_tick(_pi)/1000);
	
		int waited; // Signed int!
		do {
			if (_state == 48) { // Abort if a valid code has been received in the mean time
				return true;
			}
			waited = ((get_current_tick(_pi)/1000)-startTime);
		} while(waited>=0 && waited <= waitMillis); // Yes, clock wraps every 50 days. And then you'd have to wait for a looooong time.
	}
	return false;
}

unsigned short RemoteReceiver::decodeTelegram(unsigned long lCode) {
	unsigned long data = lCode&0xFFFFF; // only decode 20 bits
  	unsigned short trits;
  	unsigned short retval = 0;
  
  	for (unsigned short i=0;i<12;i++) {
    	// decode trits
    	trits = data % 3;
    	data-=trits;
    	data/=3;
    	// trits are 1 or 2 vs. 0, so trits encoding is not important anymore
    	retval<<=1;
    	retval += (trits?0:1);
  	}
  	return (retval);
}

// access functions
extern "C" RemoteReceiver* _RemoteReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback) {
	return new RemoteReceiver(pi, pin, minRepeats, callback);
}

extern "C" void _RemoteReceiver_delete(RemoteReceiver* t) {
	delete t;
}

extern "C" void _RemoteReceiver_enable(RemoteReceiver* t) {
	t->enable();
}

extern "C" void _RemoteReceiver_disable(RemoteReceiver* t) {
	t->disable();
}

extern "C" bool _RemoteReceiver_isReceiving(RemoteReceiver* t, int waitMillis) {
	return t->isReceiving(waitMillis);
}

extern "C" unsigned short _RemoteReceiver_decodeTelegram(RemoteReceiver* t, unsigned long lCode) {
	return t->decodeTelegram(lCode);
}