#include <pigpiod_if2.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream>
#include "DeviceTransmitter.h"

using namespace std;

int main(int argc, char **argv) 
{
	int pi;
	int pin_out = 2;
    int channel = 0;
    int socket = 4;
    bool state = false;
    
    
    if( argc != 4 ) { // not enough arguments
    	std::cout << "usage: " << argv[0] << " system device state" << std::endl;
    	std::cout << "example: " << argv[0] << " 5 4 on" << std::endl;
	    return 0;
    } else {
	    channel = atol(argv[1]);
	    socket = atol(argv[2]); //*argv[2];
	    string statestr = argv[3];
	    
	    if( statestr.compare("on") == 0 ) {
		    state = true;
	    } else {
		    state = false;
	    }
    }
    
    // load piGPIO
    pi = pigpio_start(0, 0);
    if (pi < 0)
	{
		printf("PiGPIO setup failed.\n");
		exit(1);
	}

	ElroTransmitter elroTransmitter(pi, pin_out);
	
	elroTransmitter.sendExtSignal((unsigned short)channel, (unsigned short)socket, state);

	pigpio_stop(pi);
}