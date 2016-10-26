#ifndef BLOG_H
#define BLOG_H
#include "bLogWin.h"
#include "bString.h"

#include <string>
#include <ctime>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

enum  eLogType
{
	Error, Info, Track, Player, Debug
};

class bLogEntry
{
public:
	bLogEntry();
	bLogEntry(wstring _Msg, wstring _source, eLogType _type);

	wstring Msg;
	wstring Source;
	eLogType Type;
	time_t Time;
};

class bLog
{
public:
	static vector<bLogEntry>* Log;
	static void AddLog(bLogEntry _Entry);
	static bLogWin* _bLogWin;
};


#endif