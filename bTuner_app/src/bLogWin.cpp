#include "bLogWin.h"
#include "bLog.h"
#include <sstream>

bLogWin::bLogWin()
{
	bLog::_bLogWin = NULL;
}

bLogWin::~bLogWin()
{
	bLog::_bLogWin = NULL;
}

void bLogWin::UpdateLog()
{
	_bLogList.DeleteAllItems();
	for (int i = 0; i <(int)bLog::Log->size(); i++)
	{
		struct tm*  timeinfo;
		timeinfo = localtime(&bLog::Log->at(i).Time);
		CString ms;
		ms.Format(L"%02d:%02d:%02d :: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		ms  += bLog::Log->at(i).Msg.c_str();
		AddLog(i,ms.c_str());
	}
}

void bLogWin::AddLog(int pos,std::wstring _log)
{
	LV_ITEM *it = new LV_ITEM;
	it->mask = LVIF_TEXT;
	it->iSubItem = 0;
	it->pszText = (LPWSTR)_log.c_str();
	it->iItem = pos;
	_bLogList.InsertItem(*it);

	CRect rect;
	_bLogList.GetItemRect(0, rect, LVIR_BOUNDS);

	CSize cs;
	cs.cx = 0;
	cs.cy = pos *rect.Height();
	_bLogList.Scroll(cs);

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
	bLog::_bLogWin = this;
	_bLogList.Create(GetHwnd());
	_bLogList.OnCreate();

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
		{
			_bLogList.MoveWindow(0, 0, GetClientRect().right, GetClientRect().bottom);
			_bLogList.SetColumnWidth(0, GetClientRect().right - GetSystemMetrics(SM_CXVSCROLL));
			_bLogList.RedrawWindow();
		}
		
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
	HBRUSH brush;
	PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
	HFONT font;

	if (pdis->itemID == -1)
		return;

	font = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
	SelectObject(pdis->hDC, font);
	::SetTextColor(pdis->hDC, RGB(0, 0, 0));


	if (bLog::Log->at(pdis->itemID).Type == eLogType::Error)
	{
		::SetTextColor(pdis->hDC, RGB(255, 0, 0));
		brush = CreateSolidBrush(RGB(255, 255, 255));
	}
	if (bLog::Log->at(pdis->itemID).Type == eLogType::Player)
		brush=CreateSolidBrush(RGB(40, 185, 220));
	if (bLog::Log->at(pdis->itemID).Type == eLogType::Info)
		brush=CreateSolidBrush(RGB(255, 255, 255));
	if (bLog::Log->at(pdis->itemID).Type == eLogType::Debug)
	{
		::SetTextColor(pdis->hDC, RGB(0, 150, 0));
		brush = CreateSolidBrush(RGB(255, 255, 255));
	}
	if (bLog::Log->at(pdis->itemID).Type == eLogType::Track)
		brush=CreateSolidBrush(RGB(180, 230, 240));

	FillRect(pdis->hDC, &pdis->rcItem, brush);

	CString  text = GetItemText(pdis->itemID, 0);
	TextOut(pdis->hDC, 5, pdis->rcItem.top , text, text.GetLength());

	DeleteObject(brush);
	DeleteObject(font);
};

void bLogList::PreCreate(CREATESTRUCT &cs)
{
	cs.style = cs.style = WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_NOCOLUMNHEADER | LVS_OWNERDRAWFIXED;
};

void bLogList::OnCreate()
{
	InsertColumn(0, L"Logs", LVCFMT_LEFT, 500 - GetSystemMetrics(SM_CXVSCROLL), 0);
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	SetBkColor(RGB(255, 255, 255));
	SetTextColor(RGB(0, 0, 0));
	SetTextBkColor(RGB(255, 255, 255));
	ShowScrollBar(SB_VERT,TRUE);
}