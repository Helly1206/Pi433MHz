#include <pigpiod_if2.h>
#include <cstdio>
#include <ctime>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#include "RemoteTransmitter.h"
#include "RemoteReceiver.h"
#include "RemoteControl.h"
#include "DeviceTransmitter.h"
#include "DeviceReceiver.h"
#include "config.h"
#include "logger.h"
#include "socket.h"
#include "msg.h"
#include "format.h"
#include "Pi433MHzd.h"

#define MAXFMTSIZE 3

using namespace std;

unsigned long Pi433MHzd::lastcmdtime = 0;
unsigned long Pi433MHzd::lastcode = 0;
unsigned int Pi433MHzd::ignoretime = 0;

void *Pi433MHzd::pthSocketThread(void *vargs) {
	ThreadArgs *args = (ThreadArgs *)vargs;
	Messages *msgs = (Messages *)args->msgs;
	Socket socket;
	socket.set_id(args->intarg);
	int msgid = -1;
	bool disconnected = false;

	delete args;
	if (!msgs->RxbMsg->subscribe(msgid)) {
		Logger::Log(LOG_ERROR,"Socket (%d) subscription not allowed, exiting",msgid);	
		pthread_exit(NULL);
	}

	//Logger::Log(LOG_DEBUG,"SocketThread (%d) running...",msgid);
	Logger::Log(LOG_NOTICE,"Connected to client (%d)",msgid);

	try {
		while ((!msgs->XqMsg->qget()) && (!disconnected)) {
			// write if hasmessage
			if (msgs->RxbMsg->hasmessage(msgid)) {
				std::string data;
				if (msgs->RxbMsg->read(msgid,data)) {
					socket << data;
				}
			}
			// poll and try read
			bool pollres = socket.poll(100);
			if (pollres) {
				std::string data;
				socket >> data;
				if (!msgs->TxMsg->set(data)) {
					Logger::Log(LOG_ERROR,"Tx queue buffer overflow");
				}
			}			
		}
	}
	catch (SocketException&) {
		Logger::Log(LOG_DEBUG,"Socket (%d) Disconnected...",msgid );
	}

	//Logger::Log(LOG_DEBUG,"SocketThread (%d) finished..." ,msgid);
	Logger::Log(LOG_NOTICE,"Connection to client (%d) closed",msgid);
	msgs->RxbMsg->unsubscribe(msgid);
	pthread_exit(NULL);
}

void *Pi433MHzd::pthServerThread(void *vargs) {
	ThreadArgs *args = (ThreadArgs *)vargs;
	Messages *msgs = (Messages *)args->msgs;
	int Port = args->intarg;
	int MaxClients = args->intarg2;
	delete args;
	Logger::Log(LOG_DEBUG,"ServerThread running....");

	try {
    	// Create the socket
    	ServerSocket server(Port, MaxClients);
    	server.set_non_blocking(true);

 		while (!msgs->XqMsg->qget()) {
 			Socket new_sock;
	  		bool found = server.accept(new_sock);

	  		if (found) {
	  			pthread_t pthClient;
	  			ThreadArgs *args = new ThreadArgs();

				if (args != NULL) {
    				args->msgs = msgs;
    				args->intarg = new_sock.get_id();
    				//Logger::Log(LOG_DEBUG,"New socket id: %d",args->intarg);
    				new_sock.set_id(-1); // prevent closing socket on destructing class
    				pthread_create(&pthClient, NULL, &pthSocketThread, args);
				}

	  		} else {
	  			usleep(100000);
	  		}
		}
    }
	catch (SocketException& e) {
    	Logger::Log(LOG_ERROR,"Exception was caught: %s, exiting",e.description().c_str());
    }

	Logger::Log(LOG_DEBUG,"ServerThread finished....");

    pthread_exit(NULL);
}

void Pi433MHzd::RxCallback(unsigned long receivedCode, unsigned int period) {
    struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	unsigned long now = (unsigned long)(spec.tv_sec*1000) + (unsigned long)(spec.tv_nsec / 1.0e6);
	string data = Logger::format("%lu", receivedCode);

	if ((receivedCode == lastcode) && (now > lastcmdtime) && (now-lastcmdtime < ignoretime)) {
		Logger::Log(LOG_DEBUG,"Repetition of equal data ingored");
	} else if (period == 0) { // maybe check period later
		Logger::Log(LOG_ERROR,"Rx incorrect data");
	} else if (!smsg::set(data)) {
		Logger::Log(LOG_ERROR,"Rx queue buffer overflow");
	}
	lastcode = receivedCode;
	lastcmdtime = now;
}

void Pi433MHzd::EchoCallback(std::string receivedData) {
	if (!smsg::set(receivedData)) {
		Logger::Log(LOG_ERROR,"Rx queue buffer overflow");
	}
}

void Pi433MHzd::usage() {
	cout << "\n" << 
    	"Pi433MHzd V" << DAEMON_VERSION << "\n" <<
    	"Usage: sudo Pi433MHzd [OPTION] ...\n" <<
    	"   -g,         run in foreground (do not fork),   default disabled\n" <<
    	"   -h,         display this help and exit,\n" <<
    	"   -v, -V,     display pigpio version and exit\n" <<
    	"\n";
}

int Pi433MHzd::Entry() {
	int res = RES_OK;
	config = new Config();
	string ConfigFile = config->GetFileName("Pi433MHzd.conf");
	config->Load(ConfigFile);

	logger = new Logger();
	logger->Init(config->LogFile, config->LogLevel);

	//handle signals
	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        Logger::Log(LOG_ERROR,"can't catch SIGUSR1");
    else if (signal(SIGINT, sig_handler) == SIG_ERR)
        Logger::Log(LOG_ERROR,"can't catch SIGINT"); 

    ignoretime = config->RxIgnoreRepeats;

	msgs = new Messages();
	msgs->XqMsg = new qmsg();
	msgs->TxMsg = new msg(config->TxQueue);
	msgs->RxMsg = new smsg(config->RxQueue);
	msgs->RxbMsg = new bmsg(config->MaxClients);

	ThreadArgs *args = new ThreadArgs();
	args->msgs = msgs;
    args->intarg = config->Port;
    args->intarg2 = config->MaxClients;
	if (pthread_create(&pthServer, NULL, pthServerThread, args)) {
		Logger::Log(LOG_ERROR,"ERROR Creating server thread, exiting");
		res=RES_NOK;
	}

	// load piGPIO (only if not both off or echo)
	bool nogpio = (((config->TxDevice == X_ECHO) || (config->TxDevice == X_OFF)) && ((config->RxDevice == X_ECHO) || (config->RxDevice == X_OFF)));
	if ((res == RES_OK) && (!nogpio)) {
    	pi = pigpio_start(0, 0);
    	if (pi < 0) {
			Logger::Log(LOG_ERROR,"PiGPIO setup failed (is the daemon running ?)");
			res = pi;
		}
	}

	// Init devices
	if (res == RES_OK) res = InitRemoteControl();
	if (res == RES_OK) res = InitRxDevice();
	if (res == RES_OK) res = InitTxDevice();

	return (res);
}

int Pi433MHzd::Exit() {
	int res = RES_OK;
	msgs->XqMsg->qset();
	pthread_join(pthServer, NULL);
	usleep(100000);

	if (Control != NULL) {
		Control->disable();
	}

	pigpio_stop(pi);

	delete config;
	delete logger;

	return (res);
}

void Pi433MHzd::RunDaemon() {
	Logger::Log(LOG_NOTICE,"Pi433MHzd daemon started");
	do {
		// Read messages from socket, write to 433MHz device
		if (msgs->TxMsg->hasmessage()) {
			std::string data;
			msgs->TxMsg->get(data);
			int res = TxTransmit(data);
			if (res == RES_OK) {
				Logger::Log(LOG_DEBUG,"<--%s",data.c_str());
				//std::cout << "<--" << data << std::endl;
			} else if (res != RES_OFF) {
				Logger::Log(LOG_ERROR,"Error transmitting data\n");
			}
		}
		// Write messages to socket, read from 433MHz device callback
		if (msgs->RxMsg->hasmessage()) {
			std::string data;
			msgs->RxMsg->get(data);
			int res = RxReceive(data);
			if (res == RES_OK) {
				if (!msgs->RxbMsg->broadcast(data)) {
					Logger::Log(LOG_ERROR,"Error broadcasting message\n");	
				}
				Logger::Log(LOG_DEBUG,"-->%s",data.c_str());
				//std::cout << "-->" << data << std::endl;
			} else if (res != RES_OFF) {
				Logger::Log(LOG_ERROR,"Error receiving data\n");
			}
		}
		usleep(100000);
	}  while (!msgs->XqMsg->qget());
	Logger::Log(LOG_NOTICE,"Pi433MHzd daemon finished");
}

void Pi433MHzd::sig_handler(int signo) {
    if ((signo == SIGUSR1) || (signo == SIGINT)) {
    	qmsg::qset_();
    }    
}

int Pi433MHzd::InitRemoteControl() {
	int res = RES_OK;

	Control = new RemoteControl(pi, config->EnableGpio, config->TxRxGpio, (config->PllCharge > 0));
	if (Control != NULL) {
		Logger::Log(LOG_NOTICE,"Remote Control initialized");
	} else {
		Logger::Log(LOG_ERROR,"Error initializing Remote Control");
		res = RES_NOK;
	}

	if (Control->isEnabled() == false) {
		Logger::Log(LOG_ERROR,"Error enabling Remote Control");
	}

	return (res);
}


int Pi433MHzd::InitTxDevice() {
	int res = RES_OK;
	int Period = 0;

	switch (config->TxDevice) {
		case X_RAW:
			if (config->TxPeriod == 0) {
				Period = PER_DEF_RAW; 
			} else {
				Period = config->TxPeriod;
			}
			TxDevice = (void*) new RemoteTransmitter(pi, config->TxGpio, Period, config->TxRepeats);
			if (TxDevice != NULL) {
				Logger::Log(LOG_NOTICE,"Raw Transmitter initialized");
			} else {
				Logger::Log(LOG_ERROR,"Error initializing Raw Transmitter");
				res = RES_NOK;
			}
			break;
		case X_ACTION:
			if (config->TxPeriod == 0) {
				Period = PER_DEF_ACTION; 
			} else {
				Period = config->TxPeriod;
			}
			TxDevice = (void*) new ActionTransmitter(pi, config->TxGpio, Period, config->TxRepeats);
			if (TxDevice != NULL) {
				Logger::Log(LOG_NOTICE,"Action Transmitter initialized");
			} else {
				Logger::Log(LOG_ERROR,"Error initializing Action Transmitter");
				res = RES_NOK;
			}
			break;
		case X_BLOKKER:
			if (config->TxPeriod == 0) {
				Period = PER_DEF_BLOKKER; 
			} else {
				Period = config->TxPeriod;
			}
			TxDevice = (void*) new BlokkerTransmitter(pi, config->TxGpio, Period, config->TxRepeats);
			if (TxDevice != NULL) {
				Logger::Log(LOG_NOTICE,"Blokker Transmitter initialized");
			} else {
				Logger::Log(LOG_ERROR,"Error initializing Blokker Transmitter");
				res = RES_NOK;
			}
			break;
		case X_KAKU:
			if (config->TxPeriod == 0) {
				Period = PER_DEF_KAKU; 
			} else {
				Period = config->TxPeriod;
			}
			TxDevice = (void*) new KaKuTransmitter(pi, config->TxGpio, Period, config->TxRepeats);
			if (TxDevice != NULL) {
				Logger::Log(LOG_NOTICE,"Kaku Transmitter initialized");
			} else {
				Logger::Log(LOG_ERROR,"Error initializing Kaku Transmitter");
				res = RES_NOK;
			}
			break;
		case X_ELRO:
			if (config->TxPeriod == 0) {
				Period = PER_DEF_ELRO; 
			} else {
				Period = config->TxPeriod;
			}
			TxDevice = (void*) new ElroTransmitter(pi, config->TxGpio, Period, config->TxRepeats);
			if (TxDevice != NULL) {
				Logger::Log(LOG_NOTICE,"Elro Transmitter initialized");
			} else {
				Logger::Log(LOG_ERROR,"Error initializing Elro Transmitter");
				res = RES_NOK;
			}
			break;
		case X_ECHO:
		default: // X_OFF
			// nothing to be done
			break;
	}

	return (res);
}

int Pi433MHzd::InitRxDevice() {
	int res = RES_OK;

	switch (config->RxDevice) {
		case X_RAW:
			RxDevice = (void*) new RemoteReceiver(pi, config->RxGpio, config->RxRepeats, RxCallback);
			if (RxDevice != NULL) {
				Logger::Log(LOG_NOTICE,"Raw Receiver initialized");
			} else {
				Logger::Log(LOG_ERROR,"Error initializing Raw Receiver");
				res = RES_NOK;
			}
			break;
		case X_ACTION:
			RxDevice = (void*) new ActionReceiver(pi, config->RxGpio, config->RxRepeats, RxCallback);
			if (RxDevice != NULL) {
				Logger::Log(LOG_NOTICE,"Action Receiver initialized");
			} else {
				Logger::Log(LOG_ERROR,"Error initializing Action Receiver");
				res = RES_NOK;
			}
			break;
		case X_BLOKKER:
			RxDevice = (void*) new BlokkerReceiver(pi, config->RxGpio, config->RxRepeats, RxCallback);
			if (RxDevice != NULL) {
				Logger::Log(LOG_NOTICE,"Blokker Receiver initialized");
			} else {
				Logger::Log(LOG_ERROR,"Error initializing Blokker Receiver");
				res = RES_NOK;
			}
			break;
		case X_KAKU:
			RxDevice = (void*) new KaKuReceiver(pi, config->RxGpio, config->RxRepeats, RxCallback);
			if (RxDevice != NULL) {
				Logger::Log(LOG_NOTICE,"Kaku Receiver initialized");
			} else {
				Logger::Log(LOG_ERROR,"Error initializing Kaku Receiver");
				res = RES_NOK;
			}
			break;
		case X_ELRO:
			RxDevice = (void*) new ElroReceiver(pi, config->RxGpio, config->RxRepeats, RxCallback);
			if (RxDevice != NULL) {
				Logger::Log(LOG_NOTICE,"Elro Receiver initialized");
			} else {
				Logger::Log(LOG_ERROR,"Error initializing Elro Receiver");
				res = RES_NOK;
			}
			break;
		case X_ECHO:
		default: // X_OFF
			// nothing to be done
			break;
	}

	return (res);
}

int Pi433MHzd::TxTransmit(string data) {
	int res = RES_OK;
	int codes[MAXFMTSIZE];

	if (!config->RxEcho) RxEnable(false);
	TxRx(true);

	switch (config->TxDevice) {
		case X_ECHO:
			EchoCallback(data);
			break;
		case X_RAW:
			if (res == RES_OK) {
				RemoteTransmitter* TxDev = (RemoteTransmitter*) TxDevice;
				format::decode(data, codes, 1);
				TxDev->sendCode((unsigned long)codes[0]);
			}
			break;
		case X_ACTION:
			if (res == RES_OK) {
				ActionTransmitter* TxAction = (ActionTransmitter*) TxDevice;
				format::decode(data, codes, 3);
				TxAction->sendSignalFmt(codes, 3);
			}
			break;
		case X_BLOKKER:
			if (res == RES_OK) {
				BlokkerTransmitter* TxBlokker = (BlokkerTransmitter*) TxDevice;
				format::decode(data, codes, 2);
				TxBlokker->sendSignalFmt(codes, 2);
			}
			break;
		case X_KAKU:
			if (res == RES_OK) {
				KaKuTransmitter* TxKaKu = (KaKuTransmitter*) TxDevice;
				format::decode(data, codes, 3);
				TxKaKu->sendSignalFmt(codes, 3);
			}
			break;
		case X_ELRO:
			if (res == RES_OK) {
				ElroTransmitter* TxElro = (ElroTransmitter*) TxDevice;
				format::decode(data, codes, 3);
				TxElro->sendSignalFmt(codes, 3);
			}
			break;
		default: // X_OFF
			// nothing to be done
			res = RES_OFF;
			break;
	}

	if (!config->RxEcho) RxEnable(true);
	TxRx(false);

	return (res);
}

int Pi433MHzd::RxReceive(string &data) {
	int res = RES_OK;
	unsigned long udata = strtoul(data.c_str(),NULL,10);

	switch (config->RxDevice) {
			break;
		case X_RAW:
			if (res == RES_OK) {
				int codes[1];
				codes[0] = (int)udata;
				format::encode(data, codes, 1);
			}
			break;
		case X_ACTION:
			if (res == RES_OK) {
				ActionReceiver* RxAction = (ActionReceiver*) RxDevice;
				int codes[3];
				if (RxAction->signalDecodeFmt(udata, codes, 3)) {
					format::encode(data, codes, 3);	
				} else {
					res = RES_NOK;
				}
			}
			break;
		case X_BLOKKER:
			if (res == RES_OK) {
				BlokkerReceiver* RxBlokker = (BlokkerReceiver*) RxDevice;
				int codes[2];
				if (RxBlokker->signalDecodeFmt(udata, codes, 2)) {
					format::encode(data, codes, 2);	
				} else {
					res = RES_NOK;
				}
			}
			break;
		case X_KAKU:
			if (res == RES_OK) {
				KaKuReceiver* RxKaKu = (KaKuReceiver*) RxDevice;
				int codes[3];
				if (RxKaKu->signalDecodeFmt(udata, codes, 3)) {
					format::encode(data, codes, 3);	
				} else {
					res = RES_NOK;
				}
			}
			break;
		case X_ELRO:
			if (res == RES_OK) {
				ElroReceiver* RxElro = (ElroReceiver*) RxDevice;
				int codes[3];
				if (RxElro->signalDecodeFmt(udata, codes, 3)) {
					format::encode(data, codes, 3);	
				} else {
					res = RES_NOK;
				}
			}
			break;
		case X_ECHO: // just reply data, no further action
			break;
		default: // X_OFF
			// nothing to be done
			res = RES_OFF;
			break;
	}

	return (res);
}

int Pi433MHzd::RxEnable(bool ena) {
	int res = RES_OK;
	RemoteReceiver* RxDev = (RemoteReceiver*) RxDevice;
	if (ena) {
		RxDev->enable();
	} else {
		RxDev->disable();
	}
	return (res);
}

void Pi433MHzd::TxRx(bool Tx) {
	if (Control != NULL) {
		if (Tx) {
			Control->Tx();
		} else {
			Control->Rx();
		}
	}
}

int main(int argc, char **argv) {
	int res = RES_OK, res2 = RES_OK;
	int opt = 0;
	pid_t pid;
	bool foreground = false;
	Pi433MHzd MainClass;

	//Read arguments
	while ((opt = getopt(argc, argv, "ghHvV")) != -1) {
    	switch (opt) {
        	case 'g':
            	foreground = true;
            	break;
         	case 'v':
         	case 'V':
            	cout << DAEMON_VERSION << "\n";
            	return (RES_NOK);
            	break;

        	default: /* '?' */
           		MainClass.usage();
           		return (RES_NOK);
    	}
	}

	if (!foreground) {
    	/* Fork off the parent process */
		pid = fork();

		if (pid < 0) {
			exit(RES_NOK);
		}
		/* If we got a good PID, then we can exit the parent process. */
    	if (pid > 0) {
    		exit(RES_OK);
    	}

    	if (pid == 0) { //fork process
    		cout << "Pi433MHzd; running as fork (as background process)" << endl;
    	}

		/* Change the file mode mask */
		umask(0);

    	/* Create a new SID for the child process */
		if (setsid() < 0) {
			cerr << "setsid failed\n";
			_exit(RES_NOK);
		}
	} else {
   		cout << "Pi433MHzd; running from command line" << endl;
		cout << "Press ^c to kill" << endl;
	}
	
	res = MainClass.Entry();
	if (res >= RES_OK) {
		MainClass.RunDaemon();		
	}
	res2 = MainClass.Exit();

	if (res == RES_OK) res = res2;

	return (res);
}