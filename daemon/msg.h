/*
 * msg.h
 * Headerfile for message queue for ...
 *
 * Copyright 26-12-2016 Helly
 */

#ifndef MSG_H
#define MSG_H

#include <pthread.h>
#include <queue>
#include <vector>

/* defines */


class qmsg {
	public:
		// Constructor
		qmsg();
		virtual ~qmsg();
		void qset();
		void qclr();
		bool qget();
		// static call made possible to be able to access from signal handler
		static void qset_();
		static void qclr_();
		static bool qget_();
	private:
		static bool q;
		static pthread_mutex_t mtx;
		static bool IsInit;
};

class msg {
	public:
		// Constructor
		msg(unsigned int qsize);
		virtual ~msg();

		bool read(std::string& msg);
		bool get(std::string& msg);
		bool set(std::string msg);
		bool hasmessage();
	private:
		pthread_mutex_t mtx;
		unsigned int q_size;
		std::queue <std::string> msgs;
		bool IsInit;	
};

class smsg { // static call made possible to be able to access from callbacks
	public:
		// Constructor
		smsg(unsigned int qsize);
		virtual ~smsg();

		static bool read(std::string& msg);
		static bool get(std::string& msg);
		static bool set(std::string msg);
		static bool hasmessage();
	private:
		static pthread_mutex_t mtx;
		static unsigned int q_size;
		static std::queue <std::string> msgs;
		static bool IsInit;	
};

class bmsg {
	public:
		// Constructor
		bmsg(unsigned int ssize);
		virtual ~bmsg();

		bool subscribe(int& id);
		bool unsubscribe(int id);

		bool read(int id, std::string& msg);
		bool broadcast(std::string msg);
		bool hasmessage(int id);
		int size();
		
	private:
		bool contains(int id);
		int findindex(int id);
		bool clrsread();
		bool marksread(int id);
		bool hassread(int id);
		void print();

		pthread_mutex_t mtx;
		unsigned int s_size;
		unsigned int cur_id;
		std::string smsg;
		std::vector <int> sids;
		std::vector <bool> sread;
};

#endif
