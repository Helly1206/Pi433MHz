/*
 * Pi433MHzif.h
 * Headerfile for client library interface for Pi433MHz
 *
 * Copyright 26-12-2016 Helly
 */

#ifndef PI433MHZIF_H
#define PI433MHZIF_H

#include <string>
#include <iostream>
#include "socket.h"

class Pi433MHzif {
	public:
		// Constructor
		Pi433MHzif();
		virtual ~Pi433MHzif();
		int ReadMessage(int *array, int maxsize);
		int WriteMessage(int *array, int size);
	private:
		int ReadData(std::string &data);
		int WriteData(std::string data);
		ClientSocket *client_socket;
		bool confok;
};

extern "C" Pi433MHzif* _Pi433MHzif(void);
extern "C" void _Pi433MHzif_delete(Pi433MHzif* t);
extern "C" int _Pi433MHzif_ReadMessage(Pi433MHzif* t, int *array, int maxsize);
extern "C" int _Pi433MHzif_WriteMessage(Pi433MHzif* t, int *array, int size);

#endif