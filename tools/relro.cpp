#include <pigpiod_if2.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>
#include "DeviceReceiver.h"

using namespace std;
ElroReceiver *pelroReceiver = NULL;

void callbackRF(unsigned long receivedCode, unsigned int period) {
	uint8_t sysCde;
	uint8_t devCde;
	bool on;
	char son[4];

	printf("Rawcode: %ld\n",receivedCode);
	printf("Period: %d\n",period);
	pelroReceiver->signalDecode(receivedCode, &sysCde, &devCde, &on);
	if (on) {
		sprintf(son,"on");
	} else {
		sprintf(son,"off");
	}

	printf("Decoded (system device state): %d %d %s\n",sysCde, devCde, son);
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

	ElroReceiver elroReceiver(pi, pin_out, 3, callbackRF);
	pelroReceiver = &elroReceiver;
	pelroReceiver->enable();

	while(1) {
		sleep(1);
	}

	pigpio_stop(pi);
}