#include "bLogWin.h"
#include "bLog.h"
#include <sstream>

bLogWin::bLogWin()
{

};

bLogWin::~bLogWin()
{
	
};

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
	_font= CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Lucida Console"));
	_bLogList.SetFont(_font);

	int i = 0;
	stringstream msg;
	struct tm*  timeinfo;
	timeinfo = localtime(&bLog::Log->at(i).Time);
	msg << timeinfo->tm_hour;
	msg << ":";
	msg << timeinfo->tm_min;
	msg << ":";
	msg << timeinfo->tm_sec;
	msg << " ::  ";
	msg << bLog::Log->at(i).Msg.c_str();
	_bLogList.AddString(msg.str().c_str());
	return 0;
};

LRESULT bLogWin::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		this->CloseWindow();
		break;
	case WM_SIZE:
		if (::IsWindow(_bLogList) != NULL)
			_bLogList.MoveWindow(1, 1, this->GetWindowRect().Width() - 17, this->GetWindowRect().Height() - 37, 1);
		
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