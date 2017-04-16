Pi433MHz: v0.90

Software is made under GNU PUBLIC LICENSE (see LICENSE.txt)

Please send Comments and Bugreports to hellyrulez@home.nl

This program contains the interface for 433 MHz devices, to be included in your own home automation software.
This program contains a daemon that automatically transmits and receives data to and from a 433 MHz device.
This program can be used for transmitters and receivers normally used for arduino:
http://arduinobasics.blogspot.nl/2014/06/433-mhz-rf-module-with-arduino-tutorial.html
Take care this these devices require 5V power, but the IO pins are not 5V compatible. Connection diagrma is to follow.

This software is made for Rasberry Pi running raspbian (tested on a RPI 3B running Jessie) and is using pigpio for IO.
pigpio is installed by default on raspian Jessie, so no further action on this interface is required, except for running the daemon (which is explained later).

Pi433MHz v0.90 is not tested on real hardware yet, but the software runs stable and is working on IO level.

Pi433MHz v0.90 added functionality for switching Tx and Rx modes (on transceivers) and enable/ disable hardware. Furthermore PLL charge/ discharge functionality is added for e.g. Aurel (RTX-MID-xV) devices.

Installing Pi433MHz:
====================
1) install the source code, download or clone from  from: https://github.com/helly1206/Pi433MHz

2) place source code in a temporary folder (e.g. ~/projects/Pi433MHz)

3) cd to this folder

4) build the source code:
	make all

5) install the software:
	sudo make install
   During installing 2 or 3 questions are asked:
   1) Do you want to install an automatic startup service for pigpiod (Y/n)?
   		Default = Y
   		If you didn't make an automated script for starting pigpiod during startup (or don't know), answer Y to this question.
   		If you did make an automated script for starting pigpiod during startup or do not want to install an automatic startup script, answer N to this question.
   		If you answered N, a second question is asked:
   		2) Is an automatic startup service for pigpiod already installed (Y/n)?
   			Default = Y
   			If you already made an automated script for starting pigpiod during startup, answer Y to this question.
   			If didn't make an automated script for starting pigpiod during startup and don't want Pi433MHz to be dependent on it, answer N to this question. Take care that starting Pi433MHzd might fail because no running pigpiod can be found. 
   			If you do not want to install a daemon, the answer is not relevant.
   3) Do you want to install an automatic startup service for Pi433MHzd (Y/n)?
   		Default = Y
   		If you want to automatically start Pi433MHzd during startup (or don't know), answer Y to this question.
   		If you do not want to install an automatic startup script, answer N to this question.

That's all

Uninstalling can be done by:
	sudo make unistall

Using Pi433MHz:
===============

Using C/C++ library:
====================
A library libPi433MHz.so is installed in the default library folder.
You can use this library in your own code.
See the header files (in inc/) for more information.

In C++ you can load the class like:
RemoteReceiver *myReceiver = new RemoteReceiver(pi, pin, minRepeats, callback);
and unload like:
delete myReceiver;

For C, a C interface to the C++ code is made.
You can load the class like:
RemoteReceiver* myReceiver=_RemoteReceiver(pi, pin, minRepeats, callback); 
and unload like:
_RemoteReceiver_delete(myReceiver);

The library can also be included in other languages (like python using ctypes). These interfaces are not foreseen.

Using the daemon:
=================
When the daemon is installed and started as a service, it automatically runs after booting your Raspberry Pi.
When not started as a service, you can manually start it with:
sudo Pi433MHzd
When you don't want to start it as daemon (forked), enter:
sudo Pi433MHzd -g
Take care that the pigpiod daemon is running before starting Pi433MHzd

The daemon logs to /var/log/Pi433MHzd.log by default.
This log will tell you if the daemon started correctly.

The daemon settings are stored in /etc/Pi433MHzd.conf
Edit this file (e.g. sudo nano /etc/Pi433MHzd.conf) for changing settings. This file contains comments on the settings, so you can figure out from this file yourself.
After changing settings, you need to restart the daemon.
When using the automatic startup service, enter:
sudo systemctl restart Pi433MHzd.service
When started manually, kill it first and start again after:
kill -SIGINT $(pgrep Pi433MHzd)

Communicating to the daemon from the C/C++ library:
===================================================
A library libPi433MHzif.so is installed in the default library folder.
You can use this library in your own code.
See the header file (in inc/Pi433MHzdif.h) for more information.
For C, a C interface to the C++ code is made. See explanation in the C/C++ library chapter for how to use.

There are 2 methods:
1) int ReadMessage(int *array, int maxsize);
	You need to allocate and array with a maximum size 3 before. Call like:
	read=ReadMessage(array, 3);
	The result is stored in an array. The contents depends on the device type: e.g. for raw:
	array[0]=<code>
	and for elro:
	array[0]=<system code>
	array[1]=<device code>
	array[2]=<on/off, 0 or 1>
	the return value contains the size of the array, e.g. 1 for raw and 3 for elro.

2) int WriteMessage(int *array, int size);
	You need to allocate and array with a maximum size 3 before.
	Fill the array with the required code, like is explained in the read session.
	Call like, for raw:
	written=WriteMessage(array, 1);
	And for elro:
	written=WriteMessage(array, 3);
	the return value contains the number of code actually written, e.g. 1 for raw and 3 for elro.

The test file comm/client.cpp contains a simple example on using this interface

The library can also be included in other languages (like python using ctypes). The python interface is foreseen, others not.

Communicating to the daemon from the python library:
====================================================

The C/C++ library is included in a python package, so it can be used from python.
This python package is by default installed for python 2 and python 3
To use it, inport this library in your python code:
import Pi433MHzif

The interface is similar to the C/ C++ interface
Load the class with:
client = Pi433MHzif.Pi433MHzif()
and unload with:
del client

There are 2 methods:
1) ReadMessage(self, maxsize)
	Usage is similar to the C interface:
	read, array = client.ReadMessage(3)
	read contains the size of the array, e.g. 1 for raw and 3 for elro.
	array contains the data as list, similar as explained above

2) WriteMessage(self, array, size)
	Fill the array as list with the required code.
	Call like, for raw:
	written = client.WriteMessage(array, 1);
	And for elro:
	written = client.WriteMessage(array, 3);
	the return value contains the number of code actually written, e.g. 1 for raw and 3 for elro.

The test file pycomm/client.py contains a simple example on using this interface

That's all for now ...