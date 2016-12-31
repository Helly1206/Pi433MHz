#include <pigpiod_if2.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>
#include "DeviceReceiver.h"

using namespace std;
void callbackRF(unsigned long receivedCode, unsigned int period) {
	printf("Rawcode: %ld\n",receivedCode);
	printf("Period: %d\n",period);
}

int main() 
{
	int pi;
	int pin_out = 2;

	printf("Press ^C to quit\n");

	// load piGPIO
    pi = pigpio_start(0, 0);
    if (pi < 0)
	{
		printf("PiGPIO setup failed.\n");
		exit(1);
	}

	RemoteReceiver remoteReceiver(pi, pin_out, 3, callbackRF);

	while(1) {
		sleep(1);
	}

	pigpio_stop(pi);
}