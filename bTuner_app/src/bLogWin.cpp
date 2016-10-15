#include "bLogWin.h"
#include "bLog.h"
#include <sstream>

bLogWin::bLogWin()
{
	bLog::_bLogWin = this;
};

bLogWin::~bLogWin()
{
	bLog::_bLogWin = NULL;
};

void bLogWin::UpdateLog()
{
	_bLogList.ResetContent();
	for (int i = 0; i <(int)bLog::Log->size(); i++)
	{
		struct tm*  timeinfo;
		timeinfo = localtime(&bLog::Log->at(i).Time);
		CString ms;
		ms.Format("%02d:%02d:%02d :: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		ms  += bLog::Log->at(i).Msg.c_str();
		_bLogList.InsertString(i,ms.c_str());
	}
	_bLogList.RedrawWindow();
}

void bLogWin::PreRegisterClass(WNDCLASS &wc)
{
	wc.style=CS_GLOBALCLASS;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.cbClsExtra=0;
    wc.cbWndExtra=0;
	wc.hIcon=LoadIconA(::GetModuleHandle(NULL),MAKEINTRESOURCEA(IDI_ICON));
	wc.hbrBackground=(HBRUSH) CreateSolidBrush(RGB(255,255,255));
	wc.lpszClassName="bTuner_log";
};

void bLogWin::PreCreate(CREATESTRUCT &cs)
{
	cs.lpszClass="bTuner_log";
	cs.lpszName="bTuner Log";
	cs.style= WS_TILEDWINDOW |WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE;
};

int  bLogWin::OnCreate(CREATESTRUCT& cs)
{
	_bLogList.Create(this->GetHwnd());
	HFONT _font;
	_font= CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
	_bLogList.SetFont(_font);

	UpdateLog();
	return 0;
};

LRESULT bLogWin::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		bLog::_bLogWin = NULL;
		this->CloseWindow();
		break;
	case WM_SIZE:
		if (::IsWindow(_bLogList) != NULL)
			_bLogList.MoveWindow(0, 0, GetClientRect().right, GetClientRect().bottom);
		
		break;
	}

	return WndProcDefault(uMsg, wParam, lParam);
}


bLogList::bLogList()
{

};

bLogList::~bLogList()
{

};

void bLogList::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_CHILD| WS_VISIBLE| WS_VSCROLL | WS_BORDER |LBS_NOSEL;
};