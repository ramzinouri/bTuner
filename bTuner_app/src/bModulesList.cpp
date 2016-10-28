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
	HBRUSH brush;

	if (pdis->itemID == -1)
		return;

	font = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));

	SelectObject(pdis->hDC, font);
	::SetTextColor(pdis->hDC, RGB(255, 255, 255));
	CString ms = GetItemText(pdis->itemID, 0);

	if(ActiveModuleName==ms.c_str())
		brush=CreateSolidBrush(RGB(30, 150, 220));
	else
		brush = CreateSolidBrush(RGB(30, 30, 30));

	FillRect(pdis->hDC, &pdis->rcItem, brush);
	DeleteObject(brush);
	

	SetBkMode(pdis->hDC, TRANSPARENT);
	TextOut(pdis->hDC, 45, pdis->rcItem.top+10, ms, ms.GetLength());
	DeleteObject(font);
}

void bModulesList::OnCreate()
{
	MoveWindow(0, 35, 220, GetParent().GetClientRect().bottom - 185);
	InsertColumn(0, L"Modules", LVCFMT_LEFT, 220 - GetSystemMetrics(SM_CXVSCROLL), 0);
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_TWOCLICKACTIVATE);
	SetBkColor(RGB(30, 30, 30));
	SetTextColor(RGB(255, 255, 255));
	SetTextBkColor(RGB(30, 30, 30));

	Modules.emplace(std::make_pair(std::wstring(L"Favorites"), new bFavorites));
	SetActiveModule(L"Favorites");
	for (auto &pair : Modules) {
		if (pair.second->UpdateStations())
		{
			LV_ITEM *it = new LV_ITEM;
			it->mask = LVIF_TEXT;
			it->iSubItem = 0;
			it->pszText = (LPWSTR)pair.first.c_str();
			it->iItem = 0;
			InsertItem(*it);
		}
		else
			bLog::AddLog(bLogEntry(L"Error Loading Module:"+pair.first, L"Module list", eLogType::Error));
	}
}

bModule * bModulesList::GetActiveModule()
{
	return Modules.at(ActiveModuleName);
}

void bModulesList::SetActiveModule(std::wstring name)
{
	ActiveModuleName = name;
}

void bModulesList::PreCreate(CREATESTRUCT & cs)
{
	cs.style = WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_NOCOLUMNHEADER | LVS_OWNERDRAWFIXED;
}
