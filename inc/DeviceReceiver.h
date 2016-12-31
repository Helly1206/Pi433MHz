/* 
 * RemoteSwitch library v2.2.1 (20120314) made by Randy Simons http://randysimons.nl/
 *
 * License: GPLv3. See license.txt
 *
 * Devices are extracted
 * Ported to Raspberry PI (using PIGPIO) by helly
 * v1.0.0 (20161224)
 *
 */

#ifndef DeviceReceiver_h
#define DeviceReceiver_h

#include "RemoteReceiver.h"

class ActionReceiver: public RemoteReceiver {
	public:
		/** Constructor
		* Initializes the decoder.
		*
		* @param pi 		Current pi handler from daemon
		* @param Pint 		GPIO pin on rPi that will be used as interrupt.
		* @param minRepeats The number of times the same code must be received in a row before the callback is calles
		* @param callback Pointer to a callback function, with signature void (*func)(unsigned long, unsigned int). First parameter is the decoded data, the second the period of the timing.
		*/
		ActionReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback);
		
		/**
		* codes the signal to system, device and on/off code
		* lcode is the received telegram code
		*
		* @return true if data correct, false if not
		*/
		bool signalDecode(unsigned long lCode, uint8_t *sysCde, uint8_t *devCde, bool *on);
		bool signalDecodeFmt(unsigned long lCode, int *codes, int maxsize);
};

class BlokkerReceiver: public RemoteReceiver {
	public:
		/** Constructor
		* Initializes the decoder.
		*
		* @param pi 		Current pi handler from daemon
		* @param pin 		GPIO pin on rPi that will be used as interrupt.
		* @param minRepeats The number of times the same code must be received in a row before the callback is calles
		* @param callback Pointer to a callback function, with signature void (*func)(unsigned long, unsigned int). First parameter is the decoded data, the second the period of the timing.
		*/
		BlokkerReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback);
		
		/**
		* codes the signal to device and on/off code
		* lcode is the received telegram code
		*
		* @return true if data correct, false if not
		*/
		bool signalDecode(unsigned long lCode, unsigned short *device, bool *on);
		bool signalDecodeFmt(unsigned long lCode, int *codes, int maxsize);
};

class KaKuReceiver: public RemoteReceiver {
	public:
		/** Constructor
		* Initializes the decoder.
		* 
		* @param pi 		Current pi handler from daemon
		* @param pin 		GPIO pin on rPi that will be used as interrupt.
		* @param minRepeats The number of times the same code must be received in a row before the callback is calles
		* @param callback Pointer to a callback function, with signature void (*func)(unsigned long, unsigned int). First parameter is the decoded data, the second the period of the timing.
		*/
		KaKuReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback);
		
		/**
		* codes the signal to address, device and on/off code (for devices using groups, device[2..3]=group, device[0..1]=device)
		* lcode is the received telegram code
		*
		* @return true if data correct, false if not
		*/
		bool signalDecode(unsigned long lCode, char *address, unsigned short *device, bool *on);
		bool signalDecodeFmt(unsigned long lCode, int *codes, int maxsize);
};

class ElroReceiver: public RemoteReceiver {
	public:
		/** Constructor
		* Initializes the decoder.
		*
		* @param pi 		Current pi handler from daemon
		* @param pin 		GPIO pin on rPi that will be used as interrupt.
		* @param minRepeats The number of times the same code must be received in a row before the callback is calles
		* @param callback Pointer to a callback function, with signature void (*func)(unsigned long, unsigned int). First parameter is the decoded data, the second the period of the timing.
		*/
		ElroReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback);
		
		/**
		* codes the signal to system, device and on/off code
		* lcode is the received telegram code
		*
		* @return true if data correct, false if not
		*/
		bool signalDecode(unsigned long lCode, uint8_t *sysCde, uint8_t *devCde, bool *on);
		bool signalDecodeFmt(unsigned long lCode, int *codes, int maxsize);
		
};

extern "C" ActionReceiver* _ActionReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback);
extern "C" void _ActionReceiver_delete(ActionReceiver* t);
extern "C" bool _ActionReceiver_signalDecode(ActionReceiver* t, unsigned long lCode, uint8_t *sysCde, uint8_t *devCde, bool *on);
extern "C" bool _ActionReceiver_signalDecodeFmt(ActionReceiver* t, unsigned long lCode, int *codes, int maxsize);

extern "C" BlokkerReceiver* _BlokkerReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback);
extern "C" void _BlokkerReceiver_delete(BlokkerReceiver* t);
extern "C" bool _BlokkerReceiver_signalDecode(BlokkerReceiver* t, unsigned long lCode, unsigned short *device, bool *on);
extern "C" bool _BlokkerReceiver_signalDecodeFmt(BlokkerReceiver* t, unsigned long lCode, int *codes, int maxsize);

extern "C" KaKuReceiver* _KaKuReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback);
extern "C" void _KaKuReceiver_delete(KaKuReceiver* t);
extern "C" bool _KaKuReceiver_signalDecode(KaKuReceiver* t, unsigned long lCode, char *address, unsigned short *device, bool *on);
extern "C" bool _KaKuReceiver_signalDecodeFmt(KaKuReceiver* t, unsigned long lCode, int *codes, int maxsize);

extern "C" ElroReceiver* _ElroReceiver(int pi, short pin, unsigned short minRepeats, RemoteReceiverCallBack callback);
extern "C" void _ElroReceiver_delete(ElroReceiver* t);
extern "C" bool _ElroReceiver_signalDecode(ElroReceiver* t, unsigned long lCode, uint8_t *sysCde, uint8_t *devCde, bool *on);
extern "C" bool _ElroReceiver_signalDecodeFmt(ElroReceiver* t, unsigned long lCode, int *codes, int maxsize);

#endif
