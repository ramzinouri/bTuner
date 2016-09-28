#include "bTuner.h"
#include "bLog.h"

vector<bLogEntry>* bLog::Log = new vector<bLogEntry>;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR CmdLine,int iCmdShow)
{
	bTuner *_bTuner;
	HANDLE hMutex;
	DWORD  dwReturn;
	BOOL   fGotMutex;
	char   szAppname[_MAX_PATH];
	char   szFname[_MAX_FNAME];
	GetModuleFileNameA(hInstance, szAppname,sizeof(szAppname));
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
	
		catch (std::exception &e)
		{
			::MessageBoxA(NULL,e.what(),"Exeption",MB_ICONERROR);
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