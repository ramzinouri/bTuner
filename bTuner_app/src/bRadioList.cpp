#include"bRadioList.h"

bRadioList::bRadioList()
{
	firstdraw = true;
};

bRadioList::~bRadioList()
{

};

void bRadioList::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOTIFY | LBS_SORT;
};

void bRadioList::DrawItem(WPARAM wParam, LPARAM lParam)
{
	PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
	TCHAR achBuffer[MAX_PATH];
	HFONT font;

	if (pdis->itemID == -1)
		return;
	switch (pdis->itemAction)
	{
	case ODA_SELECT:
	case ODA_DRAWENTIRE:
		if (firstdraw)
		{
			FillRect(pdis->hDC, GetClientRect(), (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)));
			firstdraw = false;
		}

		font=CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
		SelectObject(pdis->hDC, font);
		
		SendMessage(pdis->hwndItem, LB_GETTEXT, pdis->itemID, (LPARAM)achBuffer);
		SetTextColor(pdis->hDC, RGB(255, 255, 255));
		SetBkMode(pdis->hDC, TRANSPARENT);
		FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)));
		TextOut(pdis->hDC, 5,pdis->rcItem.top+5, achBuffer, wcslen(achBuffer));


		if (pdis->itemState & ODS_SELECTED)
		{
			FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)CreateSolidBrush(RGB(100, 100, 100)));
			SetTextColor(pdis->hDC, RGB(255, 255, 255));
			SetBkMode(pdis->hDC, TRANSPARENT);
			TextOut(pdis->hDC, 5, pdis->rcItem.top+5, achBuffer, wcslen(achBuffer));
		}

		break;

	case ODA_FOCUS:

		// Do not process focus changes. The focus caret 
		// (outline rectangle) indicates the selection. 
		// The IDOK button indicates the final 
		// selection. 
		break;
	}
}

void bRadioList::OnDraw(CDC& dc)
{
	
	CRect r = GetClientRect();
	dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)));
};

LRESULT bRadioList::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	switch (uMsg)
	{
	case WM_NOTIFY:
		this->CloseWindow();
		break;
	}


	return WndProcDefault(uMsg, wParam, lParam);
}