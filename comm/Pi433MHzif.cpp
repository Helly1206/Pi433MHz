#include "Pi433MHzif.h"
#include "format.h"
#include "config.h"

using namespace std;

// Constructor
Pi433MHzif::Pi433MHzif() {
	try {
		Config *config = new Config();
		string ConfigFile = config->GetFileName("Pi433MHzd.conf");
		config->Load(ConfigFile);
    	client_socket = new ClientSocket("localhost", config->Port);
    	delete config;
    	confok = true;
    }
    catch (SocketException& e) {cerr << "Pi433MHzif: Exception while Connecting\n";}	
}

Pi433MHzif::~Pi433MHzif() {
    delete client_socket;
    confok = false;
}

int Pi433MHzif::ReadMessage(int *array, int maxsize) {
	int res = -1;
	string data;
	res = ReadData(data);
	if (res > 0) {
		res = format::decode(data, array, maxsize);
	}

	return (res);
}

int Pi433MHzif::WriteMessage(int *array, int size) {
	int res = -1;
	string data;
	int res2 = format::encode(data, array, size);
	if (res2 > 0) {
    	res = WriteData(data);
    }
    if (res > 0) {
    	res = res2;
    }

    return (res);
}
        
int Pi433MHzif::ReadData(std::string &data) {
	if (!confok) return -1;
	int res = -1;
	try {
    	bool pollres = client_socket->poll(100);
    	if (pollres) {
        	(*client_socket) >> data;
        	res = 1;
    	} else
    		res = 0;
    }
    catch (SocketException& e) {cerr << "Pi433MHzif: Exception while Reading\n";}
    return (res);
}

int Pi433MHzif::WriteData(std::string data) {
	if (!confok) return -1;
	int res = -1;
	try {
    	(*client_socket) << data;
    	res = 1;
    }
    catch (SocketException& e) {cerr << "Pi433MHzif: Exception while Writing\n";}
    return (res);
}

// access functions
extern "C" Pi433MHzif* _Pi433MHzif(void) {
	return new Pi433MHzif();
}

extern "C" void _Pi433MHzif_delete(Pi433MHzif* t) {
	delete t;
}

extern "C" int _Pi433MHzif_ReadMessage(Pi433MHzif* t, int *array, int maxsize) {
	return t->ReadMessage(array, maxsize);
}

extern "C" int _Pi433MHzif_WriteMessage(Pi433MHzif* t, int *array, int size) {
	return t->WriteMessage(array, size);
}
