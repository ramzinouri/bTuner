#ifndef BLOG_H
#define BLOG_H
#include "bLogWin.h"

#include <string>
#include <ctime>
#include <time.h>
#include <vector>

using namespace std;

enum  LogType
{
	Error,Info
};

class bLogEntry
{
public:
	bLogEntry();
	bLogEntry(string _Msg, string _source, LogType _type);


	string Msg;
	string Source;
	LogType Type;
	time_t Time;
};

class bLog
{
public:
	bLog();
	static vector<bLogEntry>* Log;
	static void AddLog(bLogEntry _Entry);
	static bLogWin* _bLogWin;
};


#endif