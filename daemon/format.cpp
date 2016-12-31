/*
 * format.cpp
 * Sourcefile for formatting string to/from int array for ..
 *
 * Copyright 26-12-2016 Helly
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <string>
#include "format.h"

using namespace std;

int format::decode(string data, int *array, int maxsize) {
	int res = -1;

	if ((array != NULL) && (!data.empty())) {
		int i=0;
		string d = data;

		while (!d.empty() && (i<maxsize)) {
			int pos = d.find("!");
			string val=d.substr(0,pos);
			d = d.substr(pos+1);
			array[i]=atoi(val.c_str());
			i++;
		}
		res = i;
	}

	return (res);
}

int format::encode(string &data, int *array, int size) {
	int res = -1;
	data = "";

	if (array != NULL) {
		for (int i=0; i<size; i++) {
			char str[15];
			sprintf(str, "%d!", array[i]);
			data += str;
		}
		res = size;
	}

	return (res);
}

bool format::testarray(int *array, int size) {
	bool bres = false;
	string enc = "";
	int res = encode(enc, array, size);
	if (res > 0) {
		int narr[size+2];
		int res2=decode(enc, narr, size+2);
		if (res == res2) {
			int res3 = 0;
			for (int i=0; i<res; i++) {
				if (array[i]==narr[i]) res3++;
			}
			if (res == res3) bres = true;
		}
	}
	return (bres);
}

bool format::teststring(std::string data, int maxsize) {
	bool bres = false;
	int narr[maxsize];
	string enc = "";
	int res = decode(data, narr, maxsize);
	if (res > 0) {
		string enc = "";
		int res2 = encode(enc, narr, res);
		if (res == res2) {
			if (enc == data) bres = true;
		}
	}
	return (bres);
}