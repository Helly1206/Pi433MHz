/*
 * msg.cpp
 * Sourcefile for message queue for ..
 *
 * Copyright 26-12-2016 Helly
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <string>
#include <stdint.h>
#include "msg.h"
#include "logger.h"

using namespace std;

/******** QMSG CLASS **********/

bool qmsg::q = false;
bool qmsg::IsInit = false;
pthread_mutex_t qmsg::mtx;

qmsg::qmsg() {
	if (pthread_mutex_init(&mtx, NULL) != 0) {
		Logger::Log(LOG_ERROR,"Mutex init failed\n");
		throw("Mutex Error");
	}
	qclr();
	IsInit = true;
}

qmsg::~qmsg() {
	pthread_mutex_destroy(&mtx);
	IsInit = false;
}

void qmsg::qset() {
	qset_();
}

void qmsg::qclr() {
	qclr_();
}

bool qmsg::qget() {
	return (qget_());
}

void qmsg::qset_() {
	if (!IsInit) return;
	pthread_mutex_lock(&mtx);
	q = true;
	pthread_mutex_unlock(&mtx);
}

void qmsg::qclr_() {
	if (!IsInit) return;
	pthread_mutex_lock(&mtx);
	q = false;
	pthread_mutex_unlock(&mtx);
}

bool qmsg::qget_() {
	if (!IsInit) return (false);
	bool qret = false;
	pthread_mutex_lock(&mtx);
	qret = q;
	pthread_mutex_unlock(&mtx);
	return (qret);
}

/******** MSG CLASS **********/

msg::msg(unsigned int qsize) {
	if (pthread_mutex_init(&mtx, NULL) != 0) {
		Logger::Log(LOG_ERROR,"Mutex init failed\n");
		throw("Mutex Error");
	}
	q_size = qsize;
}

msg::~msg() {
	pthread_mutex_destroy(&mtx);
}

bool msg::read(string& msg) {
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	if (msgs.size()>0) {
		msg = msgs.front();
		rbool = true;
	}
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

bool msg::get(string& msg) {
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	if (msgs.size()>0) {
		msg = msgs.front();
		msgs.pop();
		rbool = true;
	}
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

bool msg::set(string msg) {
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	if (msgs.size() < q_size) {
		msgs.push(msg);
		rbool = true;
	}
	
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

bool msg::hasmessage() {
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	rbool = (msgs.size()>0);
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

/******** SMSG CLASS **********/

pthread_mutex_t smsg::mtx;
unsigned int smsg::q_size = 0;
queue <string> smsg::msgs;
bool smsg::IsInit = false;	

smsg::smsg(unsigned int qsize) {
	if (pthread_mutex_init(&mtx, NULL) != 0) {
		Logger::Log(LOG_ERROR,"Mutex init failed\n");
		throw("Mutex Error");
	}
	q_size = qsize;
	IsInit = true;
}

smsg::~smsg() {
	pthread_mutex_destroy(&mtx);
	IsInit = false;
}

bool smsg::read(string& msg) {
	if (!IsInit) return (false);
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	if (msgs.size()>0) {
		msg = msgs.front();
		rbool = true;
	}
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

bool smsg::get(string& msg) {
	if (!IsInit) return (false);
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	if (msgs.size()>0) {
		msg = msgs.front();
		msgs.pop();
		rbool = true;
	}
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

bool smsg::set(string msg) {
	if (!IsInit) return (false);
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	if (msgs.size() < q_size) {
		msgs.push(msg);
		rbool = true;
	}
	
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

bool smsg::hasmessage() {
	if (!IsInit) return (false);
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	rbool = (msgs.size()>0);
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

/******** BMSG CLASS **********/

// broadcast messages
// queue only contains 1 message (if requered more, then add a msg in front, 
//     but this may be dangerous because of nesting messages)
// if new message is written, the last message if popped, wheter it is read by all subscribers or not
// read messages until read by all subscribers, then pop()

bmsg::bmsg(unsigned int ssize) {
	if (pthread_mutex_init(&mtx, NULL) != 0) {
		Logger::Log(LOG_ERROR,"Mutex init failed\n");
		throw("Mutex Error");
	}
	s_size=ssize;
	cur_id=0;
}

bmsg::~bmsg() {
	pthread_mutex_destroy(&mtx);
}

bool bmsg::subscribe(int& id) {
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	if (sids.size() < s_size) {
		cur_id++;
		sids.push_back(cur_id);
		sread.push_back(true); // don't take current content into account
		id = cur_id;
		rbool = true;
	}
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

bool bmsg::unsubscribe(int id) {
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	int index = findindex(id);
	if (index>=0) {
		sids.erase(sids.begin()+index);
		sread.erase(sread.begin()+index);
		rbool = true;
	}
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

bool bmsg::read(int id, std::string& msg) {
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	rbool = !hassread(id);
	if (rbool) {
		msg = smsg;
		marksread(id);
	}
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

bool bmsg::broadcast(std::string msg) {
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	rbool = clrsread();
	smsg = msg;
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

bool bmsg::hasmessage(int id) {
	bool rbool = false;
	pthread_mutex_lock(&mtx);
	rbool = !hassread(id);
	pthread_mutex_unlock(&mtx);
	return (rbool);
}

int bmsg::size() {
	int rint = -1;
	pthread_mutex_lock(&mtx);
	rint = sids.size();
	pthread_mutex_unlock(&mtx);
	return (rint);	
}

bool bmsg::contains(int id) {
	return (findindex(id)>=0);
}

int bmsg::findindex(int id) {
	int index = -1;
	vector<int>::iterator it = find(sids.begin(), sids.end(), id);
	if (it < sids.end()) {
		index = std::distance(sids.begin(), it);
	}
	return(index);
}

bool bmsg::clrsread() {
	bool rbool = false;
	if (sread.size()>0) {
		for (unsigned int i=0; i < sread.size(); i++) {
			sread[i]=false;
		}
		rbool = true;
	}
	return (rbool);
}

bool bmsg::marksread(int id) {
	bool rbool = false;

	int index = findindex(id);
	if (index>=0) {
		sread[index]=true;
		rbool = true;
	}
	return (rbool);
}

bool bmsg::hassread(int id) {
	bool rbool = true; // set read if unavailable

	int index = findindex(id);
	if (index>=0) {
		rbool = sread[index];
	}
	return (rbool);	
}

void bmsg::print() {
	for (unsigned int i=0; i < sids.size(); i++) {
		printf("%d ,",sids[i]);
	}
	printf("\n");
	for (unsigned int i=0; i < sread.size(); i++) {
		printf("%d ,",(int)sread[i]);
	}
	printf("\n");
}