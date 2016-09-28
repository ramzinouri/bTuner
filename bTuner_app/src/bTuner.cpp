#include "bTuner.h"
#include "bLogWin.h"
#include "bLog.h"

bTuner::bTuner()
{
	bDebug = true;
};

bTuner::~bTuner()
{

};

BOOL bTuner::InitInstance()
{
	bLog::AddLog(bLogEntry("bTuner v0.0.1.1 Started", "bTuner App", LogType::Info));
	_bTWin=new bTWin;


	_bTWin->Create();

	if (bDebug)
	{
		_bLogWin = new bLogWin;
		_bLogWin->Create(NULL);
		_bLogWin->MoveWindow(1050,150,400,600);
	}



	if (!_bTWin->GetHwnd())
	{
		::MessageBox(NULL, _T("Failed to create Frame window"), _T("ERROR"), MB_ICONERROR);
		return FALSE;
	}

	_bTWin->CenterWindow();
	return TRUE;
};