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

	hMutex = CreateMutex(NULL,FALSE, L"BTUNER");
	dwReturn = WaitForSingleObject(hMutex,200);
	fGotMutex = (dwReturn == WAIT_OBJECT_0) || (dwReturn == WAIT_ABANDONED);
	if (fGotMutex)
	{
		try
		{
			_bTuner=new bTuner;
			int r=_bTuner->Run();
			return r;
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
		::MessageBox(NULL,L"bTuner already running",L"Error",MB_ICONERROR);
		return -1;
	};
	return 0;
};