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

bTWin::bTWin(): Clicked(FALSE), Playlist(NULL), Hover(bHover::None)
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
			std::vector<unsigned int> result = Playlist->Search(q);
			if (result.size())
				bList.DrawOnly(result);
			else
				bList.DeleteAllItems();
		}
		else if (Playlist&&searchbox.GetWindowTextLength() == 0 && searchbox.GetWindowText() != L"Search")
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
		int index = std::stoi(sID.c_str());
		if (index >= 0 && index < (int)Playlist->Stations.size())
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
			for (unsigned int i = 0; i < Playlist->Stations[index].Streams.size(); i++)
			{
				CString s;
				s.Format(L"[%02u] ", i + 1);
				s += Playlist->Stations[index].Streams[i].Url.c_str();
				Streammenu.AppendMenuW(MF_STRING, NULL, s.c_str());
			}

			mi.dwMenuData = 1;
			menu.SetMenuInfo(mi);
			menu.InsertMenu(0, MF_STRING, NULL, L"Play Station");
			menu.InsertMenu(1, MF_SEPARATOR);
			menu.InsertMenu(2, MF_POPUP, (UINT_PTR)Streammenu.GetHandle(), L"Streams");
			menu.InsertMenu(3, MF_SEPARATOR);
			
			if (Playlist->title.find(L"bFavorites") != std::wstring::npos)
			{
				menu.InsertMenu(4, MF_STRING | MF_DISABLED | MF_GRAYED, NULL, L"Add to favorites");
				menu.InsertMenu(5, MF_STRING | MF_DISABLED | MF_GRAYED, NULL, L"Quick add to favorites");
				menu.InsertMenu(6, MF_STRING, NULL, L"Remove from favorites");
			}
			else
			{
				menu.InsertMenu(4, MF_STRING, NULL, L"Add to favorites");
				menu.InsertMenu(5, MF_STRING, NULL, L"Quick add to favorites");
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
			unsigned int index = std::stoi(sID.c_str());
			if (index >= 0 && index < (int)Playlist->Stations.size())
				Player.OpenStation(Playlist->Stations.at(index));
		}
		if (idx == 5)
		{
			int id = bList.GetNextItem(-1, LVNI_SELECTED);
			if (id < 0)
				return;
			CString sID = bList.GetItemText(id, 1);
			unsigned int index = std::stoi(sID.c_str());
			if (index >= 0 && index < (int)Playlist->Stations.size())
			{
				bPlaylist fav;
				if (!fav.LoadFile(L"bFavorites.xspf"))
					bLog::AddLog(bLogEntry(L"Error Loading Favorites File [bFavorites.xspf]", L"bTWin", eLogType::Error));
				fav.Stations.push_back(Playlist->Stations.at(index));
				if (!fav.SaveXSPF(L"bFavorites.xspf"))
					bLog::AddLog(bLogEntry(L"Error Saving Favorites File [bFavorites.xspf]", L"bTWin", eLogType::Error));
				if (Playlist->title.find(L"bFavorites") != std::wstring::npos)
					bList.AddStation(Playlist->Stations.at(index));
			}
		}
		if (idx == 8)
		{
			int id = bList.GetNextItem(-1, LVNI_SELECTED);
			if (id < 0)
				return;
			CString sID = bList.GetItemText(id, 1);
			unsigned int index = std::stoi(sID.c_str());
			if (index >= 0 && index < (int)Playlist->Stations.size())
			{
				ShellExecuteW(NULL, L"open", Playlist->Stations.at(index).Url.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
		}

	}
	else if (StreamMenu == menu)
	{
		int id = bList.GetNextItem(-1, LVNI_SELECTED);
		if (id < 0)
			return;
		CString sID = bList.GetItemText(id, 1);
		unsigned int index = std::stoi(sID.c_str());
		if (index >= 0 && index < (int)Playlist->Stations.size())
		{
			Playlist->Stations.at(index).PlayedStreamID = idx;
			Player.OpenStation(Playlist->Stations.at(index));
		}
	}
}

void bTWin::OnDestroy()
{
	if (Playlist)
		delete Playlist;
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
	wc.hbrBackground=(HBRUSH) CreateSolidBrush(RGB(30,30,30));
	wc.lpszClassName=L"bTuner";
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
}

void bTWin::PreCreate(CREATESTRUCT &cs)
{
	cs.dwExStyle = WS_EX_ACCEPTFILES | WS_EX_TOPMOST;
	cs.lpszClass=L"bTuner";
	cs.lpszName = L".:: bTuner ::.";
	cs.style = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE ;
	cs.cx=700;
	cs.cy=550;
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
	MoveWindow(Config.LastWindowPos.x,Config.LastWindowPos.y,700,550);
	if (Player.PlayingNow)
		delete Player.PlayingNow;
	Player.PlayingNow = new bStation;

	Player.PlayingNow->Streams.push_back(bStream(Config.LastPlayed.Streams.at(0)));
	Player.PlayingNow->Name = Config.LastPlayed.Name;
	Player.PlayingNow->Url = Config.LastPlayed.Url;
	Player.PlayingNow->Image = Config.LastPlayed.Image;

	bList.Create(*this);
	bList.OnCreate();


	
	searchbox.Create(*this);
	searchbox.MoveWindow(GetClientRect().right-205,5,200,20);
	HFONT font;
	font = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
	searchbox.SetFont(font);
	searchbox.SetWindowTextW(L"Search");
	searchbox.SetMargins(10, 10);


#define TESTSWITCH

#ifdef TESTSWITCH
	
	Playlist = new bPlaylist;
	Displayed_Playlist = Playlist;
	Playlist->LoadFile(L"bFavorites.xspf");

#else
	
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
	
	bList.Playlist = Playlist;
	bList.RedrawPlaylist();
	
	UpdateWindow();
	SetTimer(1, 1000, NULL);

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

void bTWin::OnDraw(CDC& dc)
{
	HDC hdcBuffer = CreateCompatibleDC(::GetDC(GetHwnd()));
	int iOldState = SaveDC(hdcBuffer);
	int h, w;
	h = GetClientRect().Height();
	w = GetClientRect().Width();
	HBITMAP hBitmapBuffer = CreateCompatibleBitmap(::GetDC(GetHwnd()), w, h);
	SelectObject(hdcBuffer, hBitmapBuffer); 
	
	HFONT font = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	SelectObject(hdcBuffer,font);
	SetTextColor(hdcBuffer,RGB(200, 200, 200));
	SetBkMode(hdcBuffer,TRANSPARENT);
	TextOut(hdcBuffer,160, 5, Playlist->title.c_str(), (int)Playlist->title.size());
	DeleteObject(font);
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
		return (LRESULT)CreateSolidBrush(RGB(200,200,200));
	case WM_MEASUREITEM:
		pmis = (PMEASUREITEMSTRUCT)lParam;
		pmis->itemHeight = 30;
		break;

	case WM_DRAWITEM:

		pdis = (PDRAWITEMSTRUCT)lParam;
		if (pdis->CtlType == ODT_LISTVIEW)
			bList.DrawItem(wParam, lParam);
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
			InvalidateRect(VolumeRect, TRUE);
		}

		if (prv != Hover)
		{
			if (Hover == bHover::Play|| prv == bHover::Play)
				InvalidateRect(PlayRect, TRUE);
			if (Hover == bHover::Link || prv == bHover::Link)
				InvalidateRect(LinkRect, TRUE);
			if (Hover == bHover::None )
				SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
		
		break;

	case WM_MOUSEWHEEL:
		if (((short)HIWORD(wParam)) / 120 > 0 )
			Player.SetVolume(Player.GetVolume() + 5);
		if (((short)HIWORD(wParam)) / 120 < 0 )
			Player.SetVolume(Player.GetVolume() - 5);
		InvalidateRect(VolumeRect, TRUE);
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
	switch (((LPNMHDR)lParam)->code)
	{
	case LVN_ITEMACTIVATE:
		hdr = (NMHDR FAR*)lParam;
		if (hdr->hwndFrom == bList)
		{
			CString sID=bList.GetItemText(lpnmia->iItem, 1);
			int index = std::stoi(sID.c_str());
			if (index >= 0 && index <(int)Playlist->Stations.size())
				Player.OpenStation(Playlist->Stations[index]);

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
				bLog::_bLogWin->MoveWindow(GetWindowRect().right, GetWindowRect().top, 500, 500);
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
		if (Playlist)
			delete Playlist;
		Playlist = new bPlaylist;
		if (Playlist->LoadFile(L"bFavorites.xspf"))
		{
			Displayed_Playlist = Playlist;
			bList.Playlist = Displayed_Playlist;
			bList.RedrawPlaylist();
		}
		else
			bLog::AddLog(bLogEntry(L"Error Loading Favorites File [bFavorites.xspf]", L"bTWin", eLogType::Error));
		RedrawWindow();
		break;
	case ID_FAVORITES_ADDTOFAVORITES:

		break;
	case ID_FAVORITES_QUICKADDTOFAVORITES:
	{
		bPlaylist fav;
		if (!fav.LoadFile(L"bFavorites.xspf"))
			bLog::AddLog(bLogEntry(L"Error Loading Favorites File [bFavorites.xspf]", L"bTWin", eLogType::Error));
		fav.title = L"bFavorites";
		fav.Stations.push_back(*Player.PlayingNow);
		if (!fav.SaveXSPF(L"bFavorites.xspf"))
			bLog::AddLog(bLogEntry(L"Error Saving Favorites File [bFavorites.xspf]", L"bTWin", eLogType::Error));
		if (Playlist->title.find(L"bFavorites") != std::wstring::npos)
			bList.AddStation(*Player.PlayingNow);
		break;
	}

	}

	return TRUE;
}

INT_PTR bTWin::Diagproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	RECT rcOwner, rcDlg,rc;
	HFONT font;
	std::wstring version;
	switch (m) {
	case WM_INITDIALOG:

		::GetWindowRect(*this,&rcOwner);
		::GetWindowRect(h, &rcDlg);
		CopyRect(&rc, &rcOwner);
		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

		::SetWindowPos(h,HWND_TOP,rcOwner.left + (rc.right / 2),rcOwner.top + (rc.bottom / 2),0, 0,SWP_NOSIZE);

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
		}else
				bLog::AddLog(bLogEntry(L"Failed to Open Clipboard", L"bTuner Win", eLogType::Error));

		font=::CreateFont(40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
		::SendDlgItemMessageW(h, IDC_APP_NAME, WM_SETFONT, WPARAM(font), TRUE);
		version = L"Version : ";
		version += TEXT(VERSION_TEXT);
		::SetDlgItemText(h, IDC_VERSION, version.c_str());
		break;
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

	brush = CreateSolidBrush(RGB(0, 0, 0));
	SetRect(&r, 0, cr.bottom - 150, cr.right, cr.bottom);
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
		HPEN pen = CreatePen(PS_SOLID,3,RGB(200,200,200));
		SelectObject(dc, pen);
		FillRect(dc, &r, hBrush);
		DeleteObject(hBrush);
		DeleteObject(pen);
	}

	SetRect(&r, 150, cr.bottom - 75, cr.right - 5, cr.bottom - 25);
	brush = CreateSolidBrush(RGB(20, 20, 20));
	FillRect(dc, &r, brush);
	DeleteObject(brush);

	brush=CreateSolidBrush(RGB(200, 200, 200));
	if (Hover == bHover::Play)
		brush=CreateSolidBrush(RGB(255, 255, 255));
	SelectObject(dc, brush);
	SetRect(&r, 160, cr.bottom - 70, 250, cr.bottom - 30);
	RoundRect(dc, r.left,r.top,r.right,r.bottom, 8, 8);

	DeleteObject(brush);
	brush=CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(dc, brush);
	SetRect(&r, 163, cr.bottom - 67, 247, cr.bottom - 33);
	RoundRect(dc, r.left, r.top, r.right, r.bottom, 8, 8);

	DeleteObject(brush);
	brush=CreateSolidBrush(RGB(200, 200, 200));
	SelectObject(dc, brush);
	SetRect(&r, 165, cr.bottom - 65, 245, cr.bottom - 35);
	RoundRect(dc, r.left, r.top, r.right, r.bottom, 5, 5);



	font=CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	SelectObject(dc,font);
	SetTextColor(dc, RGB(0, 0, 0));
	SetBkMode(dc, TRANSPARENT);
	if (Player.status == eStatus::Stopped)
	{
		TextOut(dc, 195, cr.bottom - 65, L"Play", 4);
		DeleteObject(font);
		DeleteObject(brush);
		brush=CreateSolidBrush(RGB(0, 0, 0));
		SelectObject(dc, brush);
		POINT points[3];
		points[0].x = 172;
		points[0].y = cr.bottom - 58;
		points[1].x = 185;
		points[1].y = cr.bottom - 50;
		points[2].x = 172;
		points[2].y = cr.bottom - 42;

		Polygon(dc, points, 3);
	}

	if (Player.status != eStatus::Stopped)
	{
		TextOut(dc, 195, cr.bottom - 65, L"Stop", 4);
		DeleteObject(brush);
		brush=CreateSolidBrush(RGB(0, 0, 0));

		SetRect(&r, 172, cr.bottom - 58, 185, cr.bottom - 42);
		FillRect(dc, &r, brush);
	}

	// Volume 
	int v = Player.GetVolume();
	if (Clicked&&VolumeRect.PtInRect(Mouse))
		v = 100 - VolumeRect.right + Mouse.x;

	SetRect(&r, cr.right - 120, cr.bottom - 54, cr.right - 20, cr.bottom - 46);
	DeleteObject(brush);
	brush = CreateSolidBrush(RGB(100, 100, 100));
	
	FillRect(dc,&r, brush);

	SetRect(&r, cr.right - 120, cr.bottom - 57, cr.right - 20 - (100 - v), cr.bottom - 43);
	DeleteObject(brush);
	brush = CreateSolidBrush(RGB(200, 200, 200));
	FillRect(dc,&r, brush);

	SetTextColor(dc,RGB(0, 0, 0));
	SetBkMode(dc, TRANSPARENT);
	font=CreateFont(14, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	SelectObject(dc,font);

	CString bf;
	bf.Format(L"%u", v);
	SIZE sv;
	GetTextExtentPoint32W(dc, bf, bf.GetLength(),&sv);
	TextOut(dc, cr.right - 70 - (sv.cx / 2), cr.bottom - 57, bf, bf.GetLength());
	DeleteObject(font);


	//Playing Now info

	SetTextColor(dc, RGB(245, 245, 245));
	SetBkColor(dc, RGB(0, 0, 0));

	font=CreateFont(45, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	SelectObject(dc, font);
	if (Player.status != eStatus::Playing)
		SetTextColor(dc, RGB(150, 150, 150));
	if (Player.PlayingNow && Player.PlayingNow->Name.size())
		TextOut(dc, 150, cr.bottom - 145, Player.PlayingNow->Name.c_str(), (int)Player.PlayingNow->Name.size());
	DeleteObject(font);
	font=CreateFont(16, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
	SelectObject(dc, font);
	SetTextColor(dc, RGB(180, 180, 180));
	if (Player.status == eStatus::Buffering)
	{
		CString buf;
		buf.Format(L"[%u %s] Buffering", v,L"%");
		TextOut(dc, 150, cr.bottom - 20, buf.c_str(),buf.GetLength());
	}
	if (Player.status == eStatus::Connecting)
		TextOut(dc, 150, cr.bottom - 20, L"Connecting...", 13);
	if (Player.status == eStatus::Stopped)
		TextOut(dc, 150, cr.bottom - 20, L"Not Connected", 13);
	DeleteObject(font);

	if (Player.status == eStatus::Playing)
	{
		font=CreateFont(25, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
		SelectObject(dc, font);
		SetTextColor(dc, RGB(200, 200, 200));
		TextOut(dc, 150, cr.bottom - 105, Player.PlayingNow->Playing.c_str(), (int)Player.PlayingNow->Playing.size());
		DeleteObject(font);
		font=CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
		SelectObject(dc, font);
		SetTextColor(dc, RGB(180, 180, 180));
		

		SIZE s;
		GetTextExtentPoint32(dc, Player.PlayingNow->Url.c_str(), (int)Player.PlayingNow->Url.size(),&s);
		LinkRect.top = GetClientRect().bottom - 20;
		LinkRect.bottom = GetClientRect().bottom;
		LinkRect.left = 150;
		LinkRect.right = 150 + s.cx;
		if (Hover == bHover::Link)
			SetTextColor(dc, RGB(40, 185, 220));
		TextOut(dc, 150, cr.bottom - 20, Player.PlayingNow->Url.c_str(), (int)Player.PlayingNow->Url.size());
		DeleteObject(font);
		font=CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		SelectObject(dc, font);
		SetTextColor(dc, RGB(0, 0, 0));
		SetBkColor(dc, RGB(245, 245, 245));
		SetBkMode(dc, OPAQUE);

		wchar_t *codecT[] = { L" MP3 ",L" AAC ",L" OGG ",L" MPEG ",L" AAC+ ",L" Opus ",L" NSV ",L"" };
		int c = Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding;
		if (c < eCodecs::UNDIFINED)
			TextOut(dc, 150 + 10 + s.cx, cr.bottom - 20, codecT[c], (int)wcslen(codecT[c]));

		if (Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate)
		{
			SIZE s2;
			GetTextExtentPoint32(dc, codecT[c], (int)wcslen(codecT[c]),&s2);
			CString bf;
			bf.Format(L" %u Kbps " , Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate);
			TextOut(dc, 150 + 10 + s.cx + 5 + s2.cx, cr.bottom - 20, bf.c_str(), bf.GetLength());
		}
		DeleteObject(font);

		
	}
	font = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
	SelectObject(dc, font);
	SetTextColor(dc, RGB(220, 220, 220));
	SetBkMode(dc, TRANSPARENT);
	time_t Time;
	time(&Time);
	
	if (Player.StationTime&&Player.status == eStatus::Playing)
	{

		time_t Time2 = Time - *Player.StationTime;
		struct tm*  timeinfo2;
		timeinfo2 = localtime(&Time2);
		CString tms;
		tms.Format(L"%02d:%02d", timeinfo2->tm_min, timeinfo2->tm_sec);

		SIZE s4;
		GetTextExtentPoint32(dc, tms.c_str(), tms.GetLength(), &s4);
		TextOut(dc, cr.right - 150 - s4.cx, cr.bottom - 65, tms.c_str(), tms.GetLength());
	}
	else
	{
		SIZE s4;
		GetTextExtentPoint32(dc, L"00:00",5, &s4);
		TextOut(dc, cr.right - 150 - s4.cx, cr.bottom - 65, L"00:00", 5);
	}
	font = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
	SelectObject(dc, font);
	SetTextColor(dc, RGB(180, 180, 180));
	SetBkMode(dc, TRANSPARENT);
	struct tm*  timeinfo;
	timeinfo = localtime(&Time);
	CString tms;
	tms.Format(L"%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	SIZE s3;
	GetTextExtentPoint32(dc, tms.c_str(), tms.GetLength(), &s3);
	TextOut(dc, cr.right - 10 - s3.cx, cr.bottom - 20, tms.c_str(), tms.GetLength());

}