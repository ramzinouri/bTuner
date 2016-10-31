#include "bModulesList.h"
#include <gdiplus.h>

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

	Gdiplus::Graphics graphics(pdis->hDC);
	Gdiplus::Bitmap *bitmap;

	HRSRC hResource = ::FindResourceW(::GetModuleHandle(0), MAKEINTRESOURCE(IDB_MODULES), L"PNG");
	DWORD imageSize = ::SizeofResource(::GetModuleHandle(0), hResource);
	const void* pResourceData = ::LockResource(::LoadResource(::GetModuleHandle(0),hResource));
	HGLOBAL m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (m_hBuffer)
	{
		void* pBuffer = ::GlobalLock(m_hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);

			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
			{
				bitmap = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();

			}
			::GlobalUnlock(m_hBuffer);
		}
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}


	Gdiplus::Rect r;
	r.X = pdis->rcItem.left;
	r.Y = pdis->rcItem.top;
	r.Width = pdis->rcItem.bottom-pdis->rcItem.top;
	r.Height = pdis->rcItem.bottom - pdis->rcItem.top;
	graphics.DrawImage(bitmap,r, pdis->itemID*(pdis->rcItem.bottom - pdis->rcItem.top),0, pdis->rcItem.bottom - pdis->rcItem.top, pdis->rcItem.bottom - pdis->rcItem.top, Gdiplus::UnitPixel);
	delete bitmap;
}

void bModulesList::OnCreate()
{
	MoveWindow(0, 35, 220, GetParent().GetClientRect().bottom - 185);
	InsertColumn(0, L"Modules", LVCFMT_LEFT, 220 - GetSystemMetrics(SM_CXVSCROLL), 0);
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_TWOCLICKACTIVATE);
	SetBkColor(RGB(30, 30, 30));
	SetTextColor(RGB(255, 255, 255));
	SetTextBkColor(RGB(30, 30, 30));

	Modules.insert(std::make_pair(std::wstring(L"Favorites"), new bFavorites));
	Modules.insert(std::make_pair(std::wstring(L"Xiph.org"), new bXiphorg));
	Modules.insert(std::make_pair(std::wstring(L"History"), new bHistory));
	
	
	SetActiveModule(L"Favorites");
	int i = 0;
	for (auto &pair : Modules) {
		if (pair.second->UpdateStations())
		{
			LV_ITEM *it = new LV_ITEM;
			it->mask = LVIF_TEXT;
			it->iSubItem = 0;
			it->pszText = (LPWSTR)pair.first.c_str();
			it->iItem = i;
			InsertItem(*it);
			delete it;
		}
		else
			bLog::AddLog(bLogEntry(L"Error Loading Module:"+pair.first, L"Module list", eLogType::Error));
		i++;
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
