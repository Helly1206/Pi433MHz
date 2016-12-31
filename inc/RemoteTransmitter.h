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

#ifndef RemoteTransmitter_h
#define RemoteTransmitter_h

/**
* RemoteTransmitter provides a generic class for simulation of common RF remote controls, like the 'Klik aan Klik uit'-system 
* (http://www.klikaanklikuit.nl/), used to remotely switch lights etc.
*
* Many of these remotes seem to use a 433MHz SAW resonator and one of these chips: LP801B,  HX2262, PT2262, M3E.
* Datasheet for the HX2262/PT2262 ICs:
* http://www.princeton.com.tw/downloadprocess/downloadfile.asp?mydownload=PT2262_1.pdf
*
* Hardware required for this library: a 433MHz/434MHz SAW oscillator transmitter, e.g.
* http://www.sparkfun.com/products/10534
* http://www.conrad.nl/goto/?product=130428
*
* Notes:
* - Since these chips use (and send!) tri-state inputs (low, high and floating) I use 'trits' instead of 'bits',
*   when appropriate.
* - I measured the period lengths with a scope.  Thus: they work for my remotes, but may fail for yours...
*   A better way would be to calculate the 'target'-timings using the datasheets and the resistor-values on the remotes. 
*/
class RemoteTransmitter {
	public:
		/**
		* Constructor.
		*
		* To obtain the correct period length, an oscilloscope is convenient, but you can also read the
		* datasheet of the transmitter, measure the resistor for the oscillator and calculate the freqency.
		*
		* @param pi 		Current pi handler from daemon
		* @param pin		GPIO pin on rPi to which the transmitter is connected
		* @param periodusec	[0..511] Duration of one period, in microseconds. A trit is 6 periods.
		* @param repeats	[0..7] The 2log-Number of times the signal is repeated. The actual number of repeats will be 2^repeats. 2 would be bare minimum, 4 seems robust.
		*/		
		RemoteTransmitter(int pi, unsigned short pin, unsigned int periodusec, unsigned short repeats);
		
		/**
		* Encodes the data base on the current object and the given trits. The data can be reused, e.g.
		* for use with the static version of sendTelegram, so you won't need to instantiate costly objects!
		*
		* @return The data suited for use with RemoteTransmitter::sendTelegram.
		*/
		unsigned long encodeTelegram(unsigned short trits[]);
		
		/**
		* Send a telegram, including synchronisation-part.
		*
		* @param trits	Array of size 12. "trits" should be either 0, 1 or 2, where 2 indicaties "float"
		*/
		void sendTelegram(unsigned short trits[]);
		
		/**
		* Send a telegram, including synchronisation-part. The data-param encodes the period duration, number of repeats and the actual data.
		* Note: static method, which allows for use in low-mem situations.
		*
		* Format data:
		* pppppppp|prrrdddd|dddddddd|dddddddd (32 bit)
		* p = period (9 bit unsigned int
		* r = repeats as 2log. Thus, if r = 3, then signal is sent 2^3=8 times
		* d = data
		*
		* @param data data, period and repeats.
		*/
		void sendTelegram(unsigned long data);
		
		/**
		* The complement of RemoteReceiver.	Send a telegram with specified code as data.
		*
		* Note: static method, which allows for use in low-mem situations.
		*
		* @param code		The code to transmit. Note: only the first 20 bits are used, the rest is ignored. Also see sendTelegram.
		*/		
		void sendCode(unsigned long code);
		
		/**
		* Compares the data received with RemoteReceive with the data obtained by one of the getTelegram-functions.
		* Period duration and repetitions are ignored by this function; only the data-payload is compared.
		*
		* @return true, if the codes are identical (the 20 least significant bits match)
		*/
		bool isSameCode(unsigned long encodedTelegram, unsigned long receivedData);
		
	protected:
		//unsigned short _pin;		// Transmitter output pin
		//unsigned int _periodusec;	// Oscillator period in microseconds
		bool _initok;
		int _pi;					// Current pi handler from daemon
		unsigned short _repeats;	// Number over repetitions of one telegram	
		int _wave[4];				// Number of different waves to send
};

extern "C" RemoteTransmitter* _RemoteTransmitter(int pi, short pin, unsigned int periodusec, unsigned short repeats);
extern "C" void _RemoteTransmitter_delete(RemoteTransmitter* t);
extern "C" unsigned long _RemoteTransmitter_encodeTelegram(RemoteTransmitter* t, unsigned short trits[]);
extern "C" void _RemoteTransmitter_sendTelegram(RemoteTransmitter* t, unsigned short trits[]);
extern "C" void _RemoteTransmitter_sendCode(RemoteTransmitter* t, unsigned long code);
extern "C" bool _RemoteTransmitter_isSameCode(RemoteTransmitter* t, unsigned long encodedTelegram, unsigned long receivedData);

#endif