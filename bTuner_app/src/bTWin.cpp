#include "bTWin.h"
#include "bLog.h"
#include "bString.h"
#include "bHttp.h"
#include <ctime>
#include <time.h>

namespace
{
	bTWin *gp_bTWin = NULL;
}

INT_PTR CALLBACK  g_Diagproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	if (gp_bTWin)
		return gp_bTWin->Diagproc(h, m, w, l);
	else
		return 0;
}

bTWin::bTWin(): Clicked(FALSE), Hover(bHover::None)
{
	gp_bTWin = this;
	Mouse.x = 0;
	Mouse.y = 0;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

void  bTWin::OnClose()
{
	bLog::AddLog(bLogEntry(L"bTuner Closed", L"bTuner Win", eLogType::Info));
	if (bLog::_bLogWin)
		bLog::_bLogWin->CloseWindow();
	CWnd::OnClose();
}

void  bTWin::OnTimer(int TimerID)
{
	if (TimerID == 0)
	{
		QWORD progress = BASS_StreamGetFilePosition(Player.chan, BASS_FILEPOS_BUFFER) * 100 / BASS_StreamGetFilePosition(Player.chan, BASS_FILEPOS_END);
		Player.BuffProgress = (int)progress;
		if (progress > 90 || !BASS_StreamGetFilePosition(Player.chan, BASS_FILEPOS_CONNECTED))
		{
			::KillTimer(GetHwnd(), 0);
			if (Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding == eCodecs::UNDIFINED)
			{
				BASS_CHANNELINFO  info2;
				BASS_ChannelGetInfo(Player.chan, &info2);
				if (info2.ctype == BASS_CTYPE_STREAM_MP3)
					Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = eCodecs::MP3;
				if (info2.ctype == BASS_CTYPE_STREAM_OGG)
					Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = eCodecs::OGG;
				if (info2.ctype == BASS_CTYPE_STREAM_AAC)
					Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = eCodecs::AAC;
				if (info2.ctype == BASS_CTYPE_STREAM_OPUS)
					Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = eCodecs::OPUS;
			}
			if (Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate == 0)
			{
				float bit = 0;
				BASS_ChannelGetAttribute(Player.chan, BASS_ATTRIB_BITRATE, &bit);
				Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate = (unsigned int)bit;
			}
			BASS_ChannelSetSync(Player.chan, BASS_SYNC_META, 0, g_MetaSync, 0);
			BASS_ChannelSetSync(Player.chan, BASS_SYNC_OGG_CHANGE, 0, &g_MetaSync, 0);
			BASS_ChannelSetSync(Player.chan, BASS_SYNC_END, 0, &g_EndSync, 0);

			Player.Play();
		}
		else
		{
			Player.status = eStatus::Buffering;
			Player.UpdateWnd();
		}
	}
	if (TimerID == 1)
	{
		Player.UpdateWnd();
	}

	if (TimerID == 2)
	{
		::KillTimer(GetHwnd(), 2);
		if (searchbox.GetWindowTextLength() > 0 && searchbox.GetWindowText() != L"Search")
		{
			std::wstring q = searchbox.GetWindowText();
			std::vector<unsigned int> result = bList.Playlist->Search(q);
			if (result.size())
				bList.DrawOnly(result);
			else
				bList.DeleteAllItems();
		}
		else if (bList.Playlist&&searchbox.GetWindowTextLength() == 0 && searchbox.GetWindowText() != L"Search")
			bList.RedrawPlaylist();
	}

}

BOOL bTWin::OnEraseBkgnd(CDC & dc)
{
	return TRUE;
}

void bTWin::OnContextMenu(HWND wnd, CPoint point)
{
	if (wnd == bList.GetHwnd())
	{

		int id = bList.GetNextItem(-1, LVNI_FOCUSED);
		if (id < 0)
			return;
		CString sID = bList.GetItemText(id, 1);
		if (!sID.GetLength())
			return;
		int index = std::stoi(sID.c_str());

		if (index >= 0 && index < (int)bList.Playlist->Stations.size())
		{
			ContextMenu = ::CreatePopupMenu();
			StreamMenu = ::CreatePopupMenu();
			CMenu menu(ContextMenu);
			CMenu Streammenu(StreamMenu);

			MENUINFO mi;
			memset(&mi, 0, sizeof(mi));
			mi.cbSize = sizeof(mi);
			mi.fMask = MIM_STYLE;
			mi.dwStyle = MNS_NOTIFYBYPOS;
			mi.dwMenuData = 0;
			Streammenu.SetMenuInfo(mi);
			for (unsigned int i = 0; i < bList.Playlist->Stations[index].Streams.size(); i++)
			{
				CString s;
				s.Format(L"[%02u] ", i + 1);
				s += bList.Playlist->Stations[index].Streams[i].Url.c_str();
				Streammenu.AppendMenuW(MF_STRING, NULL, s.c_str());
			}

			mi.dwMenuData = 1;
			menu.SetMenuInfo(mi);
			menu.InsertMenu(0, MF_STRING, NULL, L"Play Station");
			menu.InsertMenu(1, MF_SEPARATOR);
			menu.InsertMenu(2, MF_POPUP, (UINT_PTR)Streammenu.GetHandle(), L"Streams");
			menu.InsertMenu(3, MF_SEPARATOR);
			
			if (bList.Playlist->title.find(L"bFavorites") != std::wstring::npos)
			{
				menu.InsertMenu(4, MF_STRING | MF_DISABLED | MF_GRAYED, NULL, L"Add station to favorites");
				menu.InsertMenu(5, MF_STRING | MF_DISABLED | MF_GRAYED, NULL, L"Quick add station to favorites");
				menu.InsertMenu(6, MF_STRING, NULL, L"Remove from favorites");
			}
			else
			{
				menu.InsertMenu(4, MF_STRING, NULL, L"Add station to favorites");
				menu.InsertMenu(5, MF_STRING, NULL, L"Quick add station to favorites");
				menu.InsertMenu(6, MF_STRING | MF_DISABLED | MF_GRAYED, NULL, L"Remove from favorites");
			}
			menu.InsertMenu(7, MF_SEPARATOR);
			menu.InsertMenu(8, MF_STRING, NULL, L"Go to website");

			::SetForegroundWindow(wnd);
			int ret=menu.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_HORPOSANIMATION, point.x, point.y, *this);
			Streammenu.DestroyMenu();
			menu.DestroyMenu();
		}
		
	}
	return;
}

void bTWin::OnMenuCommand(HMENU menu, int idx)
{
	if (ContextMenu == menu)
	{
		
		if (idx == 0)
		{
			int id = bList.GetNextItem(-1, LVNI_SELECTED);
			if (id < 0)
				return;
			CString sID = bList.GetItemText(id, 1);
			if (!sID.GetLength())
				return;
			unsigned int index = std::stoi(sID.c_str());
			if (index >= 0 && index < (int)bList.Playlist->Stations.size())
			{
				Player.OpenStation(bList.Playlist->Stations.at(index));
				int old = bList.PlayingNowID;
				bList.PlayingNowID = id;
				bList.RedrawItems(id, id);
				bList.RedrawItems(old, old);
			}
		}
		if (idx == 4)
		{
			int id = bList.GetNextItem(-1, LVNI_SELECTED);
			if (id < 0)
				return;
			CString sID = bList.GetItemText(id, 1);
			if (!sID.GetLength())
				return;
			unsigned int index = std::stoi(sID.c_str());
			if (index >= 0 && index < (int)bList.Playlist->Stations.size())
			{
				DialogBoxParamW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADDTOFAVORITES), GetHwnd(), (DLGPROC)&g_Diagproc, (LPARAM)&bList.Playlist->Stations.at(index));
			}
		}
		if (idx == 5)
		{
			int id = bList.GetNextItem(-1, LVNI_SELECTED);
			if (id < 0)
				return;
			CString sID = bList.GetItemText(id, 1);
			if (!sID.GetLength())
				return;
			unsigned int index = std::stoi(sID.c_str());
			if (index >= 0 && index < (int)bList.Playlist->Stations.size())
			{
				Modulelist.Modules[L"Favorites"]->Playlist->Stations.push_back(bList.Playlist->Stations.at(index));
				if (!Modulelist.Modules[L"Favorites"]->Playlist->SaveXSPF(L"bFavorites.xspf"))
					bLog::AddLog(bLogEntry(L"Error Saving Favorites File [bFavorites.xspf]", L"bTWin", eLogType::Error));
				Modulelist.Modules[L"Favorites"]->UpdateStations();
				if (Modulelist.ActiveModuleName == L"Favorites")
					bList.RedrawPlaylist();
			}
		}
		if (idx == 6)
		{
			int id = bList.GetNextItem(-1, LVNI_SELECTED);
			if (id < 0)
				return;
			CString sID = bList.GetItemText(id, 1);
			if (!sID.GetLength())
				return;
			unsigned int index = std::stoi(sID.c_str());
			if (index >= 0 && index < (int)bList.Playlist->Stations.size())
			{
				int i = Modulelist.Modules[L"Favorites"]->Playlist->Locate(bList.Playlist->Stations.at(index).Name);
				if(i<0)
					Modulelist.Modules[L"Favorites"]->Playlist->Stations.erase(Modulelist.Modules[L"Favorites"]->Playlist->Stations.begin() + index);
				else
					Modulelist.Modules[L"Favorites"]->Playlist->Stations.erase(Modulelist.Modules[L"Favorites"]->Playlist->Stations.begin() + i);
				if (!Modulelist.Modules[L"Favorites"]->Playlist->SaveXSPF(L"bFavorites.xspf"))
					bLog::AddLog(bLogEntry(L"Error Saving Favorites File [bFavorites.xspf]", L"bTWin", eLogType::Error));
				Modulelist.Modules[L"Favorites"]->UpdateStations();
				if (Modulelist.ActiveModuleName == L"Favorites")
					bList.RedrawPlaylist();
			}
		}
		if (idx == 8)
		{
			int id = bList.GetNextItem(-1, LVNI_SELECTED);
			if (id < 0)
				return;
			CString sID = bList.GetItemText(id, 1);
			if (!sID.GetLength())
				return;
			unsigned int index = std::stoi(sID.c_str());
			if (index >= 0 && index < (int)bList.Playlist->Stations.size())
			{
				ShellExecuteW(NULL, L"open", bList.Playlist->Stations.at(index).Url.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
		}

	}
	else if (StreamMenu == menu)
	{
		int id = bList.GetNextItem(-1, LVNI_SELECTED);
		if (id < 0)
			return;
		CString sID = bList.GetItemText(id, 1);
		if (!sID.GetLength())
			return;
		unsigned int index = std::stoi(sID.c_str());
		if (index >= 0 && index < (int)bList.Playlist->Stations.size())
		{
			bList.Playlist->Stations.at(index).PlayedStreamID = idx;
			Player.OpenStation(bList.Playlist->Stations.at(index));
			int old= bList.PlayingNowID;
			bList.PlayingNowID = id;
			bList.RedrawItems(id, id);
			bList.RedrawItems(old, old);
		}
	}
}

void bTWin::OnDestroy()
{
	GdiplusShutdown(gdiplusToken);
	Config.LastVolume= Player.GetVolume();
	Config.LastPlayed = *Player.PlayingNow;
	Config.LastWindowPos.x = GetWindowRect().left;
	Config.LastWindowPos.y = GetWindowRect().top;
	
	if (!Config.Save())
		bLog::AddLog(bLogEntry(L"Failed to Save Config File", L"bTuner Win", eLogType::Error));
}

bTWin::~bTWin()
{
	
}

void bTWin::PreRegisterClass(WNDCLASS &wc)
{
	wc.style=CS_GLOBALCLASS;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.cbClsExtra=0;
    wc.cbWndExtra=0;
	wc.hIcon=LoadIconA(::GetModuleHandle(NULL),MAKEINTRESOURCEA(IDI_ICON));
	wc.hbrBackground=(HBRUSH) CreateSolidBrush(RGB(45,45,48));
	wc.lpszClassName=TEXT(APP_NAME);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
}

void bTWin::PreCreate(CREATESTRUCT &cs)
{
	cs.dwExStyle = WS_EX_ACCEPTFILES;
	cs.lpszClass= TEXT(APP_NAME);
	cs.lpszName = TEXT(APP_TITLE);
	cs.style = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE ;
	cs.cx=800;
	cs.cy=565;
}

int bTWin::OnCreate(CREATESTRUCT& cs)
{	
	Player.hwnd = this->GetHwnd();
	if (!Config.Load())
		bLog::AddLog(bLogEntry(L"Failed to Load Config File", L"bTuner Win", eLogType::Error));
	if(!Config.LastPlayed.Streams[0].Url.size())
		GetMenu().EnableMenuItem(ID_PLAYBACK_RESUME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	GetMenu().EnableMenuItem(ID_PLAYBACK_STOP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	if (Config.LogWindow)
		GetMenu().CheckMenuItem(ID_HELP_LOGWINDOW, MF_CHECKED );
	else
		GetMenu().CheckMenuItem(ID_HELP_LOGWINDOW, MF_UNCHECKED);
	GetMenu().SetMenuItemBitmaps(ID_FILE_EXIT, MF_BYCOMMAND, HBMMENU_MBAR_CLOSE, HBMMENU_MBAR_CLOSE);
	SetRect(&VolumeRect, GetClientRect().right - 120, GetClientRect().bottom - 57, GetClientRect().right - 20, GetClientRect().bottom - 43);
	SetRect(&PlayRect, 160, GetClientRect().bottom - 70, 250, GetClientRect().bottom - 30);
	Player.SetVolume(Config.LastVolume);
	MoveWindow(Config.LastWindowPos.x,Config.LastWindowPos.y,800,565);
	Player.PlayingNow = new bStation;
	Player.PlayingNow->Streams.push_back(bStream(Config.LastPlayed.Streams.at(0)));
	Player.PlayingNow->Name = Config.LastPlayed.Name;
	Player.PlayingNow->Url = Config.LastPlayed.Url;
	Player.PlayingNow->Image = Config.LastPlayed.Image;

	bList.Create(*this);
	bList.OnCreate();

	Modulelist.Create(*this);
	Modulelist.OnCreate();


	searchbox.Create(*this);
	searchbox.MoveWindow(GetClientRect().right-205,7,200,20);
	HFONT font;
	font = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
	searchbox.SetFont(font);
	DeleteObject(font);
	searchbox.SetWindowTextW(L"Search");
	searchbox.SetMargins(10, 10);

//#define TESTSWITCH

#ifdef TESTSWITCH
	
	bLog::AddLog(bLogEntry(L"Start Downloading .... [http://dir.xiph.org/yp.xml]", L"bTuner Win", eLogType::Info));
	if (bHttp::DownloadFile(L"http://dir.xiph.org/yp.xml", L"yp.xml"))
	{
		bLog::AddLog(bLogEntry(L"Finish Downloading [http://dir.xiph.org/yp.xml]", L"bTuner Win", eLogType::Info));
		xml_document doc;
		bLog::AddLog(bLogEntry(L"Loading .... yp.xml", L"bTuner Win", eLogType::Info));
		xml_parse_result result = doc.load_file("yp.xml");

		if (result)
		{
			xml_node nDir = doc.child(L"directory");
			Playlist = new bPlaylist;
			Playlist->title = L"dir.xiph.org/yp.xml";
			int i = 0;
			for (xml_node nEntry = nDir.first_child(); nEntry; nEntry = nEntry.next_sibling())
			{
				
				int pos=Playlist->Locate(nEntry.child(L"server_name").text().as_string());
				if (pos > 0)
				{
					bStream s(nEntry.child(L"listen_url").text().as_string());
					s.Bitrate = nEntry.child(L"bitrate").text().as_int();
					Playlist->Stations.at(pos).Streams.push_back(s);
					continue;
				}
				
				bStation st;
				st.Name = nEntry.child(L"server_name").text().as_string();
				st.Genre = nEntry.child(L"genre").text().as_string();
				bStream s(nEntry.child(L"listen_url").text().as_string());
				s.Bitrate = nEntry.child(L"bitrate").text().as_int();
				st.Streams.push_back(s);
				st.ID = i;
				Playlist->Stations.push_back(st);

				i++;
			}

			CString msg;
			msg.Format(L"[ %u ] Station Loaded From yp.xml", Playlist->Stations.size());
			bLog::AddLog(bLogEntry(msg.c_str(), L"bTuner Win", eLogType::Info));
		}
	}

#endif
	

	bList.Playlist = Modulelist.GetActiveModule()->Playlist;
	bList.RedrawPlaylist();
	bList.PlayingNowID = Modulelist.GetActiveModule()->Playlist->Locate(Player.PlayingNow->Name);

	UpdateWindow();
	SetTimer(1, 500, NULL);

	if (Config.LogWindow)
	{
		bLog::_bLogWin = new bLogWin;
		bLog::_bLogWin->Create();
		bLog::_bLogWin->MoveWindow(GetWindowRect().right, GetWindowRect().top, 500, GetWindowRect().Height());
		if (!bLog::_bLogWin->GetHwnd())
			bLog::AddLog(bLogEntry(L"Failed to create Log window", L"bTuner App", eLogType::Error));
	}
	SetFocus();
	return 0;
};

void bTWin::OnDraw(HDC dc)
{
	HDC hdcBuffer = CreateCompatibleDC(dc);
	int iOldState = SaveDC(hdcBuffer);
	int h, w;
	h = GetClientRect().Height();
	w = GetClientRect().Width();
	HBITMAP hBitmapBuffer = CreateCompatibleBitmap(dc, w, h);
	SelectObject(hdcBuffer, hBitmapBuffer); 

	//Client rect
	HBRUSH bkcolor = CreateSolidBrush(RGB(45, 45, 48));
	FillRect(hdcBuffer, GetClientRect(), bkcolor);
	DeleteObject(bkcolor);
	//playlist rect
	HFONT font = CreateFont(22, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	SelectObject(hdcBuffer, font);
	SIZE sv;
	RECT r;
	GetTextExtentPoint32(hdcBuffer, Modulelist.GetActiveModule()->Playlist->title.c_str(), (int)Modulelist.GetActiveModule()->Playlist->title.size(), &sv);
	SetRect(&r, GetClientRect().right - bList.GetClientRect().Width() + 4, 35 - sv.cy - 6, GetClientRect().right - bList.GetClientRect().Width() + 16 + sv.cx, 35);
	HBRUSH  playlistcolor = CreateSolidBrush(RGB(63, 63, 70));
	FillRect(hdcBuffer, &r, playlistcolor);
	DeleteObject(playlistcolor);

	SetRect(&r, GetClientRect().right-bList.GetClientRect().Width()+5,35-sv.cy-5, GetClientRect().right - bList.GetClientRect().Width() + 15+sv.cx, 35);
	playlistcolor = CreateSolidBrush(RGB(30, 30, 30));
	FillRect(hdcBuffer, &r, playlistcolor);
	DeleteObject(playlistcolor);

	SetTextColor(hdcBuffer,RGB(180, 180, 180));
	SetBkMode(hdcBuffer,TRANSPARENT);
	TextOut(hdcBuffer, GetClientRect().right - bList.GetClientRect().Width() + 10, 35 - sv.cy, Modulelist.GetActiveModule()->Playlist->title.c_str(), (int)Modulelist.GetActiveModule()->Playlist->title.size());
	DeleteObject(font);

	//Player Draw
	DrawPlayer(hdcBuffer);	

	
	BitBlt(dc,0,0, w, h, hdcBuffer, 0, 0, SRCCOPY);
	RestoreDC(hdcBuffer, iOldState);
	DeleteObject(hBitmapBuffer);
	DeleteDC(hdcBuffer); 
	
}

LRESULT bTWin::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int prv;
	PDRAWITEMSTRUCT pdis;
	PMEASUREITEMSTRUCT pmis;
	switch (uMsg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(GetHwnd(), &ps);

		OnDraw(hdc);

		::EndPaint(GetHwnd(), &ps);
	}
	return 0;
	case WM_MENUCOMMAND:
	{		
		HMENU menu = (HMENU)lParam;
		int idx =(int)wParam;
		OnMenuCommand(menu, idx);
		break; 
	}
	case WM_CONTEXTMENU:
		OnContextMenu((HWND)wParam,CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;
	case WM_CTLCOLOREDIT:
		return (LRESULT)CreateSolidBrush(RGB(127,127,127));
	case WM_MEASUREITEM:
		pmis = (PMEASUREITEMSTRUCT)lParam;
		if (pmis->CtlType == ODT_LISTVIEW)
		{
			pmis->itemHeight = 40;
		}
		else

			
		break;

	case WM_DRAWITEM:

		pdis = (PDRAWITEMSTRUCT)lParam;
		if (pdis->hwndItem == 0)
			break;
		if (pdis->hwndItem == bList.GetHwnd())
			bList.DrawItem(wParam, lParam);
		if (pdis->hwndItem == Modulelist.GetHwnd())
			Modulelist.DrawItem(wParam, lParam);
		break;

	case WM_LBUTTONDOWN:
		Clicked = TRUE;
		ClickP.x = LOWORD(lParam);
		ClickP.y = HIWORD(lParam);
		if (VolumeRect.PtInRect(Mouse))
		{
			Player.SetVolume(100 - VolumeRect.right + Mouse.x);
			InvalidateRect(VolumeRect, TRUE);
		}
		break;

	case WM_LBUTTONUP:
		Clicked = FALSE;
		Mouse.x = LOWORD(lParam);
		Mouse.y = HIWORD(lParam);
		if (VolumeRect.PtInRect(Mouse) && VolumeRect.PtInRect(ClickP))
		{
			Player.SetVolume(100 - VolumeRect.right + Mouse.x);
			InvalidateRect(VolumeRect, TRUE);
			break;
		}
		if (LinkRect.PtInRect(Mouse) && LinkRect.PtInRect(ClickP))
		{
			ShellExecuteW(NULL, L"open", Player.PlayingNow->Url.c_str(), NULL, NULL, SW_SHOWNORMAL);
			break;
		}

		if (PlayRect.PtInRect(Mouse) && !Clicked)
		{
			if (Player.status == eStatus::Playing)
				Player.Stop();
			else
			{
				if (Player.status == eStatus::Stopped&& Player.PlayingNow)
					Player.Resume();
			}
		}

		break;

	case WM_MOUSEMOVE:
		Mouse.x = LOWORD(lParam);
		Mouse.y = HIWORD(lParam);
		prv = Hover;
		if (VolumeRect.PtInRect(Mouse))
			Hover = bHover::Volume;
		else if (LinkRect.PtInRect(Mouse))
		{
			Hover = bHover::Link;
			if (Player.status == eStatus::Playing)
				SetCursor(LoadCursor(NULL, IDC_HAND));
		}
		else if (PlayRect.PtInRect(Mouse))
			Hover = bHover::Play;
		else
		{
			Hover = bHover::None;
			
		}
		if (Clicked&&VolumeRect.PtInRect(Mouse)&& VolumeRect.PtInRect(ClickP))
		{
			Player.SetVolume(100 - VolumeRect.right + Mouse.x);
			Player.UpdateWnd();
		}

		if (prv != Hover)
		{
			Player.UpdateWnd();
			if (Hover == bHover::None )
				SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
		
		break;

	case WM_MOUSEWHEEL:
		if (((short)HIWORD(wParam)) / 120 > 0 )
			Player.SetVolume(Player.GetVolume() + 5);
		if (((short)HIWORD(wParam)) / 120 < 0 )
			Player.SetVolume(Player.GetVolume() - 5);
		Hover = bHover::Volume;
		Player.UpdateWnd();
		break;

	case WM_TIMER:
		OnTimer((int)wParam);
	break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	}
	return WndProcDefault(uMsg, wParam, lParam);
}

LRESULT bTWin::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR *hdr;
	LPNMITEMACTIVATE lpnmia = (LPNMITEMACTIVATE)lParam;
	hdr = (NMHDR FAR*)lParam;
	switch (((LPNMHDR)lParam)->code)
	{
	case NM_RETURN:

	case NM_DBLCLK:
		if (hdr->hwndFrom == bList)
		{
			int id = bList.GetNextItem(-1, LVNI_SELECTED);
			if (id < 0)
				break;
			CString sID=bList.GetItemText(id, 1);
			int index = std::stoi(sID.c_str());
			if (index >= 0 && index < (int)Modulelist.GetActiveModule()->Playlist->Stations.size())
			{
				Player.OpenStation(Modulelist.GetActiveModule()->Playlist->Stations[index]);

				int old = bList.PlayingNowID;
				bList.PlayingNowID = lpnmia->iItem;
				bList.RedrawItems(lpnmia->iItem, lpnmia->iItem);
				bList.RedrawItems(old, old);
			}
		}
		break;
	case NM_CLICK:
		if (hdr->hwndFrom == Modulelist)
		{
			if (lpnmia->iItem > -1 && lpnmia->iItem < (int)Modulelist.Modules.size())
			{
				std::wstring s = Modulelist.GetItemText(lpnmia->iItem, 0).c_str();
				if (s.size())
				{
					if (s != Modulelist.ActiveModuleName)
					{
						Modulelist.SetActiveModule(s);
						bList.Playlist = Modulelist.GetActiveModule()->Playlist;
						bList.RedrawPlaylist();
					}
				}
			}
		}
		break;
	}
	return NULL;
}

BOOL bTWin::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (HIWORD(wParam))
	{
	case EN_SETFOCUS:
		if(searchbox.GetWindowText()== L"Search")
			searchbox.SetWindowTextW(L"");
		return TRUE;
	case EN_KILLFOCUS:
		if (searchbox.GetWindowTextLength()==0)
			searchbox.SetWindowTextW(L"Search");
		return TRUE;
	case EN_CHANGE:
		SetTimer(2, 1200, 0);
		return TRUE;
	}
	switch (LOWORD(wParam))
	{

	case ID_PLAYBACK_RESUME:
		Player.Resume();

		break;

	case ID_PLAYBACK_STOP:
		Player.Stop();
		break;

	case ID_HELP_LOGWINDOW:

		if (!Config.LogWindow)
		{
			GetMenu().CheckMenuItem(ID_HELP_LOGWINDOW, MF_CHECKED);
			Config.LogWindow = true;
			Config.Save();
			if (!bLog::_bLogWin)
			{
				bLog::_bLogWin = new bLogWin;
				bLog::_bLogWin->Create();
				bLog::_bLogWin->MoveWindow(GetWindowRect().right, GetWindowRect().top, 500, GetWindowRect().Height());
				if (!bLog::_bLogWin->GetHwnd())
					bLog::AddLog(bLogEntry(L"Failed to create Log window", L"bTuner App", eLogType::Error));
			}
			SetFocus();
		}
		else
		{
			GetMenu().CheckMenuItem(ID_HELP_LOGWINDOW, MF_UNCHECKED);
			Config.LogWindow = false;
			Config.Save();
			if (bLog::_bLogWin)
			{
				bLog::_bLogWin->Destroy();
				delete bLog::_bLogWin;
				bLog::_bLogWin = NULL;
			}
		}
		break;

	case ID_FILE_EXIT:
		PostQuitMessage(0);
		break;

	case ID_FILE_OPEN_FILE:
		break;
	case ID_FILE_OPEN_URL:
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_OPENURL), GetHwnd(), (DLGPROC)&g_Diagproc);
		break;
	case ID_HELP_ABOUT:
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_ABOUT), GetHwnd(), (DLGPROC)&g_Diagproc);
		break;
	case ID_FAVORITES_RELOADFAVORITES:
			Modulelist.Modules[L"Favorites"]->UpdateStations();
			if (Modulelist.ActiveModuleName == L"Favorites")
				bList.RedrawPlaylist();
		break;
	case ID_FAVORITES_ADDTOFAVORITES:
		DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADDTOFAVORITES), GetHwnd(), (DLGPROC)&g_Diagproc, (LPARAM)Player.PlayingNow);
		break;
	case ID_FAVORITES_QUICKADDTOFAVORITES:
	{
		Modulelist.Modules[L"Favorites"]->Playlist->Stations.push_back(*Player.PlayingNow);
		if (!Modulelist.Modules[L"Favorites"]->Playlist->SaveXSPF(L"bFavorites.xspf"))
			bLog::AddLog(bLogEntry(L"Error Saving Favorites File [bFavorites.xspf]", L"bTWin", eLogType::Error));
		Modulelist.Modules[L"Favorites"]->UpdateStations();
		if (Modulelist.ActiveModuleName == L"Favorites")
			bList.RedrawPlaylist();
		break;
	}

	}

	return TRUE;
}

INT_PTR bTWin::Diagproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m) {
	case WM_INITDIALOG:
	{
		RECT rcOwner, rcDlg, rc;
		HFONT font;
		std::wstring version;
		::GetWindowRect(*this, &rcOwner);
		::GetWindowRect(h, &rcDlg);
		CopyRect(&rc, &rcOwner);
		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

		::SetWindowPos(h, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE);
		if (l)
		{
			bStation *st = (bStation*)l;
			::SetDlgItemText(h, IDC_TITLE, st->Name.c_str());
			::SetDlgItemText(h, IDC_IMAGE, st->Image.c_str());
			::SetDlgItemText(h, IDC_WEBSITE, st->Url.c_str());
			std::wstring streams;
			for (unsigned int i = 0; i < st->Streams.size(); i++)
				streams += st->Streams.at(i).Url + L"\n";
			::SetDlgItemText(h, IDC_SOURCES, streams.c_str());
			break;
		}
		if (OpenClipboard(*this))
		{
			HANDLE hData = GetClipboardData(CF_TEXT);
			char * buffer = (char*)GlobalLock(hData);
			GlobalUnlock(hData);
			CloseClipboard();
			if (!buffer)
				break;
			if (!strnicmp(buffer, "http://", 7))
				::SetDlgItemTextA(h, IDC_LINK, buffer);
		}
		else
			bLog::AddLog(bLogEntry(L"Failed to Open Clipboard", L"bTuner Win", eLogType::Error));

		font = ::CreateFont(40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
		::SendDlgItemMessageW(h, IDC_APP_NAME, WM_SETFONT, WPARAM(font), TRUE);
		version = L"Version : ";
		version += TEXT(VERSION_TEXT);
		::SetDlgItemText(h, IDC_VERSION, version.c_str());
		break;
	}
	case WM_CLOSE:
		EndDialog(h, NULL);
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)l)->code)
		{
		  case NM_CLICK:
			  if (((LPNMHDR)l)->idFrom == IDC_SYSLINK2|| ((LPNMHDR)l)->idFrom == IDC_SYSLINK3)
			  {
				  PNMLINK pNMLink = (PNMLINK)((LPNMHDR)l);
				  ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
			  }
		}
		break;

	case WM_COMMAND:
		if (LOWORD(w) == IDCANCEL)
			EndDialog(h, NULL);
		if (LOWORD(w) == IDOK)
		{
			wchar_t *u = new wchar_t[256];
			::GetDlgItemText(h, IDC_LINK, u, 256);
				Player.OpenURL(u);
				EndDialog(h, NULL);
		}
		if (LOWORD(w) == ID_ADD)
		{
			bStation st;
			wchar_t *buf = new wchar_t[2048];
			::GetDlgItemText(h, IDC_TITLE, buf, 2048);
			st.Name = buf;
			::GetDlgItemText(h, IDC_IMAGE, buf, 2048);
			st.Image = buf;
			::GetDlgItemText(h, IDC_WEBSITE, buf, 2048);
			st.Url = buf;
			memset(buf, 0, 2048);
			::GetDlgItemText(h, IDC_SOURCES, buf, 2048);
			vector<wstring> sources = bString::Explode(wstring(buf), L"\n");
			for (unsigned int i = 0;i<sources.size(); i++)
			{
				if(sources[i].size())
					st.Streams.push_back(bStream(sources[i]));
			}
			if (!st.Name.size() || !st.Streams.size())
				return 0;
			Modulelist.Modules[L"Favorites"]->Playlist->Stations.push_back(st);
			if (!Modulelist.Modules[L"Favorites"]->Playlist->SaveXSPF(L"bFavorites.xspf"))
				bLog::AddLog(bLogEntry(L"Error Saving Favorites File [bFavorites.xspf]", L"bTWin", eLogType::Error));
			Modulelist.Modules[L"Favorites"]->UpdateStations();
			if (Modulelist.ActiveModuleName == L"Favorites")
				bList.RedrawPlaylist();
			EndDialog(h, NULL);
		}
			
		break;
	}
	return 0;
}

void  bTWin::DrawPlayer(HDC dc)
{

	CRect cr=GetClientRect();
	RECT r;
	HFONT font;
	HBRUSH brush;

	//Player rect

	brush = CreateSolidBrush(RGB(20, 20, 20));
	SetRect(&r, 0, cr.bottom - 150, cr.right, cr.bottom);
	FillRect(dc, &r, brush);
	DeleteObject(brush);

	//Control rect

	SetRect(&r, 155, cr.bottom - 75, cr.right - 5, cr.bottom - 25);
	brush = CreateSolidBrush(RGB(10, 10, 10));
	FillRect(dc, &r, brush);
	DeleteObject(brush);

	//Cover art rect
	SetRect(&r, 3, cr.bottom - 148, 148, cr.bottom-2);
	brush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(dc, &r, brush);
	DeleteObject(brush);
	SetRect(&r, 0, cr.bottom - 150, 150, cr.bottom);
	brush = CreateSolidBrush(RGB(10, 10, 10));
	FillRect(dc, &r, brush);
	DeleteObject(brush);

	if (Player.status == eStatus::Playing &&(Player.ImageLoaded || Player.CoverLoaded))
	{
		Graphics *graphics=new Graphics(dc);
		Image *image;
		if(Player.CoverLoaded)
			image = new Image(L"Cover.png");
		else if(Player.ImageLoaded)
			image = new Image(L"Station.png");
		graphics->DrawImage(image, 5, cr.bottom - 145, 140, 140);
		delete image;
		delete graphics;
	}
	else
	{
		SetRect(&r, 5, cr.bottom - 145, 145, cr.bottom - 5);
		HBRUSH hBrush = ::CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 0));
		FillRect(dc, &r, hBrush);
		DeleteObject(hBrush);
	}


	//Play Stop Button

	brush=CreateSolidBrush(RGB(45, 45, 45));
	if (Hover == bHover::Play)
		brush=CreateSolidBrush(RGB(62, 62, 64));
	SelectObject(dc, brush);
	SetRect(&r, 160, cr.bottom - 70, 250, cr.bottom - 30);
	RoundRect(dc, r.left,r.top,r.right,r.bottom, 8, 8);
	DeleteObject(brush);

	brush=CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(dc, brush);
	SetRect(&r, 163, cr.bottom - 67, 247, cr.bottom - 33);
	RoundRect(dc, r.left, r.top, r.right, r.bottom, 8, 8);
	DeleteObject(brush);

	brush=CreateSolidBrush(RGB(62, 62, 64));
	SelectObject(dc, brush);
	SetRect(&r, 165, cr.bottom - 65, 245, cr.bottom - 35);
	RoundRect(dc, r.left, r.top, r.right, r.bottom, 5, 5);
	DeleteObject(brush);


	font=CreateFont(25, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
	SelectObject(dc,font);
	SetTextColor(dc, RGB(255, 255, 255));
	SetBkMode(dc, TRANSPARENT);
	if (Player.status == eStatus::Stopped)
	{
		TextOut(dc, 195, cr.bottom - 65, L"Play", 4);
		DeleteObject(font);
		
		HPEN pen;
		if (Hover == bHover::Play)
			pen= ::CreatePen(PS_SOLID, 1, RGB(51, 153, 255));
		else
			pen = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		
		if (Hover == bHover::Play)
			brush = CreateSolidBrush(RGB(51, 153, 255));
		else
			brush = CreateSolidBrush(RGB(255, 255, 255));
		
		SelectObject(dc, brush);
		SelectObject(dc, pen);
		POINT points[3];
		points[0].x = 172;
		points[0].y = cr.bottom - 58;
		points[1].x = 185;
		points[1].y = cr.bottom - 51;
		points[2].x = 172;
		points[2].y = cr.bottom - 45;

		Polygon(dc, points, 3);
		DeleteObject(brush);
		DeleteObject(pen);
	}

	if (Player.status != eStatus::Stopped)
	{
		TextOut(dc, 195, cr.bottom - 65, L"Stop", 4);
		
		if (Hover == bHover::Play)
			brush = CreateSolidBrush(RGB(51, 153, 255));
		else
			brush = CreateSolidBrush(RGB(255, 255, 255));

		SelectObject(dc, brush);
		SetRect(&r, 172, cr.bottom - 58, 185, cr.bottom - 45);
		FillRect(dc, &r, brush);

		DeleteObject(brush);
	}

	// Volume 
	int v = Player.GetVolume();
	if (Clicked&&VolumeRect.PtInRect(Mouse))
		v = 100 - VolumeRect.right + Mouse.x;

	SetRect(&r, cr.right - 120, cr.bottom - 54, cr.right - 20, cr.bottom - 46);
	
	brush = CreateSolidBrush(RGB(100, 100, 100));
	FillRect(dc,&r, brush);

	SetRect(&r, cr.right - 120, cr.bottom - 57, cr.right - 20 - (100 - v), cr.bottom - 43);
	DeleteObject(brush);
	brush = CreateSolidBrush(RGB(200, 200, 200));
	FillRect(dc,&r, brush);

	if (Hover == bHover::Volume)
	{
		SetTextColor(dc, RGB(51, 153, 255));
		SetBkMode(dc, TRANSPARENT);
		font = CreateFont(14, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		SelectObject(dc, font);

		CString bf;
		bf.Format(L"Volume: %u%%", v);
		SIZE sv;
		GetTextExtentPoint32W(dc, bf, bf.GetLength(), &sv);

		SetRect(&r, cr.right - 70 - (sv.cx / 2) - 4, cr.bottom - 54 - 24, cr.right - 70 + (sv.cx / 2) + 4, cr.bottom - 54 - 22 + sv.cy);
		DeleteObject(brush);
		brush = CreateSolidBrush(RGB(45, 45, 48));
		FillRect(dc, &r, brush);

		SetRect(&r, cr.right - 70 - (sv.cx / 2)-3, cr.bottom - 54-23, cr.right - 70 + (sv.cx / 2) + 3, cr.bottom - 54 - 23+sv.cy);
		DeleteObject(brush);
		brush = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(dc, &r, brush);



		TextOut(dc, cr.right - 70 - (sv.cx / 2), cr.bottom - 57 - 20, bf, bf.GetLength());
		DeleteObject(font);
	}

	

	//Playing Now info

	SetTextColor(dc, RGB(255, 255, 255));
	SetBkColor(dc, RGB(45, 45, 48));
	SetBkMode(dc, TRANSPARENT);

	font=CreateFont(45, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	SelectObject(dc, font);
	if (Player.status != eStatus::Playing)
		SetTextColor(dc, RGB(127, 127, 127));
	if (Player.PlayingNow && Player.PlayingNow->Name.size())
		TextOut(dc, 155, cr.bottom - 145, Player.PlayingNow->Name.c_str(), (int)Player.PlayingNow->Name.size());
	DeleteObject(font);
	font=CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	SelectObject(dc, font);
	SetTextColor(dc, RGB(180, 180, 180));
	if (Player.status == eStatus::Buffering)
	{
		CString buf;
		buf.Format(L"[%u %s] Buffering", v,L"%");
		TextOut(dc, 155, cr.bottom - 20, buf.c_str(),buf.GetLength());
	}
	if (Player.status == eStatus::Connecting)
		TextOut(dc, 155, cr.bottom - 20, L"Connecting...", 13);
	if (Player.status == eStatus::Stopped)
		TextOut(dc, 155, cr.bottom - 20, L"Not Connected", 13);
	DeleteObject(font);

	if (Player.status == eStatus::Playing)
	{
		font=CreateFont(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
		SelectObject(dc, font);
		SetTextColor(dc, RGB(200, 200, 200));
		TextOut(dc, 155, cr.bottom - 105, Player.PlayingNow->Playing.c_str(), (int)Player.PlayingNow->Playing.size());
		DeleteObject(font);
		font=CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		SelectObject(dc, font);
		SetTextColor(dc, RGB(180, 180, 180));
		

		SIZE s;
		GetTextExtentPoint32(dc, Player.PlayingNow->Url.c_str(), (int)Player.PlayingNow->Url.size(),&s);
		LinkRect.top = GetClientRect().bottom - 20;
		LinkRect.bottom = GetClientRect().bottom;
		LinkRect.left = 155;
		LinkRect.right = 155 + s.cx;
		if (Hover == bHover::Link)
			SetTextColor(dc, RGB(40, 185, 220));
		TextOut(dc, 155, cr.bottom - 20, Player.PlayingNow->Url.c_str(), (int)Player.PlayingNow->Url.size());
		DeleteObject(font);
		font=CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Open Sans"));
		SelectObject(dc, font);
		SetTextColor(dc, RGB(0, 0, 0));
		SetBkColor(dc, RGB(245, 245, 245));
		SetBkMode(dc, OPAQUE);

		wchar_t *codecT[] = { L" MP3 ",L" AAC ",L" OGG ",L" MPEG ",L" AAC+ ",L" Opus ",L" NSV ",L"" };
		int c = Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding;
		if (c < eCodecs::UNDIFINED)
			TextOut(dc, 155 + 10 + s.cx, cr.bottom - 20, codecT[c], (int)wcslen(codecT[c]));
		SIZE s2;
		CString bf;
		if (Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate)
		{
			GetTextExtentPoint32(dc, codecT[c], (int)wcslen(codecT[c]),&s2);
			
			bf.Format(L" %u Kbps " , Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate);
			TextOut(dc, 155 + 10 + s.cx + 5 + s2.cx, cr.bottom - 20, bf.c_str(), bf.GetLength());
		}

		SIZE s3;
		GetTextExtentPoint32(dc, bf.c_str(), (int)bf.GetLength(), &s3);
		CString sf;
		sf.Format(L" Stream %u/%u ", Player.PlayingNow->PlayedStreamID+1, Player.PlayingNow->Streams.size());
		DeleteObject(font);
		TextOut(dc, 155+10+s.cx+5+s3.cx+s2.cx+5, cr.bottom - 20, sf.c_str(), sf.GetLength());
		
	}

	//Time
	font = CreateFont(25, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
	SelectObject(dc, font);
	SetTextColor(dc, RGB(220, 220, 220));
	SetBkMode(dc, TRANSPARENT);
	time_t Time;
	time(&Time);
	tm  nowinfo;
	localtime_s(&nowinfo, &Time);
	if (Player.StationTime&&Player.TrackTime&&Player.status == eStatus::Playing)
	{

		time_t _StationTime = static_cast<time_t>(difftime (Time , *Player.StationTime));
		time_t _TrackTime = static_cast<time_t>(difftime(Time, *Player.TrackTime));
		tm  _StationTimeinfo;
		localtime_s(&_StationTimeinfo,&_StationTime);
		tm  _TrackTimeinfo;
		localtime_s(&_TrackTimeinfo,&_TrackTime);
		tm  _hinfo;
		time_t h_t = static_cast<time_t>(difftime(Time, Time));;
		localtime_s(&_hinfo, &h_t);
		CString tms;
		unsigned int h = _hinfo.tm_hour;
		if (_StationTimeinfo.tm_hour-h)
		{
			if (_TrackTimeinfo.tm_hour-h)
				tms.Format(L"%u:%02u:%02u I %u:%02u:%02u", _TrackTimeinfo.tm_hour-h, _TrackTimeinfo.tm_min, _TrackTimeinfo.tm_sec, _StationTimeinfo.tm_hour-h, _StationTimeinfo.tm_min, _StationTimeinfo.tm_sec);
			else
				tms.Format(L"%02u:%02u I %u:%02u:%02u", _TrackTimeinfo.tm_min, _TrackTimeinfo.tm_sec, _StationTimeinfo.tm_hour-h, _StationTimeinfo.tm_min, _StationTimeinfo.tm_sec);
		}
		else
			tms.Format(L"%02u:%02u I %02u:%02u", _TrackTimeinfo.tm_min, _TrackTimeinfo.tm_sec, _StationTimeinfo.tm_min, _StationTimeinfo.tm_sec);

		SIZE s4;
		GetTextExtentPoint32(dc, tms.c_str(), tms.GetLength(), &s4);
		TextOut(dc, cr.right - 150 - s4.cx, cr.bottom - 63, tms.c_str(), tms.GetLength());
	}
	else
	{
		SIZE s4;
		GetTextExtentPoint32(dc, L"00:00 I 00:00",13, &s4);
		TextOut(dc, cr.right - 150 - s4.cx, cr.bottom - 63, L"00:00 I 00:00", 13);
	}

	DeleteObject(font);

	// Clock
	font = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
	SelectObject(dc, font);
	SetTextColor(dc, RGB(180, 180, 180));
	SetBkMode(dc, TRANSPARENT);
	
	CString tms;
	tms.Format(L"%02u:%02u:%02u", nowinfo.tm_hour, nowinfo.tm_min, nowinfo.tm_sec);

	SIZE s3;
	GetTextExtentPoint32(dc, tms.c_str(), tms.GetLength(), &s3);
	TextOut(dc, cr.right - 10 - s3.cx, cr.bottom - 20, tms.c_str(), tms.GetLength());
	DeleteObject(font);
}