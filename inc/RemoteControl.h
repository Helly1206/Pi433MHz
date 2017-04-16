/*
 * RemoteSwitch library v2.2.1 (20120314) made by Randy Simons http://randysimons.nl/
 *
 * License: GPLv3. See license.txt
 *
 * Ported to Raspberry PI (using PIGPIO) by helly
 * GPIO needs to be initialised in calling function .... 
 * Control library to set control signals for transceivers like Aurel RTX-MID-xV
 *
 */

#ifndef RemoteControl_h
#define RemoteControl_h

class RemoteControl {
	public:
		/**Constructor Initializes the decoder.
		* 
		*
		* @param pi 		Current pi handler from daemon
		* @param enapin		GPIO pin on rPi that will be used to enable transceiver.
		* @param txrxpin	GPIO pin on rPi that will be used to switch tx and rx.		
		* @param chargepll	If true, the PLL needs to be recharged when entering Rx mode (Aurel RTX-MIDxV).					
		*/
		RemoteControl(int pi, short enapin, short txrxpin, bool chargepll);
		
		/**
		* Enable Transceiver. Defaults to Rx mode
		*/
		void enable();
		
		/**
		* Disable Transceiver. You can re-enable Transceiver by calling enable();
		*/
		void disable();
		
		/**
		* Tells wether the Transceiver is enabled. 
		*
		* @return boolean If enabled
		*/
		bool isEnabled();

		/**
		* Switch Transceiver to Rx mode.
		*/
		void Rx();
		
		/**
		* Switch Transceiver to Tx mode.
		*/
		void Tx();

		/**
		* Tells wether the Transceiver is in Tx mode. 
		*
		* @return boolean If Tx mode: true, if Rx mode: false
		*/
		bool isTx();
	
	private:
		int _pi;
		short _enapin;
		short _txrxpin;
		bool _chargepll;
		bool _enabled;
		bool _tx;
		bool _initok;

		void enable2();
		void Rx2();

};

extern "C" RemoteControl* _RemoteControl(int pi, short enapin, short txrxpin, bool chargepll);
extern "C" void _RemoteControl_delete(RemoteControl* t);
extern "C" void _RemoteControl_enable(RemoteControl* t);
extern "C" void _RemoteControl_disable(RemoteControl* t);
extern "C" void _RemoteControl_Rx(RemoteControl* t);
extern "C" void _RemoteControl_Tx(RemoteControl* t);
extern "C" bool _RemoteControl_isEnabled(RemoteControl* t);
extern "C" bool _RemoteControl_isTx(RemoteControl* t);


#endif
