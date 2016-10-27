#include "bModulesList.h"

bModulesList::bModulesList()
{
}

bModulesList::~bModulesList()
{
}

void bModulesList::DrawItem(WPARAM wParam, LPARAM lParam)
{
	PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
	HFONT font;

	if (pdis->itemID == -1)
		return;

	font = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));

	SelectObject(pdis->hDC, font);
	::SetTextColor(pdis->hDC, RGB(255, 255, 255));

	if(pdis->itemState& ODS_SELECTED)
		FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)CreateSolidBrush(RGB(30, 150, 220)));
	else
		FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)CreateSolidBrush(RGB(30, 30, 30)));

	
	wchar_t buffer[255];
	this->GetText(pdis->itemID,buffer);
	CString ms(buffer);

	SetBkMode(pdis->hDC, TRANSPARENT);
	TextOut(pdis->hDC, 45, pdis->rcItem.top+10, ms, ms.GetLength());
	DeleteObject(font);
}

void bModulesList::OnCreate()
{
	MoveWindow(0, 35, 220, GetParent().GetClientRect().bottom - 185);
}

void bModulesList::PreCreate(CREATESTRUCT & cs)
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED;
}
