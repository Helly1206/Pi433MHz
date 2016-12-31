/*
 * logger.h
 * Headerfile for logging for ..
 *
 * Copyright 26-12-2016 Helly
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <cstdarg>

/* defines */
#define LOG_NONE	0 // 0000
#define LOG_NORMAL	1 // 0001
#define LOG_DEBUG	2 // 0010
#define LOG_NOTICE	5 // 0101 
#define LOG_WARNING	9 // 1001
#define LOG_ERROR	13 // 1101

/* typedefs */

/* globals */

/* Function Prototypes */
class Logger {
	public:
		// Constructor
		Logger();
		void Init(std::string LogName, int Clevel);
		static void Log(int level, const char *sformat, ...);
		static std::string format(const char *const fmt, ...);
	private:
		void CheckLogFile(void);
		static void CheckNSwitchLogFile(void);
		static std::string format(const char *const fmt, va_list args);
		//void LOG_SetLevel();
		static bool InitDone;
		static int logClevel;
		static std::string LogFile;
		static int logLines;
};
#endif
