#include "bTuner.h"
#include "bLogWin.h"
#include "bLog.h"

bTuner::bTuner()
{
#ifdef NDEBUG
	bDebug = false;
#else
	bDebug = true;
#endif
};

bTuner::~bTuner()
{

};

BOOL bTuner::InitInstance()
{
	bLog::AddLog(bLogEntry(L"bTuner v0.0.0.1 Started", L"bTuner App", eLogType::Info));
	_bTWin=new bTWin;

	_bTWin->Create();

	if (!_bTWin->GetHwnd())
	{
		bLog::AddLog(bLogEntry(L"Failed to create Main window", L"bTuner App", eLogType::Error));
		return FALSE;
	}

	return TRUE;
};