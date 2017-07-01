/*
 * Pi433MHzd.h
 * Headerfile for the raspberry pi 433MHz daemon
 *
 * Copyright 26-12-2016 Helly
 */

#ifndef PI433MHZD_H
#define PI433MHZD_H

#define RES_OK     0
#define RES_NOK    -1
#define RES_OFF    -5

#define DAEMON_VERSION 0.92

/* defines */
class Messages {
	public:
		qmsg* XqMsg;
		msg* TxMsg;
		smsg* RxMsg;
		bmsg* RxbMsg;
};

class ThreadArgs {
	public:
    	Messages *msgs;
    	int intarg;
    	int intarg2;
};

class Pi433MHzd {
	public:
		// Constructor
		Pi433MHzd() {};
		virtual ~Pi433MHzd() {};
		void usage();
		int Entry();
		void RunDaemon();
		int Exit();
		static void *pthSocketThread(void *vargs);
		static void *pthServerThread(void *vargs);
		static void sig_handler(int signo);
		static void RxCallback(unsigned long receivedCode, unsigned int period);
		static void EchoCallback(std::string receivedData);
	private:
		int InitRemoteControl();
		int InitTxDevice();
		int InitRxDevice();
		int TxTransmit(std::string data);
		int RxReceive(std::string &data);
		int RxEnable(bool ena);
		void TxRx(bool Tx);
		Logger *logger;
		Config *config;
		Messages *msgs;
		pthread_t pthServer;
		void *RxDevice;
		void *TxDevice;
		RemoteControl *Control;
		int pi;
		static unsigned long lastcmdtime;
		static unsigned long lastcode;
		static unsigned int ignoretime;
};

#endif
