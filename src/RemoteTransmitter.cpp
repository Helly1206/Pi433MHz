/* 
 * RemoteSwitch library v2.2.1 (20120314) made by Randy Simons http://randysimons.nl/
 * See RemoteTransmitter.h for details.
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
#include "RemoteTransmitter.h"

/************
* RemoteTransmitter
************/

RemoteTransmitter::RemoteTransmitter(int pi, unsigned short pin, unsigned int periodusec, unsigned short repeats) {
	//_pin=pin;
	//_periodusec=periodusec;
	_pi=pi;
	_repeats=repeats;
	int retval = 0;
	uint32_t gpin = (1<<pin);
	
	retval = set_mode(pi, pin, PI_OUTPUT); // Set GPIOxx as output.

	if (!retval) {
		retval = set_pull_up_down(pi, pin, PI_PUD_OFF); // no pull up/ pull down
	}

	// Generate waves
	wave_clear(pi);

	gpioPulse_t wave0[4]={{gpin, 0, periodusec},{0, gpin, periodusec*3},{gpin, 0, periodusec},{0, gpin, periodusec*3}};
	gpioPulse_t wave1[4]={{gpin, 0, periodusec*3},{0, gpin, periodusec},{gpin, 0, periodusec*3},{0, gpin, periodusec}};
	gpioPulse_t wave2[4]={{gpin, 0, periodusec},{0, gpin, periodusec*3},{gpin, 0, periodusec*3},{0, gpin, periodusec}};
	gpioPulse_t wave3[2]={{gpin, 0, periodusec},{0, gpin, periodusec*31}};

	wave_add_generic(pi, 4, wave0);
    _wave[0] = wave_create(pi);
	wave_add_generic(pi, 4, wave1);
    _wave[1] = wave_create(pi);
    wave_add_generic(pi, 4, wave2);
    _wave[2] = wave_create(pi);
    wave_add_generic(pi, 2, wave3);
    _wave[3] = wave_create(pi);

	_initok = retval? false:true;
}

unsigned long RemoteTransmitter::encodeTelegram(unsigned short trits[]) {
	unsigned long data = 0;
		
	// Encode data
	for (unsigned short i=0;i<12;i++) {
		data*=3;
		data+=trits[i];
	}
	
	// Encode period duration
	//data |= (unsigned long)_periodusec << 23;
	
	// Encode repeats
	//data |= (unsigned long)_repeats << 20;
	
	return data;
}
		
void RemoteTransmitter::sendTelegram(unsigned short trits[]) {
	sendTelegram(encodeTelegram(trits));	
}

/**
* Format data:
* pppppppp|prrrdddd|dddddddd|dddddddd (32 bit)
* p = perioud (9 bit unsigned int
* r = repeats as 2log. Thus, if r = 3, then signal is sent 2^3=8 times
* d = data
*/
void RemoteTransmitter::sendTelegram(unsigned long data) {
	//unsigned int periodusec = (unsigned long)data >> 23;
	//unsigned short repeats = ((unsigned long)data >> 20) & 0x0007;
	
	sendCode(data);	
}

void RemoteTransmitter::sendCode(unsigned long code) {
	if (_initok) {
		unsigned short repeats = 0;
		char WaveChain[24]; // chain contains 13 waves + repeat information
		int WaveCnt=0;
	
		code &= 0xfffff; // Truncate to 20 bit ;
		// Convert the base3-code to base4, to avoid lengthy calculations when transmitting.. Messes up timings.
		unsigned long dataBase4 = 0;
		
		for (unsigned short i=0; i<12; i++) {
			dataBase4<<=2;
			dataBase4|=(code%3);
			code/=3;
		}
		
		repeats = 1 << (_repeats & 0x0007); // repeats := 2^repeats;
	
		// Recycle code as working var to save memory
		// Start loop in wave
		WaveChain[WaveCnt++]=(char)255;
		WaveChain[WaveCnt++]=(char)0;
		// Sent one telegram
		code=dataBase4;
		for (unsigned short i=0; i<12; i++) {
			switch (code & 0x0003) {
				case 0:
					WaveChain[WaveCnt++]=(char)_wave[0];
					break;
				case 1:
					WaveChain[WaveCnt++]=(char)_wave[1];
					break;
				case 2: // KA: X or float
					WaveChain[WaveCnt++]=(char)_wave[2];
					break;
			}
			// Next trit
			code>>=2;
		}
		
		// Send termination/synchronisation-signal. Total length: 32 periods
		WaveChain[WaveCnt++]=(char)_wave[3];
	
		// Start repeat loop in wave
		WaveChain[WaveCnt++]=(char)255;
		WaveChain[WaveCnt++]=(char)1;
		WaveChain[WaveCnt++]=(char)repeats;
		WaveChain[WaveCnt++]=(char)0;
		
		wave_chain(_pi, WaveChain, WaveCnt);
	
	   while (wave_tx_busy(_pi)) time_sleep(0.1);
	}
}

bool RemoteTransmitter::isSameCode(unsigned long encodedTelegram, unsigned long receivedData) {
	return (receivedData==(encodedTelegram & 0xFFFFF)); // Compare the 20 LSB's
}

// access functions
extern "C" RemoteTransmitter* _RemoteTransmitter(int pi, short pin, unsigned int periodusec, unsigned short repeats) {
	return new RemoteTransmitter(pi, pin, periodusec, repeats);
}

extern "C" void _RemoteTransmitter_delete(RemoteTransmitter* t) {
	delete t;
}

extern "C" unsigned long _RemoteTransmitter_encodeTelegram(RemoteTransmitter* t, unsigned short trits[]) {
	return t->encodeTelegram(trits);
}

extern "C" void _RemoteTransmitter_sendTelegram(RemoteTransmitter* t, unsigned short trits[]) {
	t->sendTelegram(trits);
}

extern "C" void _RemoteTransmitter_sendCode(RemoteTransmitter* t, unsigned long code) {
	t->sendCode(code);
}

extern "C" bool _RemoteTransmitter_isSameCode(RemoteTransmitter* t, unsigned long encodedTelegram, unsigned long receivedData) {
	return t->isSameCode(encodedTelegram, receivedData);
}

