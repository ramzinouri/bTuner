#include "bLog.h"
#include <Windows.h>

bLogWin* bLog::_bLogWin=NULL;

bLogEntry::bLogEntry()
{
	Type = eLogType::Info;
	time(&Time);
};

bLogEntry::bLogEntry(wstring _Msg, wstring _source, eLogType _type)
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
	if(_bLogWin)
		_bLogWin->AddLog();
};
