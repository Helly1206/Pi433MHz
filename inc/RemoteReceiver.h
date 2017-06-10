/*
 * RemoteSwitch library v2.2.1 (20120314) made by Randy Simons http://randysimons.nl/
 *
 * License: GPLv3. See license.txt
 *
 * Ported to Raspberry PI (using PIGPIO) by helly
 * GPIO needs to be initialised in calling function .... 
 * v1.0.0 (20161224)
 *
 */

#ifndef RemoteReceiver_h
#define RemoteReceiver_h

typedef void (*RemoteReceiverCallBack)(unsigned long, unsigned int);

/**
* See RemoteSwitch for introduction.
*
* RemoteReceiver decodes the signal received from a 433MHz-receiver, like the "KlikAanKlikUit"-system
* as well as the signal sent by the RemoteSwtich class. When a correct signal is received,
* a user-defined callback function is called.
*
* Note that in the callback function, the interrupts are still disabled. You can enabled them, if needed.
* A call to the callback must be finished before RemoteReceiver will call the callback function again, thus
* there is no re-entrant problem.
*
* When sending your own code using RemoteSwich, disable() the receiver first.
*
* This is a pure static class, for simplicity and to limit memory-use.
*/

class RemoteReceiver {
	public:
		/**Constructor Initializes the decoder.
		* 
		*
		* @param pi 		Current pi handler from daemon
		* @param pin 		GPIO pin on rPi that will be used as interrupt.
							If < 0, you must call interruptHandler() yourself. 
		* @param minRepeats The number of times the same code must be received in a row before the callback is called
		* @param callback Pointer to a callback function, with signature void (*func)(unsigned long, unsigned int). First parameter is the decoded data, the second the period of the timing.
		*/
		RemoteReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback);
		
		/**
		* Enable decoding. No need to call enable() after init().
		*/
		void enable();
		
		/**
		* Disable decoding. You can re-enable decoding by calling enable();
		*/
		void disable();
		
		/**
		* Tells wether a signal is being received. If a compatible signal is detected within the time out, isReceiving returns true.
		* Since it makes no sense to transmit while another transmitter is active, it's best to wait for isReceiving() to false.
		* By default it waits for 150ms, in which a (relative slow) KaKu signal can be broadcasted three times.
		*
		* Note: isReceiving() depends on interrupts enabled. Thus, when disabled()'ed, or when interrupts are disabled (as is
		* the case in the callback), isReceiving() will not work properly.
		*		
		* @param waitMillis number of milliseconds to monitor for signal.
		* @return boolean If after waitMillis no signal was being processed, returns false. If before expiration a signal was being processed, returns true.
		*/
		bool isReceiving(int waitMillis = 150);
				
		static void interruptHandler(int /*pi*/, unsigned int /*gpio*/, unsigned int /*level*/, unsigned int tick);
		
		unsigned short decodeTelegram(unsigned long lCode);
		
	private:
		int _pi;
		bool _initok;
		volatile static short _state;		// State of decoding process. There are 49 states, 1 for "waiting for signal" and 48 for decoding the 48 edges in a valid code.
		static unsigned short _minRepeats;
		static RemoteReceiverCallBack _callback;
		static bool _inCallback;					// When true, the callback function is being executed; prevents re-entrance.
		static bool _enabled;					// If true, monitoring and decoding is enabled. If false, interruptHandler will return immediately.
};

extern "C" RemoteReceiver* _RemoteReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback);
extern "C" void _RemoteReceiver_delete(RemoteReceiver* t);
extern "C" void _RemoteReceiver_enable(RemoteReceiver* t);
extern "C" void _RemoteReceiver_disable(RemoteReceiver* t);
extern "C" bool _RemoteReceiver_isReceiving(RemoteReceiver* t, int waitMillis = 150);
extern "C" unsigned short _RemoteReceiver_decodeTelegram(RemoteReceiver* t, unsigned long lCode);

#endif
