#include"bRadioList.h"

bRadioList::bRadioList()
{
	PlayingNowID = -1;
};

bRadioList::~bRadioList()
{

};

void bRadioList::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_NOCOLUMNHEADER | LVS_OWNERDRAWFIXED;
};
void bRadioList::AddStation(const bStation& station)
{
	int i = GetItemCount();
	LV_ITEM *it = new LV_ITEM;
	it->mask = LVIF_TEXT;
	it->iSubItem = 0;
	it->pszText = (LPWSTR)station.Name.c_str();
	it->iItem = i;
	InsertItem(*it);

	LV_ITEM *id = new LV_ITEM;
	id->mask = LVIF_TEXT;
	id->iSubItem = 1;
	CString sID;
	sID.Format(L"%u", i);
	id->pszText = (LPWSTR)sID.c_str();
	id->lParam = (LPARAM)sID.c_str();
	id->iItem = i;
	SetItem(*id);

	delete it;
	delete id;
}

void bRadioList::RedrawPlaylist()
{
	DeleteAllItems();
	for (unsigned int i = 0; i < Playlist->Stations.size(); i++)
		AddStation(Playlist->Stations.at(i));
}

void bRadioList::DrawItem(WPARAM wParam, LPARAM lParam)
{
	PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
	HBRUSH brush;
	if (pdis->itemID == -1)
		return;

		CString text=GetItemText(pdis->itemID, 0);

		if(pdis->itemID%2)
			brush=CreateSolidBrush(RGB(0, 0, 0));
		else
			brush=CreateSolidBrush(RGB(15, 15, 15));

		if (pdis->itemState & ODS_SELECTED)
			brush=CreateSolidBrush(RGB(30, 150, 220));

		FillRect(pdis->hDC, &pdis->rcItem, brush);

		DeleteObject(brush);
		::SetTextColor(pdis->hDC, RGB(255, 255, 255));
		if(PlayingNowID ==pdis->itemID)
			::SetTextColor(pdis->hDC, RGB(140, 255, 140));

		SetBkMode(pdis->hDC, TRANSPARENT);
		TextOut(pdis->hDC, 20, pdis->rcItem.top + 7, text.c_str(), text.GetLength());


}

void bRadioList::OnCreate()
{
	MoveWindow(150, 35, GetParent().GetClientRect().Width() - 150, GetParent().GetClientRect().bottom - 185);
	InsertColumn(0, L"Stations", LVCFMT_LEFT, GetParent().GetClientRect().Width() - 150 - GetSystemMetrics(SM_CXVSCROLL),0);
	InsertColumn(1, L"ID", LVCFMT_LEFT | LVCFMT_FIXED_WIDTH,0,1);
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_TWOCLICKACTIVATE);
	SetBkColor(RGB(0, 0, 0));
	SetTextColor(RGB(255, 255, 255));
	SetTextBkColor(RGB(0, 0, 0));
	HFONT font;
	font = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
	SetFont(font);
}