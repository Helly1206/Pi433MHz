/*
 * config.h
 * Headerfile for reading and storing configuration for ...
 *
 * Copyright 26-12-2016 Helly
 */

#ifndef CONFIG_H
#define CONFIG_H

/* defines */
#define STR_LOGLEVEL   	"LOGLEVEL"
#define STR_LOGFILE    	"LOGFILE"
#define STR_PORT       	"PORT"
#define STR_MAXCLIENTS 	"MAXCLIENTS"
#define STR_RXDEVICE   	"RXDEVICE"
#define STR_RXREPEATS  	"RXREPEATS"
#define STR_RXGPIO     	"RXGPIO"
#define STR_RXQUEUE    	"RXQUEUE"
#define STR_RXECHO     	"RXECHO"
#define STR_TXDEVICE   	"TXDEVICE"
#define STR_TXPERIOD   	"TXPERIOD"
#define STR_TXREPEATS  	"TXREPEATS"
#define STR_TXGPIO     	"TXGPIO"
#define STR_TXQUEUE    	"TXQUEUE"
#define STR_PLLCHARGE   "PLLCHARGE"
#define STR_TXRXGPIO    "TXRXGPIO"
#define STR_ENABLEGPIO  "ENABLEGPIO"

#define STR_OFF		   	"off"
#define STR_ECHO	   	"echo"
#define STR_RAW		   	"raw"
#define STR_ACTION	   	"action"
#define STR_BLOKKER	   	"blokker"
#define STR_KAKU	   	"kaku"
#define STR_ELRO	   	"elro"

#define X_OFF		   	0
#define X_ECHO	   	   	1
#define X_RAW		   	2
#define X_ACTION	   	3
#define X_BLOKKER	   	4
#define X_KAKU	   	   	5
#define X_ELRO	   	   	6

#define STR_NONE	   	"none"
#define STR_NORMAL	   	"normal"
#define STR_DEBUG	   	"debug"

#define LOG_NONE	   	0
#define LOG_NORMAL	   	1
#define LOG_DEBUG	   	2

#define TRIMCHARS       " \t\n\r\f\v"
#define MAX_LINE    	100
#define CFG_DEF_PATH   	"/etc/"
#define CFG_DEF_LOG     1
#define CFG_DEF_LOGFILE	"/var/log/Pi433HMzd.log"
#define CFG_DEF_PORT	51721
#define CFG_DEF_MAXCL	5
#define CFG_DEF_X       X_RAW
#define CFG_DEF_RXRPTS  3
#define CFG_DEF_RXGPIO  3
#define CFG_DEF_RXQ     5
#define CFG_DEF_TXRPTS  4
#define CFG_DEF_TXGPIO  2
#define CFG_DEF_TXQ     5
#define CFG_DEF_PLLCH   0
#define CFG_DEF_TRXGPIO 27
#define CFG_DEF_ENAGPIO 22

#define PER_DEF_RAW     320
#define PER_DEF_ACTION  190
#define PER_DEF_BLOKKER 230
#define PER_DEF_KAKU    375
#define PER_DEF_ELRO    320

class Config {
	public:
		// Constructor
		Config();
		// Get filename
		std::string GetFileName(std::string FileName, std::string EtcLoc = CFG_DEF_PATH);
		// Load or Reload
		int Load(std::string FileName);
		void PrintConfig();

		uint8_t LogLevel;
		std::string LogFile;
		unsigned short Port;
		uint8_t MaxClients;
		uint8_t RxDevice;
		uint8_t RxRepeats;
		uint8_t RxGpio;
		uint8_t RxQueue;
		uint8_t RxEcho;
		uint8_t TxDevice;
		uint8_t TxPeriod;
		uint8_t TxRepeats;
		uint8_t TxGpio;
		uint8_t TxQueue;
		uint8_t PllCharge;
		uint8_t TxRxGpio;
		uint8_t EnableGpio;
	private:
		void SetDefaults();
		std::string& trim(std::string& s, const char* t = TRIMCHARS);
		void ParseLine(std::string s);
		void StoreSetting(std::string Param, std::string Value);
		uint8_t GetDevice(std::string Value);
};

#endif
