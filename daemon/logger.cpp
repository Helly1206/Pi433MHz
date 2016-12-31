/*
 * logger.cpp
 * Sourcefile for logging for ...
 *
 * Copyright 26-12-2016 Helly
 */

#include <iostream>
#include <fstream>
#include <algorithm> 
#include <string>
#include <cstdio>
#include <ctime>

#include "logger.h"

/* defines */
#define STR_NONE    "NONE   "
#define STR_DEBUG   "DEBUG  "
#define STR_NOTICE  "NOTICE "
#define STR_WARNING "WARNING"
#define STR_ERROR   "ERROR  "
#define STR_NORMAL  "NORMAL "
#define STR_DEFPATH "/var/log/"
#define LOG_DEFLEVEL LOG_NONE
#define MAX_LOG      256
#define MAX_LFN      80
#define MAX_TMLVL    20
#define DEF_LOGLINES 10000
#define DEF_OLDLOGS  3

using namespace std;

/* typedefs */

/* globals */

bool Logger::InitDone = false;
int Logger::logClevel = LOG_DEFLEVEL;
string Logger::LogFile = "";
int Logger::logLines = 0;

Logger::Logger() {
}

void Logger::Init(string LogName, int Clevel) {
	LogFile = LogName;
    logClevel = Clevel;
    CheckLogFile();   
    CheckNSwitchLogFile();
    InitDone = true; 
}

void Logger::Log(int level, const char *sformat, ...) {
	if (InitDone) {
		if ( ((logClevel&0x03) == (level&0x03)) || (logClevel == LOG_DEBUG) ) { // correct level
			string slevel;
			switch(level) {
        		case LOG_DEBUG:
            		slevel = STR_DEBUG;
            		break;
        		case LOG_NOTICE:
            		slevel = STR_NOTICE;
            		break;
        		case LOG_WARNING:
            		slevel = STR_WARNING;
            		break;
        		case LOG_ERROR:
            		slevel = STR_ERROR;
            		break;
        		case LOG_NONE:
            		slevel = STR_NONE;
            		break;
        		default:
        			slevel = STR_NORMAL;
            		break;
        	}
        	// format logline
        	char tmbuf[MAX_TMLVL];
        	time_t now = time (0);
        	struct tm *sTm = localtime (&now);

        	strftime (tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", sTm);

        	va_list ap;

    		va_start(ap, sformat);
    		std::string buffer = format(sformat,ap);
    		va_end(ap);
        	// append logline
        	ofstream myfile(LogFile.c_str(), ios::out | ios::app);
  			if (myfile.is_open()) {
        		myfile << format("%s %s %s\n",tmbuf, slevel.c_str(),buffer.c_str());
        		myfile.close();
        	}
        	logLines++;
        	// check and switch file
        	CheckNSwitchLogFile();

		}		
	}
}

void Logger::CheckLogFile(void) {
	ifstream myfile (LogFile.c_str());
	string s;
	logLines = 0;

	if (myfile.is_open()) {
		while ( getline (myfile,s) ) {
			logLines++;
		}
		myfile.close();
	}
}

void Logger::CheckNSwitchLogFile(void) {
	if (logLines >= DEF_LOGLINES) { // move files to .x
		for(int i=DEF_OLDLOGS;i>0;i--) {
			string file1, file2;
			file2 = format("%s.%d",LogFile.c_str(),i);
			if (i>1) {
				file1 = format("%s.%d",LogFile.c_str(),i-1);
			} else {
				file1 = LogFile;
			}
			//printf("rename: %s to %s\n",file1.c_str(),file2.c_str());
			rename(file1.c_str(), file2.c_str()); 

		}
		remove(LogFile.c_str());
		logLines = 0;
		Log(LOG_NOTICE,"LogFile Switched, new logfile created");
	}
}

string Logger::format(const char *const fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    std::string result = format(fmt,ap);
    va_end(ap);

    return result;
}

string Logger::format(const char *const fmt, va_list args) {
    char *buffer = NULL;

    (void)vasprintf(&buffer, fmt, args);
    
    std::string result = buffer;
    free(buffer);

    return result;
}

