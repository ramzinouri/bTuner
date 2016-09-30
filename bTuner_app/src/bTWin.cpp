#include "bTWin.h"


bTWin::bTWin()
{
	MainMenu=new CMenu(IDR_MENU1);
};

bTWin::~bTWin()
{
	
};

void bTWin::PreRegisterClass(WNDCLASS &wc)
{
	wc.style=CS_GLOBALCLASS;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.cbClsExtra=0;
    wc.cbWndExtra=0;
	wc.hIcon=LoadIconA(::GetModuleHandle(NULL),MAKEINTRESOURCEA(IDI_ICON));
	wc.hbrBackground=(HBRUSH) CreateSolidBrush(RGB(30,30,30));
	wc.lpszClassName=L"bTuner";
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
};

void bTWin::PreCreate(CREATESTRUCT &cs)
{

	cs.dwExStyle = WS_EX_ACCEPTFILES;
	cs.lpszClass=L"bTuner";
	cs.lpszName=L".:: bTuner ::. V 0.0.1.1";
	cs.style=WS_OVERLAPPED|WS_DLGFRAME|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE;
	cs.cx=700;
	cs.cy=500;
};
int bTWin::OnCreate(CREATESTRUCT& cs)
{
	Player.OpenURL("http://stream12.iloveradio.de/iloveradio1-aac.mp3");
	
	return 0;
};


void bTWin::OnDraw(CDC& dc)
{
	CRect cr = GetClientRect();
	RECT r;
	SetRect(&r, 0, cr.bottom - 150, cr.right, cr.bottom);
	dc.FillRect(r,(HBRUSH)CreateSolidBrush(RGB(0, 0, 0)));

	SetRect(&r, 5, cr.bottom - 145, 145, cr.bottom-5);
	
	dc.BeginPath();
	dc.RoundRect(r, 10, 10);
	dc.EndPath();
	dc.SelectClipPath(RGN_COPY);

	dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(150, 150, 150)));



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
};

BOOL bTWin::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{

	case ID_FILE_EXIT:
		PostQuitMessage(0);
		break;

	case ID_FILE_OPEN_FILE:
		break;
	case ID_FILE_OPEN_URL:
		break;
	case ID_HELP_ABOUT:
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_ABOUT), this->GetHwnd(), (DLGPROC)&bTWin::AboutDiagproc);
		break;
		
	}
	return TRUE;
};

INT_PTR _stdcall bTWin::AboutDiagproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m) {
	case WM_INITDIALOG:
		break;
	case WM_CLOSE:
		EndDialog(h, NULL);
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)l)->code)
		{
		  case NM_CLICK:
			  if (((LPNMHDR)l)->idFrom == IDC_SYSLINK2)
			  {
				
				  PNMLINK pNMLink = (PNMLINK)((LPNMHDR)l);
				  ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
			  }
		      break;

		}
		break;
	case WM_COMMAND:
		if (LOWORD(w) == IDOK) 
			EndDialog(h, NULL);
		break;
	}
	return 0;
}