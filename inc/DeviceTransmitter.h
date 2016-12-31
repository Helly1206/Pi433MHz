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

#ifndef DeviceTransmitter_h
#define DeviceTransmitter_h

#include "RemoteTransmitter.h"

/**
* ActionTransmitter simulatos a remote, as sold in the Dutch 'Action' stores. But there are many similar systems on the market.
* If your remote has setting for 5 address bits, and can control 5 devices on or off, then you can try to use the ActionTransmitter.
* Otherwise you may have luck with the ElroTransmitter, which is similar.
*/
class ActionTransmitter: public RemoteTransmitter {
	public:
		/**
		* Constructor
		*
		* @param pi 		Current pi handler from daemon
		* @param pin		GPIO pin on rPi to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 190usec
		* @param repeats	[0..7] The 2log-Number of times the signal is repeated.
		* @see RemoteTransmitter
		*/
		ActionTransmitter(int pi, unsigned short pin, unsigned int periodusec=190, unsigned short repeats=4);
		
		/**
		* Send a on or off signal to a device.
		*
		* @param systemCode	5-bit addres (dip switches in remote). Range [0..31]
		* @param device	Device to switch. Range: [A..E] (case sensitive!)
		* @param on	True, to switch on. False to switch off,
		*/
		void sendSignal(unsigned short systemCode, char device, bool on);
		void sendSignalFmt(int *codes, int size);
		
		/**
		* Generates the telegram (data) which can be used for RemoteTransmitter::sendTelegram.
		* See sendSignal for details on the parameters
		*
		* @return Encoded data, including repeats and period duration.
		*/
		unsigned long getTelegram(unsigned short systemCode, char device, bool on);
};

/**
* BlokkerTransmitter simulatos a remote, as sold in the Dutch 'Blokker' stores. But there are many similar systems on the market.
* These remotes have 4 on, 4 off buttons and a switch to switch between device 1-4 and 5-8. No futher configuration
* possible.
*/
class BlokkerTransmitter: public RemoteTransmitter {
	public:
		/**
		* Constructor
		*
		* @param pi 		Current pi handler from daemon
		* @param pin		GPIO pin on rPi to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 307usec
		* @param repeats	[0..7] The 2log-Number of times the signal is repeated.
		* @see RemoteTransmitter
		*/
		BlokkerTransmitter(int pi, unsigned short pin, unsigned int periodusec=230, unsigned short repeats=4);
		
		/**
		* Send a on or off signal to a device.
		*		
		* @param device	Device to switch. Range: [1..8]
		* @param on	True, to switch on. False to switch off,
		*/
		void sendSignal(unsigned short device, bool on);
		void sendSignalFmt(int *codes, int size);
		
		/**
		* @see RemoteTransmitter::getTelegram
		*/
		unsigned long getTelegram(unsigned short device, bool on);
};

/**
* KaKuTransmitter simulates a KlikAanKlikUit-remote, but there are many clones.
* If your transmitter has a address dial with the characters A till P, you can try this class.
*/
class KaKuTransmitter: public RemoteTransmitter {
	public:
		/**
		* Constructor
		*
		* @param pi 		Current pi handler from daemon
		* @param pin		GPIO pin on rPi to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 375usec
		* @param repeats	[0..7] The 2log-Number of times the signal is repeated.
		* @see RemoteTransmitter
		*/
		KaKuTransmitter(int pi, unsigned short pin, unsigned int periodusec=375, unsigned short repeats=4);
		
		/**
		* Send a on or off signal to a device.
		*
		* @param address	addres (dial switches in remote). Range [A..P] (case sensitive!)
		* @param group	Group to switch. Range: [1..4]
		* @param device	Device to switch. Range: [1..4]
		* @param on	True, to switch on. False to switch off,
		*/
		void sendSignal(char address, unsigned short group, unsigned short device, bool on);
		
		/**
		* Send a on or off signal to a device.
		*
		* @param address	addres (dip switches in remote). Range [A..P] (case sensitive!)
		* @param device	device (dial switches in remote). Range [1..16]
		* @param on	True, to switch on. False to switch off,
		*/
		void sendSignal(char address, unsigned short device, bool on);
		void sendSignalFmt(int *codes, int size);
		
		/**
		* @see RemoteTransmitter::getTelegram
		*/
		unsigned long getTelegram(char address, unsigned short group, unsigned short device, bool on);
		
		/**
		* @see RemoteTransmitter::getTelegram
		*/
		unsigned long getTelegram(char address, unsigned short device, bool on);
};

/**
* ElroTransmitter simulates remotes of the Elro "Home Control" series
* see http://www.elro.eu/en/m/products/category/home_automation/home_control
* There are are many similar systems on the market. If your remote has setting for 5 address bits, and can control
* 4 devices on or off, then you can try to use the ElroTransmitter. Otherwise you may have luck with the
* ActionTransmitter, which is similar.
*/
class ElroTransmitter: public RemoteTransmitter {
	public:
		/**
		* Constructor
		*
		* @param pi 		Current pi handler from daemon
		* @param pin		GPIO pin on rPi to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 320usec
		* @param repeats	[0..7] The 2log-Number of times the signal is repeated.
		* @see RemoteSwitch
		*/
		ElroTransmitter(int pi, unsigned short pin, unsigned int periodusec=320, unsigned short repeats=4);

		/**
		* Send a on or off signal to a device.
		*
		* @param systemCode	5-bit addres (dip switches in remote). Range [0..31]
		* @param device	Device to switch. Range: [A..D] (case sensitive!)
		* @param on	True, to switch on. False to switch off,
		*/
		void sendSignal(unsigned short systemCode, char device, bool on);
		
		/**
		* Send a on or off signal to a device.
		*
		* @param systemCode	5-bit addres (dip switches in remote). Range [0..31]
		* @param device	Device to switch. Extened device range. Range: [0..31] 
		* @param on	True, to switch on. False to switch off,
		*/
		void sendExtSignal(unsigned short systemCode, unsigned short device, bool on);
		void sendSignalFmt(int *codes, int size);

		/**
		* Generates the telegram (data) which can be used for RemoteSwitch::sendTelegram.
		* See sendSignal for details on the parameters
		*
		* @return Encoded data, including repeats and period duration.
		*/
		unsigned long getTelegram(unsigned short systemCode, char device, bool on);
		
		/**
		* Generates the telegram (data) which can be used for RemoteSwitch::sendTelegram.
		* To be used with extended signals
		* See sendExtSignal for details on the parameters
		*
		* @return Encoded data, including repeats and period duration.
		*/
		unsigned long getExtTelegram(unsigned short systemCode, unsigned short device, bool on);
};

extern "C" ActionTransmitter* _ActionTransmitter(int pi, short pin, unsigned int periodusec=190, unsigned short repeats=4);
extern "C" void _ActionTransmitter_delete(ActionTransmitter* t);
extern "C" void _ActionTransmitter_sendSignal(ActionTransmitter* t, unsigned short systemCode, char device, bool on);
extern "C" void _ActionTransmitter_sendSignalFmt(ActionTransmitter* t, int *codes, int size);
extern "C" unsigned long _ActionTransmitter_getTelegram(ActionTransmitter* t, unsigned short systemCode, char device, bool on);

extern "C" BlokkerTransmitter* _BlokkerTransmitter(int pi, short pin, unsigned int periodusec=230, unsigned short repeats=4);
extern "C" void _BlokkerTransmitter_delete(BlokkerTransmitter* t);
extern "C" void _BlokkerTransmitter_sendSignal(BlokkerTransmitter* t, unsigned short device, bool on);
extern "C" void _BlokkerTransmitter_sendSignalFmt(BlokkerTransmitter* t, int *codes, int size);
extern "C" unsigned long _BlokkerTransmitter_getTelegram(BlokkerTransmitter* t, unsigned short device, bool on);

extern "C" KaKuTransmitter* _KaKuTransmitter(int pi, short pin, unsigned int periodusec=375, unsigned short repeats=4);
extern "C" void _KaKuTransmitter_delete(KaKuTransmitter* t);
extern "C" void _KaKuTransmitter_sendSignal(KaKuTransmitter* t, char address, unsigned short device, bool on);
extern "C" void _KaKuTransmitter_sendExtSignal(KaKuTransmitter* t, char address, unsigned short group, unsigned short device, bool on);
extern "C" void _KaKuTransmitter_sendSignalFmt(KaKuTransmitter* t, int *codes, int size);
extern "C" unsigned long _KaKuTransmitter_getTelegram(KaKuTransmitter* t, char address, unsigned short device, bool on);
extern "C" unsigned long _KaKuTransmitter_getExtTelegram(KaKuTransmitter* t, char address, unsigned short group, unsigned short device, bool on);

extern "C" ElroTransmitter* _ElroTransmitter(int pi, short pin, unsigned int periodusec=320, unsigned short repeats=4);
extern "C" void _ElroTransmitter_delete(ElroTransmitter* t);
extern "C" void _ElroTransmitter_sendSignal(ElroTransmitter* t, unsigned short systemCode, char device, bool on);
extern "C" void _ElroTransmitter_sendExtSignal(ElroTransmitter* t, unsigned short systemCode, unsigned short device, bool on);
extern "C" void _ElroTransmitter_sendSignalFmt(ElroTransmitter* t, int *codes, int size);
extern "C" unsigned long _ElroTransmitter_getTelegram(ElroTransmitter* t, unsigned short systemCode, char device, bool on);
extern "C" unsigned long _ElroTransmitter_getExtTelegram(ElroTransmitter* t, unsigned short systemCode, unsigned short device, bool on);

#endif