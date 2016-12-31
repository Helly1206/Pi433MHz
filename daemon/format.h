/*
 * format.h
 * Headerfile for formatting string to/from int array for ...
 *
 * Copyright 26-12-2016 Helly
 */

#ifndef FORMAT_H
#define FORMAT_H

/* defines */


class format {
	public:
		// Constructor
		format() {};
		virtual ~format() {};
		static int decode(std::string data, int *array, int maxsize);
		static int encode(std::string &data, int *array, int size);
		static bool testarray(int *array, int size);
		static bool teststring(std::string data, int maxsize);
		
	//private:
};

#endif
