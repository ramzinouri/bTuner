#include "bTWin.h"
#include "./API/TuneIn.h"

bTWin::bTWin()
{
		// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
};

bTWin::~bTWin()
{
	GdiplusShutdown(m_gdiplusToken);
};

void bTWin::PreRegisterClass(WNDCLASS &wc)
{
	wc.style=CS_GLOBALCLASS;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.cbClsExtra=0;
    wc.cbWndExtra=0;
	wc.hIcon=LoadIconA(::GetModuleHandle(NULL),MAKEINTRESOURCEA(IDI_ICON));
	wc.hbrBackground=(HBRUSH) CreateSolidBrush(RGB(0,0,0));
	wc.lpszClassName=L"bTuner";
};

void bTWin::PreCreate(CREATESTRUCT &cs)
{

	cs.dwExStyle = WS_EX_ACCEPTFILES;
	cs.lpszClass=L"bTuner";
	cs.lpszName=L".:: bTuner ::. V 0.0.1.1";
	cs.style=WS_OVERLAPPED|WS_DLGFRAME|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE;
	cs.cx=500;
	cs.cy=400;
};

void bTWin::OnDraw(CDC* pDC)
{
	Graphics myGraphics(*pDC);
	CListBox _list;
	_list.Create();

	//TuneIn _tunein;
	//_tunein.Tune("s53927");

};

LRESULT bTWin::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	}

	return WndProcDefault(uMsg, wParam, lParam);
}
