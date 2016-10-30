#include"bRadioList.h"


namespace
{
	bRadioList *gp_bRadioList = NULL;
}

bRadioList::bRadioList(): PlayingNowID(-1), killthread(false), hThread (NULL)
{
	gp_bRadioList = this;
}

bRadioList::~bRadioList()
{

}

void bRadioList::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_NOCOLUMNHEADER | LVS_OWNERDRAWFIXED;
}

void bRadioList::AddStation(const bStation station)
{
	int i = GetItemCount();
	LV_ITEM *it = new LV_ITEM;
	it->mask = LVIF_TEXT;
	it->iSubItem = 0;
	it->pszText = (LPWSTR)station.Name.c_str();
	it->iItem = i;
	ListView_InsertItem(*this, it);

	it->iSubItem = 1;
	CString sID;
	sID.Format(L"%u", station.ID);
	it->pszText = (LPWSTR)sID.c_str();
	ListView_SetItem(*this, it);

	delete it;
}

unsigned __stdcall bRadioList::StaticThreadEntry(void* c)
{
	UINT_PTR i= (UINT_PTR)c;
	if (i == eRadioThread::RedrawPlaylist)
		gp_bRadioList->T_RedrawPlaylist();
	if (i == eRadioThread::DrawOnly)
		gp_bRadioList->T_DrawOnly();

	return  0;
}

void bRadioList::RedrawPlaylist()
{
	
	if (hThread)
	{
		std::unique_lock<std::mutex> lk(m);
		killthread = true;
		cv.notify_all();	
		hThread = NULL;
	}

	hThread = (HANDLE)_beginthreadex(NULL, 0, &bRadioList::StaticThreadEntry, (void*)eRadioThread::RedrawPlaylist , 0, &threadID);

}

void bRadioList::T_RedrawPlaylist()
{
	DeleteAllItems();
	for (unsigned int i = 0; i < Playlist->Stations.size(); i++)
	{
		AddStation(Playlist->Stations.at(i));
		std::unique_lock<std::mutex> lk(m);
		cv.wait_for(lk, std::chrono::milliseconds(1));
		if (killthread)
		{
			killthread = false;
			break;
		}
	}
	CloseHandle(hThread);
	hThread = NULL;
	_endthreadex(0);
}

void bRadioList::DrawOnly(std::vector<unsigned int> items)
{
	m_items = items;
	
	if (hThread)
	{
		std::unique_lock<std::mutex> lk(m);
		killthread = true;
		cv.notify_all();
		hThread = NULL;
	}
	hThread = (HANDLE)_beginthreadex(NULL, 0, &bRadioList::StaticThreadEntry, (void*)eRadioThread::DrawOnly, 0, &threadID);
	
}

void bRadioList::T_DrawOnly()
{
	DeleteAllItems();
	for (unsigned int i = 0; i < m_items.size(); i++)
	{
		AddStation(Playlist->Stations.at(m_items[i]));
		std::unique_lock<std::mutex> lk(m);
		cv.wait_for(lk, std::chrono::milliseconds(1));
		if (killthread)
		{
			killthread = false;
			break;
		}
	}
	CloseHandle(hThread);
	hThread = NULL;
	_endthreadex(0);
}

void bRadioList::DrawItem(WPARAM wParam, LPARAM lParam)
{
	PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
	HBRUSH brush;
	HFONT font;
	font = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Open Sans"));
	SelectObject(pdis->hDC,font);
	
	if (pdis->itemID == -1)
		return;

		CString text1=GetItemText(pdis->itemID, 0);
		CString text = text1;
		if (Playlist->title == L"History")
			text.Format(L"[ %u ]- %s", pdis->itemID+1, text1.c_str());

		if(pdis->itemID%2)
			brush=CreateSolidBrush(RGB(0, 0, 0));
		else
			brush=CreateSolidBrush(RGB(15, 15, 15));

		if (pdis->itemState & ODS_SELECTED)
		{
			DeleteObject(brush);
			brush = CreateSolidBrush(RGB(30, 150, 220));
		}

		FillRect(pdis->hDC, &pdis->rcItem, brush);

		DeleteObject(brush);
		::SetTextColor(pdis->hDC, RGB(255, 255, 255));
		if(PlayingNowID ==pdis->itemID)
			::SetTextColor(pdis->hDC, RGB(140, 255, 140));

		SetBkMode(pdis->hDC, TRANSPARENT);
		TextOut(pdis->hDC, 20, pdis->rcItem.top + 7, text.c_str(), text.GetLength());
		DeleteObject(font);
}

void bRadioList::OnCreate()
{
	MoveWindow(220, 35, GetParent().GetClientRect().right-220, GetParent().GetClientRect().bottom - 185);
	InsertColumn(0, L"Stations", LVCFMT_LEFT, GetClientRect().Width() - GetSystemMetrics(SM_CXVSCROLL),0);
	InsertColumn(1, L"ID", LVCFMT_RIGHT | LVCFMT_FIXED_WIDTH,0,1);
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );
	SetBkColor(RGB(0, 0, 0));
	SetTextColor(RGB(255, 255, 255));
	SetTextBkColor(RGB(0, 0, 0));
}