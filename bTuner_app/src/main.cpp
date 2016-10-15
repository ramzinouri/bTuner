#include "bTuner.h"
#include "bLog.h"

vector<bLogEntry>* bLog::Log = new vector<bLogEntry>;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR CmdLine,int iCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(CmdLine);
	UNREFERENCED_PARAMETER(iCmdShow);
#ifdef _DEBUG
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(0);
#endif

	bTuner *_bTuner;
	HANDLE hMutex;
	DWORD  dwReturn;
	BOOL   fGotMutex;
	char   szAppname[_MAX_PATH];
	char   szFname[_MAX_FNAME];
	GetModuleFileNameA(GetModuleHandle(NULL), szAppname,sizeof(szAppname));
	_splitpath(szAppname, NULL, NULL, szFname, NULL);
	strupr(szFname);
	hMutex = CreateMutexA(NULL,FALSE,szFname);
	dwReturn = WaitForSingleObject(hMutex,500);
	fGotMutex = (dwReturn == WAIT_OBJECT_0) || (dwReturn == WAIT_ABANDONED);
	if (fGotMutex)
	{
		try
		{
			_bTuner=new bTuner;
			return _bTuner->Run();
		}
	
		catch (const CException &e)
		{
			MessageBox(NULL, e.GetText(), A2T(e.what()), MB_ICONERROR);
			return -1;
		}
		ReleaseMutex(hMutex);
	}
	else
	{
		::MessageBoxA(NULL,"bTuner already running","Error",MB_ICONERROR);
		return -1;
	};
	return 0;
};