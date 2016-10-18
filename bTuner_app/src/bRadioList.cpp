#include"bRadioList.h"

bRadioList::bRadioList()
{
	
};

bRadioList::~bRadioList()
{

};

void bRadioList::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD | LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SINGLESEL | LVS_OWNERDRAWFIXED;
};

void bRadioList::DrawItem(WPARAM wParam, LPARAM lParam)
{
	PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
	HFONT font;

	if (pdis->itemID == -1)
		return;

		font=CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
		SelectObject(pdis->hDC, font);
		

		CString text=this->GetItemText(pdis->itemID, 0);


		if (pdis->itemState & ODS_SELECTED)
			FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)CreateSolidBrush(RGB(100, 100, 100)));
		else if (pdis->itemState & ODS_DEFAULT)
			FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)));

		::SetTextColor(pdis->hDC, RGB(255, 255, 255));
		SetBkMode(pdis->hDC, TRANSPARENT);
		TextOut(pdis->hDC, 5, pdis->rcItem.top + 5, text.c_str(), text.GetLength());


}

void bRadioList::OnCreate()
{
	MoveWindow(150, 0, GetParent().GetClientRect().Width() - 150, GetParent().GetClientRect().Height() - 150);
	InsertColumn(0, L"Stations", LVCFMT_LEFT, GetParent().GetClientRect().Width() - 150 - GetSystemMetrics(SM_CXVSCROLL));
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_TWOCLICKACTIVATE);
	SetBkColor(RGB(0, 0, 0));
	SetTextColor(RGB(255, 255, 255));
	SetTextBkColor(RGB(0, 0, 0));

}
