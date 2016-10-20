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
		ms.Format(L"%02d:%02d:%02d :: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		ms  += bLog::Log->at(i).Msg.c_str();
		_bLogList.InsertString(i,ms.c_str());
	}
}
void bLogWin::AddLog()
{
		struct tm*  timeinfo;
		timeinfo = localtime(&bLog::Log->at(bLog::Log->size()-1).Time);
		CString ms;
		ms.Format(L"%02d:%02d:%02d :: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		ms += bLog::Log->at(bLog::Log->size()-1).Msg.c_str();
		_bLogList.AddString(ms.c_str());
		_bLogList.SetCurSel(bLog::Log->size()-1);

}


void bLogWin::PreRegisterClass(WNDCLASS &wc)
{
	wc.style=CS_GLOBALCLASS;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.cbClsExtra=0;
    wc.cbWndExtra=0;
	wc.hIcon=LoadIconA(::GetModuleHandle(NULL),MAKEINTRESOURCEA(IDI_ICON));
	wc.hbrBackground=(HBRUSH) CreateSolidBrush(RGB(255,255,255));
	wc.lpszClassName=L"bTuner_log";
};

void bLogWin::PreCreate(CREATESTRUCT &cs)
{
	cs.lpszClass=L"bTuner_log";
	cs.lpszName=L"bTuner Log";
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
	PDRAWITEMSTRUCT pdis;
	switch (uMsg)
	{
	case WM_DRAWITEM:

		pdis = (PDRAWITEMSTRUCT)lParam;
		if (pdis->hwndItem == _bLogList)
			_bLogList.DrawItem(wParam, lParam);
		break;
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

}
void bLogList::DrawItem(WPARAM wParam, LPARAM lParam)
{
	PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
	HFONT font;

	if (pdis->itemID == -1)
		return;

	font = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));

	SelectObject(pdis->hDC, font);

	FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)CreateSolidBrush(RGB(255, 255, 255)));
	::SetTextColor(pdis->hDC, RGB(0, 0, 0));

	if (bLog::Log->at(pdis->itemID).Type==eLogType::Error)
		::SetTextColor(pdis->hDC, RGB(255, 0, 0));
	if (bLog::Log->at(pdis->itemID).Type == eLogType::Player)
		FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)CreateSolidBrush(RGB(40, 185, 220)));
	if (bLog::Log->at(pdis->itemID).Type == eLogType::Info)
		FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)CreateSolidBrush(RGB(255, 255, 255)));
	if (bLog::Log->at(pdis->itemID).Type == eLogType::Debug)
		::SetTextColor(pdis->hDC, RGB(0, 150, 0));
	if (bLog::Log->at(pdis->itemID).Type == eLogType::Track)
		FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)CreateSolidBrush(RGB(180, 230, 240)));

	struct tm*  timeinfo;
	timeinfo = localtime(&bLog::Log->at(pdis->itemID).Time);
	CString ms;
	ms.Format(L"%02d:%02d:%02d :: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	ms += bLog::Log->at(pdis->itemID).Msg.c_str();

	SetBkMode(pdis->hDC, TRANSPARENT);
	TextOut(pdis->hDC, 5, pdis->rcItem.top , ms, ms.GetLength());

};

void bLogList::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_CHILD| WS_VISIBLE| WS_VSCROLL | WS_BORDER |LBS_NOSEL| LBS_OWNERDRAWFIXED;
};