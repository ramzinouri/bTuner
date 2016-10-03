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
	bLog::AddLog(bLogEntry("bTuner v0.0.0.1 Started", "bTuner App", LogType::Info));
	_bTWin=new bTWin;
	


	_bTWin->Create();
	_bTWin->CenterWindow();

	if (bDebug)
	{
		_bLogWin = new bLogWin;
		_bLogWin->Create();
		_bLogWin->MoveWindow(_bTWin->GetWindowRect().right,_bTWin->GetWindowRect().top,400,500);
	}
	_bTWin->SetFocus();


	if (!_bTWin->GetHwnd())
	{
		::MessageBox(NULL, _T("Failed to create Frame window"), _T("ERROR"), MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
};