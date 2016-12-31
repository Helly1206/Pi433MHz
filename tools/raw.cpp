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
    unsigned long data = 12345678;    
    
    if( argc != 2 ) { // not enough arguments
    	std::cout << "usage: " << argv[0] << " data" << std::endl;
    	std::cout << "example: " << argv[0] << " 12345678" << std::endl;
	    return 0;
    } else {
	    data = atol(argv[1]);
    }
    
    // load piGPIO
    pi = pigpio_start(0, 0);
    if (pi < 0)
	{
		printf("PiGPIO setup failed.\n");
		exit(1);
	}

	RemoteTransmitter remoteTransmitter(pi, pin_out, 320, 4);
	
	remoteTransmitter.sendCode(data);

	pigpio_stop(pi);
}