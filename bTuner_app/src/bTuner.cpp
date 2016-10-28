#include "bTuner.h"
#include "bLog.h"

bTuner::bTuner()
{
#ifdef NDEBUG
	bDebug = false;
#else
	bDebug = true;
#endif
};

BOOL bTuner::InitInstance()
{
	std::wstring msg = TEXT(APP_NAME);
	msg += TEXT(" ");
	msg += TEXT(VERSION_TEXT);
	msg += L" Started";
	bLog::AddLog(bLogEntry(msg, L"bTuner App", eLogType::Info));
	_bTWin=new bTWin;

	_bTWin->Create();

	if (!_bTWin->GetHwnd())
	{
		bLog::AddLog(bLogEntry(L"Failed to create Main window", L"bTuner App", eLogType::Error));
		return FALSE;
	}

	return TRUE;
}