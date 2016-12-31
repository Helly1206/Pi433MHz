/* 
 * RemoteSwitch library v2.2.1 (20120314) made by Randy Simons http://randysimons.nl/
 * See RemoteReceiver.h for details.
 *
 * License: GPLv3. See license.txt
 *
 * Devices are extracted
 * Ported to Raspberry PI (using PIGPIO) by helly
 * v1.0.0 (20161224)
 *
 */

#include <stdint.h>
#include "DeviceReceiver.h"

/************
* ActionReceiver
************/
ActionReceiver::ActionReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback) : RemoteReceiver(pi, pin, minRepeats, callback) {
	// Call contructor
}

bool ActionReceiver::signalDecode(unsigned long lCode, uint8_t *sysCde, uint8_t *devCde, bool *on) {
	unsigned short Decoded=0;
	bool on2;

	Decoded = decodeTelegram(lCode);

	//trits 0-4 contain address (2^5=32 addresses)
	*sysCde = Decoded & 0x1F;    //11111
	Decoded>>=5;

	//trits 5-9 contain device. Only one trit has value 0, others have 2 (float)!
	*devCde = Decoded & 0x1F;
	Decoded>>=5;
  
	//switch on or off trits 10-11
	*on = (Decoded&1);
	on2 = ((Decoded&2)?0:1);
  
	if (on2 == *on) {
		return (true);
	} else {
		return (false);
	}
}

bool ActionReceiver::signalDecodeFmt(unsigned long lCode, int *codes, int maxsize) {
	unsigned short Decoded=0;
	int on2;
	if (maxsize < 3) return (false);

	Decoded = decodeTelegram(lCode);

	//trits 0-4 contain address (2^5=32 addresses)
	codes[0] = Decoded & 0x1F;    //11111
	Decoded>>=5;

	//trits 5-9 contain device. Only one trit has value 0, others have 2 (float)!
	codes[1] = Decoded & 0x1F;
	Decoded>>=5;

	//switch on or off trits 10-11
	codes[2] = (Decoded&1);
	on2 = ((Decoded&2)?0:1);

	if (on2 == codes[2]) {
		return (true);
	} else {
		return (false);
	}
}

/************
* BlokkerReceiver
************/
BlokkerReceiver::BlokkerReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback) : RemoteReceiver(pi, pin, minRepeats, callback) {
	// Call contructor
}

bool BlokkerReceiver::signalDecode(unsigned long lCode, unsigned short *device, bool *on) {
	unsigned short Decoded=0;

	Decoded = decodeTelegram(lCode);

	//trits 0-3 contain device (2^4=16 devices, bit 0 is always zero)
	*device = Decoded & 0x0F;    //1111
	Decoded>>=4;

	//trits 4-7 contain no important information;
	Decoded>>=4;

	//on or off trit 8
	*on = (Decoded&1);

	return (true);
}

bool BlokkerReceiver::signalDecodeFmt(unsigned long lCode, int *codes, int maxsize) {
	unsigned short Decoded=0;
	if (maxsize < 2) return (false);

	Decoded = decodeTelegram(lCode);

	//trits 0-3 contain device (2^4=16 devices, bit 0 is always zero)
	codes[0] = Decoded & 0x0F;    //1111
	Decoded>>=4;

	//trits 4-7 contain no important information;
	Decoded>>=4;

	//on or off trit 8
	codes[1] = (Decoded&1);

	return (true);
}

/************
* KaKuReceiver
************/
KaKuReceiver::KaKuReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback) : RemoteReceiver(pi, pin, minRepeats, callback) {
	// Call contructor
}

bool KaKuReceiver::signalDecode(unsigned long lCode, char *address, unsigned short *device, bool *on) {
	unsigned short Decoded=0;

	Decoded = decodeTelegram(lCode);

	//trits 0-3 contain address (2^4=16 addresses)
	*address = Decoded & 0x0F;    //1111
	Decoded>>=4;

	//trits 4-7 contain device. (2^4=16 addresses)
	*device = Decoded & 0x0F;
	Decoded>>=4;

	// trits 8-10 seem to be fixed
	Decoded>>=3;  

	//switch on or off trit 11
	*on = (Decoded&1);

	return (true);
}

bool KaKuReceiver::signalDecodeFmt(unsigned long lCode, int *codes, int maxsize) {
	unsigned short Decoded=0;
	if (maxsize < 3) return (false);

	Decoded = decodeTelegram(lCode);

	//trits 0-3 contain address (2^4=16 addresses)
	codes[0] = Decoded & 0x0F;    //1111
	Decoded>>=4;

	//trits 4-7 contain device. (2^4=16 addresses)
	codes[1] = Decoded & 0x0F;
	Decoded>>=4;

	// trits 8-10 seem to be fixed
	Decoded>>=3;  

	//switch on or off trit 11
	codes[2] = (Decoded&1);

	return (true);
}

/************
* ElroReceiver
************/
ElroReceiver::ElroReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback) : RemoteReceiver(pi, pin, minRepeats, callback) {
	// Call contructor
}

bool ElroReceiver::signalDecode(unsigned long lCode, uint8_t *sysCde, uint8_t *devCde, bool *on) {
	unsigned short Decoded=0;
	bool on2;

	Decoded = decodeTelegram(lCode);

	//trits 0-4 contain address (2^5=32 addresses)
	*sysCde = Decoded & 0x1F;    //11111
	Decoded>>=5;

	//trits 5-9 contain device. Only one trit has value 0, others have 2 (float)!
	*devCde = Decoded & 0x1F;
	Decoded>>=5;

	//switch on or off trits 10-11
	*on = (Decoded&1);
	on2 = ((Decoded&2)?0:1);

	if (on2 == *on) {
		return (true);
	} else {
		return (false);
	}
}

bool ElroReceiver::signalDecodeFmt(unsigned long lCode, int *codes, int maxsize) {
	unsigned short Decoded=0;
	int on2;
	if (maxsize < 3) return (false);

	Decoded = decodeTelegram(lCode);

	//trits 0-4 contain address (2^5=32 addresses)
	codes[0] = Decoded & 0x1F;    //11111
	Decoded>>=5;

	//trits 5-9 contain device. Only one trit has value 0, others have 2 (float)!
	codes[1] = Decoded & 0x1F;
	Decoded>>=5;

	//switch on or off trits 10-11
	codes[2] = (Decoded&1);
	on2 = ((Decoded&2)?0:1);

	if (on2 == codes[2]) {
		return (true);
	} else {
		return (false);
	}
}

// access functions
extern "C" ActionReceiver* _ActionReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback) {
	return new ActionReceiver(pi, pin, minRepeats, callback);
}

extern "C" void _ActionReceiver_delete(ActionReceiver* t) {
	delete t;
}

extern "C" bool _ActionReceiver_signalDecode(ActionReceiver* t, unsigned long lCode, uint8_t *sysCde, uint8_t *devCde, bool *on) {
	return t->signalDecode(lCode, sysCde, devCde, on);
}

extern "C" bool _ActionReceiver_signalDecodeFmt(ActionReceiver* t, unsigned long lCode, int *codes, int maxsize) {
	return t->signalDecodeFmt(lCode, codes, maxsize);
}

extern "C" BlokkerReceiver* _BlokkerReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback) {
	return new BlokkerReceiver(pi, pin, minRepeats, callback);
}

extern "C" void _BlokkerReceiver_delete(BlokkerReceiver* t) {
	delete t;
}

extern "C" bool _BlokkerReceiver_signalDecode(BlokkerReceiver* t, unsigned long lCode, unsigned short *device, bool *on) {
	return t->signalDecode(lCode, device, on);
}

extern "C" bool _BlokkerReceiver_signalDecodeFmt(BlokkerReceiver* t, unsigned long lCode, int *codes, int maxsize) {
	return t->signalDecodeFmt(lCode, codes, maxsize);
}

extern "C" KaKuReceiver* _KaKuReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback) {
	return new KaKuReceiver(pi, pin, minRepeats, callback);
}

extern "C" void _KaKuReceiver_delete(KaKuReceiver* t) {
	delete t;
}

extern "C" bool _KaKuReceiver_signalDecode(KaKuReceiver* t, unsigned long lCode, char *address, unsigned short *device, bool *on) {
	return t->signalDecode(lCode, address, device, on);
}

extern "C" bool _KaKuReceiver_signalDecodeFmt(KaKuReceiver* t, unsigned long lCode, int *codes, int maxsize) {
	return t->signalDecodeFmt(lCode, codes, maxsize);
}

extern "C" ElroReceiver* _ElroReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback) {
	return new ElroReceiver(pi, pin, minRepeats, callback);
}

extern "C" void _ElroReceiver_delete(ElroReceiver* t) {
	delete t;
}

extern "C" bool _ElroReceiver_signalDecode(ElroReceiver* t, unsigned long lCode, uint8_t *sysCde, uint8_t *devCde, bool *on) {
	return t->signalDecode(lCode, sysCde, devCde, on);
}

extern "C" bool _ElroReceiver_signalDecodeFmt(ElroReceiver* t, unsigned long lCode, int *codes, int maxsize) {
	return t->signalDecodeFmt(lCode, codes, maxsize);
}
