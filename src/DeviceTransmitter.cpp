/* 
 * RemoteSwitch library v2.2.1 (20120314) made by Randy Simons http://randysimons.nl/
 * See RemoteTransmitter.h for details.
 *
 * License: GPLv3. See license.txt
 *
 * Devices are extracted
 * Ported to Raspberry PI (using PIGPIO) by helly
 * v1.0.0 (20161224)
 *
 */

#include <stdint.h>
#include "DeviceTransmitter.h"

/************
* ActionTransmitter
************/

ActionTransmitter::ActionTransmitter(int pi, unsigned short pin, unsigned int periodusec, unsigned short repeats) : RemoteTransmitter(pi, pin,periodusec,repeats) {
	// Call contructor
}


void ActionTransmitter::sendSignal(unsigned short systemCode, char device, bool on) {
	sendTelegram(getTelegram(systemCode,device,on));
}

void ActionTransmitter::sendSignalFmt(int *codes, int size) {
	if (size<3) return;
	sendTelegram(getTelegram((unsigned short)codes[0], (char)codes[1], (bool)codes[2]));
}

unsigned long ActionTransmitter::getTelegram(unsigned short systemCode, char device, bool on) {
	unsigned short trits[12];
	
	device-=65;
	
	for (unsigned short i=0; i<5; i++) {
		// Trits 0-4 contain address (2^5=32 addresses)
		trits[i]=(systemCode & 1)?1:2;          
		systemCode>>=1;
		
		// Trits 5-9 contain device. Only one trit has value 0, others have 2 (float)!
		trits[i+5]=(i==(unsigned short)device?0:2);
    }
	
	// Switch on or off
	trits[10]=(!on?0:2);
	trits[11]=(on?0:2);
	
	return encodeTelegram(trits);
}

/************
* BlokkerTransmitter
************/

BlokkerTransmitter::BlokkerTransmitter(int pi, unsigned short pin, unsigned int periodusec, unsigned short repeats) : RemoteTransmitter(pi, pin,periodusec,repeats) {
	// Call contructor
}


void BlokkerTransmitter::sendSignal(unsigned short device, bool on) {
	sendTelegram(getTelegram(device,on));
}

void BlokkerTransmitter::sendSignalFmt(int *codes, int size) {
	if (size<2) return;
	sendTelegram(getTelegram((unsigned short)codes[0], (bool)codes[1]));
}

unsigned long BlokkerTransmitter::getTelegram(unsigned short device, bool on) {
	unsigned short trits[12]={0};
	
	device--;
	
	for (unsigned short i=1; i<4; i++) {
		// Trits 1-3 contain device 
		trits[i]=(device & 1)?0:1;          
		device>>=1;
    }
	
	// Switch on or off
	trits[8]=(on?1:0);
	
	return encodeTelegram(trits);
}

/************
* KaKuTransmitter
************/

KaKuTransmitter::KaKuTransmitter(int pi, unsigned short pin, unsigned int periodusec, unsigned short repeats) : RemoteTransmitter(pi, pin,periodusec,repeats) {
	// Call contructor
}

void KaKuTransmitter::sendSignal(char address, unsigned short device, bool on) {
	sendTelegram(getTelegram(address, device, on));
}

void KaKuTransmitter::sendSignalFmt(int *codes, int size) {
	if (size<3) return;
	if (size== 3)
		sendTelegram(getTelegram((char) codes[0], (unsigned short)codes[1], (bool)codes[2]));
	else
		sendTelegram(getTelegram((char) codes[0], (unsigned short)codes[1], (unsigned short)codes[2], (bool)codes[3]));
}

unsigned long KaKuTransmitter::getTelegram(char address, unsigned short device, bool on) {
	unsigned short trits[12];
	
	address-=65;
	device-=1;
	
	for (unsigned short i=0; i<4; i++) {
		// Trits 0-3 contain address (2^4 = 16 addresses)
		trits[i]=(address & 1)?2:0;          
		address>>=1;
		
		// Trits 4-8 contain device (2^4 = 16 addresses)
		trits[i+4]=(device & 1)?2:0;          
		device>>=1;
    }
	
	// Trits 8-10 seem to be fixed
	trits[8]=0;
	trits[9]=2;
	trits[10]=2;
	
	// Switch on or off
	trits[11]=(on?2:0);
	
	return encodeTelegram(trits);
}

void KaKuTransmitter::sendSignal(char address, unsigned short group, unsigned short device, bool on) {
	sendTelegram(getTelegram(address, group, device, on));
}

unsigned long KaKuTransmitter::getTelegram(char address, unsigned short group, unsigned short device, bool on) {
	unsigned short trits[12], i;
	
	address-=65;
	group-=1;
	device-=1;
	
	// Address. M3E Pin A0-A3
	for (i=0; i<4; i++) {
		// Trits 0-3 contain address (2^4 = 16 addresses)
		trits[i]=(address & 1)?2:0;          
		address>>=1;		
    }
		
	// Device. M3E Pin A4-A5
	for (; i<6; i++) {
		trits[i]=(device & 1)?2:0;          
		device>>=1;
	}
	
	// Group. M3E Pin A6-A7
	for (; i<8; i++) {
		trits[i]=(group & 1)?2:0;          
		group>>=1;
	}
	
	// Trits 8-10 are be fixed. M3E Pin A8/D0-A10/D2
	trits[8]=0;
	trits[9]=2;
	trits[10]=2;
	
	// Switch on or off, M3E Pin A11/D3
	trits[11]=(on?2:0);
	
	return encodeTelegram(trits);
}

/************
* ElroTransmitter
************/

ElroTransmitter::ElroTransmitter(int pi, unsigned short pin, unsigned int periodusec, unsigned short repeats) : RemoteTransmitter(pi, pin, periodusec, repeats) {
	//Call contructor
}

void ElroTransmitter::sendSignal(unsigned short systemCode, char device, bool on) {
	sendTelegram(getTelegram(systemCode, device, on));
}

void ElroTransmitter::sendExtSignal(unsigned short systemCode, unsigned short device, bool on) {
	sendTelegram(getExtTelegram(systemCode, device, on));
}

void ElroTransmitter::sendSignalFmt(int *codes, int size) {
	if (size<3) return;
	sendTelegram(getExtTelegram((unsigned short) codes[0], (unsigned short)codes[1], (bool)codes[2]));
}

unsigned long ElroTransmitter::getTelegram(unsigned short systemCode, char device, bool on) {
	unsigned short trits[12];

	device-=65;

	for (unsigned short i=0; i<5; i++) {
		//trits 0-4 contain address (2^5=32 addresses)
		trits[i]=(systemCode & 1)?0:2;
		systemCode>>=1;

		//trits 5-9 contain device. Only one trit has value 0, others have 2 (float)!
		trits[i+5]=(i==(unsigned short)device?0:2);
    }

	//switch on or off
	trits[10]=(on?0:2);
	trits[11]=(!on?0:2);

	return encodeTelegram(trits);
}

unsigned long ElroTransmitter::getExtTelegram(unsigned short systemCode, unsigned short device, bool on) {
	unsigned short trits[12];

	for (unsigned short i=0; i<5; i++) {
		//trits 0-4 contain address (2^5=32 addresses)
		trits[i]=(systemCode & 1)?0:2;
		systemCode>>=1;

		//trits 5-9 contain device. Use all 31 values for extended transmit
		trits[i+5]=(device & 1)?0:2;
		device>>=1;
    }

	//switch on or off
	trits[10]=(on?0:2);
	trits[11]=(!on?0:2);

	return encodeTelegram(trits);
}

// access functions
extern "C" ActionTransmitter* _ActionTransmitter(int pi, short pin, unsigned int periodusec, unsigned short repeats) {
	return new ActionTransmitter(pi, pin, periodusec, repeats);
}

extern "C" void _ActionTransmitter_delete(ActionTransmitter* t) {
	delete t;
}

extern "C" void _ActionTransmitter_sendSignal(ActionTransmitter* t, unsigned short systemCode, char device, bool on) {
	t->sendSignal(systemCode, device, on);
}

extern "C" void _ActionTransmitter_sendSignalFmt(ActionTransmitter* t, int *codes, int size) {
	t->sendSignalFmt(codes, size);	
}

extern "C" unsigned long _ActionTransmitter_getTelegram(ActionTransmitter* t, unsigned short systemCode, char device, bool on) {
	return t->getTelegram(systemCode, device, on);
}

extern "C" BlokkerTransmitter* _BlokkerTransmitter(int pi, short pin, unsigned int periodusec, unsigned short repeats) {
	return new BlokkerTransmitter(pi, pin, periodusec, repeats);
}

extern "C" void _BlokkerTransmitter_delete(BlokkerTransmitter* t) {
	delete t;
}

extern "C" void _BlokkerTransmitter_sendSignal(BlokkerTransmitter* t, unsigned short device, bool on) {
	t->sendSignal(device, on);
}

extern "C" void _BlokkerTransmitter_sendSignalFmt(BlokkerTransmitter* t, int *codes, int size) {
	t->sendSignalFmt(codes, size);	
}

extern "C" unsigned long _BlokkerTransmitter_getTelegram(BlokkerTransmitter* t, unsigned short device, bool on) {
	return t->getTelegram(device, on);
}

extern "C" KaKuTransmitter* _KaKuTransmitter(int pi, short pin, unsigned int periodusec, unsigned short repeats) {
	return new KaKuTransmitter(pi, pin, periodusec, repeats);
}

extern "C" void _KaKuTransmitter_delete(KaKuTransmitter* t) {
	delete t;
}

extern "C" void _KaKuTransmitter_sendSignal(KaKuTransmitter* t, char address, unsigned short device, bool on) {
	t->sendSignal(address, device, on);
}

extern "C" void _KaKuTransmitter_sendExtSignal(KaKuTransmitter* t, char address, unsigned short group, unsigned short device, bool on) {
	t->sendSignal(address, group, device, on);
}

extern "C" void _KaKuTransmitter_sendSignalFmt(KaKuTransmitter* t, int *codes, int size) {
	t->sendSignalFmt(codes, size);	
}

extern "C" unsigned long _KaKuTransmitter_getTelegram(KaKuTransmitter* t, char address, unsigned short device, bool on) {
	return t->getTelegram(address, device, on);
}

extern "C" unsigned long _KaKuTransmitter_getExtTelegram(KaKuTransmitter* t, char address, unsigned short group, unsigned short device, bool on) {
	return t->getTelegram(address, group, device, on);
}

extern "C" ElroTransmitter* _ElroTransmitter(int pi, short pin, unsigned int periodusec, unsigned short repeats) {
	return new ElroTransmitter(pi, pin, periodusec, repeats);
}

extern "C" void _ElroTransmitter_delete(ElroTransmitter* t) {
	delete t;
}

extern "C" void _ElroTransmitter_sendSignal(ElroTransmitter* t, unsigned short systemCode, char device, bool on) {
	t->sendSignal(systemCode, device, on);
}

extern "C" void _ElroTransmitter_sendExtSignal(ElroTransmitter* t, unsigned short systemCode, unsigned short device, bool on) {
	t->sendExtSignal(systemCode, device, on);
}

extern "C" void _ElroTransmitter_sendSignalFmt(ElroTransmitter* t, int *codes, int size) {
	t->sendSignalFmt(codes, size);	
}

extern "C" unsigned long _ElroTransmitter_getTelegram(ElroTransmitter* t, unsigned short systemCode, char device, bool on) {
	return t->getTelegram(systemCode, device, on);
}

extern "C" unsigned long _ElroTransmitter_getExtTelegram(ElroTransmitter* t, unsigned short systemCode, unsigned short device, bool on) {
	return t->getExtTelegram(systemCode, device, on);
}
