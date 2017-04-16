/*
 * config.cpp
 * Sourcefile for reading and storing configuration for ..
 *
 * Copyright 26-12-2016 Helly
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <string>
#include <stdint.h>
#include "config.h"
#include "logger.h"

using namespace std;

Config::Config() {
    SetDefaults();
}

string Config::GetFileName(string FileName, string EtcLoc) {
    string retstr = "";
    string fname;

    if (*EtcLoc.rbegin() == '/') {
        fname = EtcLoc + FileName;
    } else {
        fname = EtcLoc + "/" + FileName;
    }
    ifstream f(fname.c_str());
    if (f.good()) {
        retstr = fname;
    } else {
        fname = "./" + FileName;
        ifstream f(fname.c_str());
        if (f.good()) {
        	retstr = fname;
    	} 
    }
    return (retstr);
}

int Config::Load(string FileName) {
    int retval = 0;
    string s;
    ifstream myfile (FileName.c_str());
    if (myfile.is_open()) {
        while ( getline (myfile,s) ) {
            if (!s.empty()) {
                trim(s);
                if (s.at(0) != '#') {
                    ParseLine(s);
                }
            }
        }
        myfile.close();
    } else {
        retval = 1;
    }

    return(retval);
}

void Config::PrintConfig() {
	Logger::Log(LOG_NOTICE,"Current Configuration:");
	Logger::Log(LOG_NOTICE,"%s = %d",STR_LOGLEVEL,LogLevel);
	Logger::Log(LOG_NOTICE,"%s = %s",STR_LOGFILE,LogFile.c_str());
	Logger::Log(LOG_NOTICE,"%s = %d",STR_PORT,Port);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_MAXCLIENTS,MaxClients);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_RXDEVICE,RxDevice);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_RXREPEATS,RxRepeats);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_RXGPIO,RxGpio);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_RXQUEUE,RxQueue);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_RXECHO,RxEcho);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_TXDEVICE,TxDevice);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_TXPERIOD,TxPeriod);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_TXREPEATS,TxRepeats);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_TXGPIO,TxGpio);
	Logger::Log(LOG_NOTICE,"%s = %d",STR_TXQUEUE,TxQueue);
    Logger::Log(LOG_NOTICE,"%s = %d",STR_PLLCHARGE,PllCharge);
    Logger::Log(LOG_NOTICE,"%s = %d",STR_TXRXGPIO,TxRxGpio);
    Logger::Log(LOG_NOTICE,"%s = %d",STR_ENABLEGPIO,EnableGpio);
}

void Config::SetDefaults() {
    LogLevel = CFG_DEF_LOG;
    LogFile = CFG_DEF_LOGFILE;
    Port = CFG_DEF_PORT;
    MaxClients = CFG_DEF_MAXCL;
    RxDevice = CFG_DEF_X;
    RxRepeats = CFG_DEF_RXRPTS;
    RxGpio = CFG_DEF_RXGPIO;
    RxQueue = CFG_DEF_RXQ;
    RxEcho = 0;
    TxDevice = CFG_DEF_X;
    TxPeriod = 0;
    TxRepeats = CFG_DEF_TXRPTS;
    TxGpio = CFG_DEF_TXGPIO;
    TxQueue = CFG_DEF_TXQ;
    PllCharge = CFG_DEF_PLLCH;
    TxRxGpio = CFG_DEF_TRXGPIO;
    EnableGpio = CFG_DEF_ENAGPIO;
}

void Config::ParseLine(string s) {
    string Param = "";
    string Value = "";

    int pos = s.find("=");
    if (pos>0) {
        Param=s.substr(0,pos);
        trim(Param);
        Value=s.substr(pos);
        trim(Value,"=");
        trim(Value);
        StoreSetting(Param, Value);
    }
}

void Config::StoreSetting(string Param, string Value) { 
    transform(Param.begin(), Param.end(), Param.begin(), ::toupper);
    if (Param == STR_LOGLEVEL) {
        transform(Value.begin(), Value.end(), Value.begin(), ::tolower);
        if (Value == STR_NONE) {
            LogLevel = LOG_NONE;    
        } else if (Value == STR_DEBUG) {
            LogLevel = LOG_DEBUG;    
        } else { // STR_NORMAL
            LogLevel = LOG_NORMAL;
        }
    } else if (Param == STR_LOGFILE) {
        LogFile = Value;
    } else if (Param == STR_PORT) {
        Port = (unsigned short)atoi(Value.c_str());
    } else if (Param == STR_MAXCLIENTS) {
        MaxClients = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_RXDEVICE) {
        RxDevice = GetDevice(Value);
    } else if (Param == STR_RXREPEATS) {
        RxRepeats = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_RXGPIO) {
        RxGpio = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_RXQUEUE) {
        RxQueue = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_RXECHO) {
        RxEcho = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_TXDEVICE) {
        TxDevice = GetDevice(Value);
    } else if (Param == STR_TXPERIOD) {
        TxPeriod = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_TXREPEATS) {
        TxRepeats = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_TXGPIO) {
        TxGpio = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_TXQUEUE) {
        TxQueue = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_PLLCHARGE) {
        PllCharge = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_TXRXGPIO) {
        TxRxGpio = (uint8_t)atoi(Value.c_str());
    } else if (Param == STR_ENABLEGPIO) {
        EnableGpio = (uint8_t)atoi(Value.c_str());
    }

    //printf("Param: '%s', Value: '%s'\n",Param.c_str(),Value.c_str());
}

uint8_t Config::GetDevice(string Value) { 
    uint8_t Device=CFG_DEF_X;
    transform(Value.begin(), Value.end(), Value.begin(), ::tolower);
    if (Value == STR_OFF) {
        Device = X_OFF;    
    } else if (Value == STR_ECHO) {
        Device = X_ECHO;    
    } else if (Value == STR_RAW) {
        Device = X_RAW;    
    } else if (Value == STR_ACTION) {
        Device = X_ACTION;    
    } else if (Value == STR_BLOKKER) {
        Device = X_BLOKKER;    
    } else if (Value == STR_KAKU) {
        Device = X_KAKU;    
    } else if (Value == STR_ELRO) {
        Device = X_ELRO;    
    }

    return (Device);
}

string& Config::trim(string& s, const char* t) {
    s.erase(0, s.find_first_not_of(t));
    s.erase(s.find_last_not_of(t) + 1);
    return(s);
}