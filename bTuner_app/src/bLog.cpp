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
	struct tm*  timeinfo;
	timeinfo = localtime(&bLog::Log->at(bLog::Log->size() - 1).Time);
	CString ms;
	ms.Format(L"%02d:%02d:%02d :: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	ms += bLog::Log->at(bLog::Log->size() - 1).Msg.c_str();

	if(_bLogWin)
		_bLogWin->AddLog(ms.c_str());
	std::fstream ostream;
	if(Log->size()==1)
		ostream.open(L"bTuner.log", std::ios::trunc | std::ios::out);
	else
		ostream.open(L"bTuner.log", std::ios::app | std::ios::out);
	if (ostream.is_open())
	{
		std::string s=bString::ConvertToS(ms.c_str());
		ostream.write(s.c_str(),s.length());
		ostream.write("\n", 1);
		ostream.close();
	}

};
