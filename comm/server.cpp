#include <pthread.h>
#include <signal.h>
#include <string>
#include <iostream>
#include "socket.h"
#include "msg.h"
#include "format.h"

#define qsize 5 // Rxqueue size
#define ssize 5 //number of max connections

bool running = true;

class messages {
	public:
		qmsg* tqmsg;
		msg* tmsg;
		bmsg* tbmsg;
};

class thread_args {
	public:
    	messages *msgs;
    	int id;
};

static void * pthSocketThread(void *vargs) {
	thread_args *args = (thread_args *)vargs;
	messages *msgs = (messages *)args->msgs;
	Socket socket;
	socket.set_id(args->id);
	int msgid = -1;
	bool disconnected = false;

	delete args;
	if (!msgs->tbmsg->subscribe(msgid)) {
		std::cout << "Socket not allowed... " << msgid << std::endl;	
		pthread_exit(NULL);
	}

	std::cout << "SocketThread running... " << msgid << std::endl;

	try {
		while ((!msgs->tqmsg->qget()) && (!disconnected)) {
			// write if hasmessage
			if (msgs->tbmsg->hasmessage(msgid)) {
				std::string data;
				if (msgs->tbmsg->read(msgid,data)) {
					socket << data;
				}
			}
			//socket << ":" << data;
			// poll and try read
			bool pollres = socket.poll(100);
			if (pollres) {
				std::string data;
				socket >> data;
				msgs->tmsg->set(data);
				//std::cout << "&" << data << std::endl;
			}			
		}
	}
	catch (SocketException&) {
		std::cout << "Socket Disconnected... " << msgid << std::endl;
	}

	std::cout << "SocketThread finished..." << msgid << std::endl;
	msgs->tbmsg->unsubscribe(msgid);
	pthread_exit(NULL);
}

static void * pthServerThread(void *vmsgs) {
	messages *msgs = (messages *)vmsgs;
	std::cout << "ServerThread running....\n";

	try {
    	// Create the socket
    	ServerSocket server(51721);
    	server.set_non_blocking(true);

 		while (!msgs->tqmsg->qget()) {
 			Socket new_sock;
	  		bool found = server.accept(new_sock);

	  		if (found) {
	  			std::cout << "*" << msgs->tbmsg->size() << std::endl;

	  			pthread_t pthClient;
	  			thread_args *args = new thread_args(); //malloc(sizeof *args);

				if (args != NULL) {
    				args->msgs = msgs;
    				args->id = new_sock.get_id();
    				std::cout << "id: " << args->id << std::endl;
    				new_sock.set_id(-1); // prevent closing socket on destructing class
    				pthread_create(&pthClient, NULL, &pthSocketThread, args);
				}

	  		} else {
	  			usleep(100000);
	  		}
		}
    }
	catch (SocketException& e) {
    	std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }

	std::cout << "ServerThread finished....\n";

    pthread_exit(NULL);
}

void sig_handler(int signo) {
    if ((signo == SIGUSR1) || (signo == SIGINT)) {
        running = false;
    }    
}

int main() {
	pthread_t pthServer;
	messages *msgs = new messages();
	msgs->tqmsg = new qmsg();
	msgs->tmsg = new msg(qsize);
	msgs->tbmsg = new bmsg(ssize);

	std::cout << "Main....\n";


	// TEST
	/*std::string a="123!345!765!34567!452!";
	int arr[7];
	int res = format::decode(a,arr,7);
	std::cout << a<< "\n";
	std::cout << res << "\n";
	for (int i=0;i<res;i++) {
		std::cout << arr[i] << ",";
	}
	std::cout << "\n";

	a = "";
	res = format::encode(a,arr,res);
	std::cout << a<< "\n";
	std::cout << res << "\n";*/
	// END TEST

	//handle signals
	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        Logger::Log(LOG_ERROR,"can't catch SIGUSR1");
    else if (signal(SIGINT, sig_handler) == SIG_ERR)
        Logger::Log(LOG_ERROR,"can't catch SIGINT"); 

	if (pthread_create(&pthServer, NULL, pthServerThread, msgs)) {
		std::cout << "ERROR Creating thread\n";
		return(1);
	}

	int i=0;
	int j=0;
	while (running) {
		if (msgs->tmsg->hasmessage()) {
			std::string data;
			msgs->tmsg->get(data);
			std::cout << "<--" << data << std::endl;
		}
		usleep(100000);
		i++;
		if (i>100) {
			std::string data;
			data = Logger::format("%d!%d!%d!",j,j+1,j%2);
			if (j<100) j++; else j=0;
			msgs->tbmsg->broadcast(data);
			std::cout << "-->" << data << std::endl;	
			i=0;
		}
	}

	msgs->tqmsg->qset();
	pthread_join(pthServer, NULL);
	usleep(100000);
	std::cout << "Ready....\n";
	return 0;
}