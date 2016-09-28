#include <windows.h>
#include <iostream>



int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR CmdLine,int iCmdShow)
{
	
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
			
			return 0;
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