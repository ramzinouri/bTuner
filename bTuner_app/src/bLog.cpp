#include "bLog.h"
#include <Windows.h>

bLogEntry::bLogEntry()
{
	Type = LogType::Info;
	time(&Time);
};

bLogEntry::bLogEntry(string _Msg, string _source, LogType _type)
{
	Msg = _Msg;
	Source = _source;
	Type = _type;
	time(&Time);
};



bLog::bLog()
{
	
	MessageBoxA(NULL,"blog","blog created",MB_OK);
};

void bLog::AddLog(bLogEntry _Entry)
{
	bLog::Log->emplace_back(_Entry);

};