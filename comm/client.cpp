#include <iostream>
#include <string>
#include <pthread.h>
#include "Pi433MHzif.h"
#include "format.h"

#define MAXSIZE 3

// Very ugly solution, but just to test ....
std::string inputstring = "";

static void * pthMyThread(void *) {
	while (1) {
		std::cout << "-->";
  		std::cin >> inputstring;	
	}
}

int main() {
	pthread_t pthMine;
    Pi433MHzif *client = new Pi433MHzif();
    bool cont=true;

    std::string reply;
    std::string inputstringcpy = "";

    std::cout << "TestClient for Pi433MHzd\n";
    std::cout << "Enter your input depending on switch type\n";
    std::cout << "raw: 'xxxxxx' (= integer number)\n";
    std::cout << "action: 'syscode!devcode!on!' (on= 0 or 1)\n";
    std::cout << "blokker: 'device!on!' (on= 0 or 1)\n";
    std::cout << "kaku: 'address!device!on!' (on= 0 or 1)\n";
    std::cout << "elro: 'syscode!devcode!on!' (on= 0 or 1)\n";

    if (pthread_create(&pthMine, NULL, pthMyThread, NULL)) {
		std::cout << "ERROR Creating thread\n";
		return(1);
	}

    while (cont) {
    	if ((!inputstring.empty()) && (inputstring != inputstringcpy)) {
    		inputstringcpy = inputstring;
    		std::string data;
    		if (*inputstring.rbegin() == '!') data = inputstring;
    		else data = inputstring + "!";
    		if (format::teststring(data,MAXSIZE)) {
    			int array[MAXSIZE];
    			int size = format::decode(data, array, MAXSIZE);
    			if (size > 0) {
					int res = client->WriteMessage(array, size);
            		if (res < 0) cont = false;
        			//client_socket << inputstring;
            	}
    		} else {
    			std::cout << "Invalid input string\n";
    		}          
		}

		if (cont) {
        	std::string data;
        	int array[MAXSIZE];
			int res = client->ReadMessage(array, MAXSIZE);
			if (res < 0) cont = false;
    		else if (res > 0) {
    			res = format::encode(data, array, res);
				std::cout << "<--" << data << std::endl;
			}			
		}
    }

    delete client;

  	return 0;
}