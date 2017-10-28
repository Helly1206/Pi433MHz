#!/bin/bash
INSTALL="/usr/bin/install -c"
INSTALL_DATA="$INSTALL -m 644"
INSTALL_PROGRAM="$INSTALL"
ETCDIR="/etc"
USRDIR="/usr"
INCDIR="$USRDIR/include"
LIBDIR="$USRDIR/lib"
BINDIR="$USRDIR/bin"
SYSTEMDDIR="./systemd"
SERVICEDIR="$ETCDIR/systemd/system"
SERVICESCRIPT="Pi433MHzd.service"
SERVICE2SCRIPT="Pi433MHzd2.service"
SERVICEPIGPIODSCRIPT="pigpiod.service"
DAEMON="Pi433MHzd"
DAEMONCONF="Pi433MHzd.conf"
PYDIR="./pycomm"
PYIF="Pi433MHzif"
LIB="libPi433MHz.so"
LIBIF="libPi433MHzif.so"

if [ "$EUID" -ne 0 ]
then 
	echo "Please execute as root ('sudo install.sh' or 'sudo make install')"
	exit
fi

if [ "$1" == "-u" ] || [ "$1" == "-U" ]
then
	echo "Pi433MHz uninstall script"

	echo "Uninstalling daemon Pi433MHzd"
	systemctl stop "$SERVICESCRIPT"
	systemctl disable "$SERVICESCRIPT"
	if [ -e "$SERVICEDIR/$SERVICESCRIPT" ]; then rm -f "$SERVICEDIR/$SERVICESCRIPT"; fi
	if [ -e "$BINDIR/$DAEMON" ]; then rm -f "$BINDIR/$DAEMON"; fi
	
	echo "Uninstalling python interface"
	pip uninstall "$PYIF"
	pip3 uninstall "$PYIF"
	
	echo "Uninstalling C/ C++ library"
	if [ -e "$LIBDIR/$LIB" ]; then rm -f "$LIBDIR/$LIB"; fi
	if [ -e "$LIBDIR/$LIBIF" ]; then rm -f "$LIBDIR/$LIBIF"; fi
		for FILE in ./inc/*
	do
		BFILE=$(basename $FILE)
		if [ -e "$INCDIR/$BFILE" ]; then rm -f "$INCDIR/$BFILE"; fi
	done

else
	echo "Pi433MHz install script"

	echo "Stop running services"
	systemctl stop $SERVICESCRIPT
    systemctl disable $SERVICESCRIPT
    sleep 2
    systemctl stop $SERVICEPIGPIODSCRIPT
    systemctl disable $SERVICEPIGPIODSCRIPT

	echo "Installing C/ C++ library"
	if [ ! -e "./$LIB" ]
	then 
		echo "Error: ./$LIB not found"
		echo "Execute 'make all' first"
		exit
	else
		$INSTALL_DATA "./$LIB" $LIBDIR
	fi
	if [ ! -e "./$LIBIF" ]
	then 
		echo "Error: ./$LIBIF not found"
		echo "Execute 'make all' first"
		exit
	else
		$INSTALL_DATA "./$LIBIF" $LIBDIR
	fi
	for FILE in ./inc/*
	do
    	$INSTALL_DATA "$FILE" $INCDIR
	done

	echo "Installing python interface"
	sudo pip install "$PYDIR/."
	sudo pip3 install "$PYDIR/."

	read -p "Do you want to install an automatic startup service for pigpiod (Y/n)? " -n 1 -r
	echo    # (optional) move to a new line
	if [[ $REPLY =~ ^[Nn]$ ]]
	then
    	echo "Skipping install automatic startup service for pigpiod"
    	read -p "Is an automatic startup service for pigpiod already installed (Y/n)? " -n 1 -r
		echo    # (optional) move to a new line
		if [[ $REPLY =~ ^[Nn]$ ]]
		then
    		PIGPIODSERVICE=0
    	else
    		PIGPIODSERVICE=1
    		systemctl enable $SERVICEPIGPIODSCRIPT
    		systemctl start $SERVICEPIGPIODSCRIPT
    	fi
    else
    	echo "Install automatic startup service for pigpiod"
    	PIGPIODSERVICE=1
    	$INSTALL_DATA "$SYSTEMDDIR/$SERVICEPIGPIODSCRIPT" $SERVICEDIR
    	systemctl enable $SERVICEPIGPIODSCRIPT
    	systemctl start $SERVICEPIGPIODSCRIPT
	fi

	echo "Installing daemon Pi433MHzd"
	if [ ! -e "./$DAEMON" ]
	then 
		echo "Error: ./$DAEMON not found"
		echo "Execute 'make all' first"
		exit
	else
		$INSTALL_PROGRAM "./$DAEMON" $BINDIR
		if [ -e "$SERVICEDIR/$SERVICESCRIPT" ]
		then
			echo "File $ETCDIR/$DAEMONCONF already exists. Leaving untouched."
		else 
			$INSTALL_DATA "./$DAEMONCONF" $ETCDIR
		fi	
	fi

	read -p "Do you want to install an automatic startup service for Pi433MHzd (Y/n)? " -n 1 -r
	echo    # (optional) move to a new line
	if [[ $REPLY =~ ^[Nn]$ ]]
	then
    	echo "Skipping install automatic startup service for Pi433MHzd"
    else
    	echo "Install automatic startup service for Pi433MHzd"
    	if [ $PIGPIODSERVICE == 1 ]
    	then
    		echo "Installing wait for pigpiod running version"
    		$INSTALL_DATA "$SYSTEMDDIR/$SERVICESCRIPT" $SERVICEDIR
		else
			echo "Installing don't wait for pigpiod running version"
			$INSTALL_DATA "$SYSTEMDDIR/$SERVICE2SCRIPT" "$SERVICEDIR/$SERVICESCRIPT"
		fi
		systemctl enable $SERVICESCRIPT
		systemctl start $SERVICESCRIPT
	fi
fi





