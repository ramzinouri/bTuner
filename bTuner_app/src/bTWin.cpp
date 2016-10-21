#include "bTWin.h"
#include "bLog.h"
#include "bString.h"

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


bTWin::bTWin()
{
	gp_bTWin = this;
	Hover = bHover::None;
	Clicked = FALSE;
	Mouse.x = 0;
	Mouse.y = 0;
	Playlist = NULL;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
};
void  bTWin::OnClose()
{
	if (bLog::_bLogWin)
		bLog::_bLogWin->CloseWindow();
	CWnd::OnClose();
}

void  bTWin::OnTimer(int TimerID)
{
	QWORD progress = BASS_StreamGetFilePosition(Player.chan, BASS_FILEPOS_BUFFER)* 100 / BASS_StreamGetFilePosition(Player.chan, BASS_FILEPOS_END); 
	Player.BuffProgress = (int)progress;
	if (progress>90 || !BASS_StreamGetFilePosition(Player.chan, BASS_FILEPOS_CONNECTED))
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

BOOL bTWin::OnEraseBkgnd(CDC & dc)
{
	return TRUE;
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
};

bTWin::~bTWin()
{
	
};

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
};

void bTWin::PreCreate(CREATESTRUCT &cs)
{

	cs.dwExStyle = WS_EX_ACCEPTFILES;
	cs.lpszClass=L"bTuner";
#ifdef  _DEBUG
	cs.lpszName = L"[Debug] .:: bTuner ::.";
#else
	cs.lpszName = L".:: bTuner ::.";
#endif
	cs.style=WS_OVERLAPPED|WS_DLGFRAME|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE;
	cs.cx=700;
	cs.cy=500;
};
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

	SetRect(&VolumeRect, GetClientRect().right - 120, GetClientRect().bottom - 57, GetClientRect().right - 20, GetClientRect().bottom - 43);
	SetRect(&PlayRect, 160, GetClientRect().bottom - 70, 250, GetClientRect().bottom - 30);

	Player.SetVolume(Config.LastVolume);
	MoveWindow(Config.LastWindowPos.x,Config.LastWindowPos.y,700,500);
	if (Player.PlayingNow)
		delete Player.PlayingNow;
	Player.PlayingNow = new bStation;

	Player.PlayingNow->Streams.push_back(bStream(Config.LastPlayed.Streams.at(0)));
	Player.PlayingNow->Name = Config.LastPlayed.Name;
	Player.PlayingNow->Url = Config.LastPlayed.Url;
	Player.PlayingNow->Image = Config.LastPlayed.Image;

	
	bList.Create(*this);
	bList.OnCreate();


	
	Playlist = new bPlaylist;
	Playlist->LoadFile(L"../../../test/bFavorites.xspf");
	for (unsigned int i = 0; i < Playlist->Stations.size(); i++)
		bList.AddStation(Playlist->Stations.at(i));

	
	/*
	xml_document doc;
	bLog::AddLog(bLogEntry(L"Loading .... yp.xml", L"bTuner Win", LogType::Info));
	xml_parse_result result = doc.load_file("../../../test/yp.xml");

	if (result)
	{
		xml_node nDir = doc.child(L"directory");
		Playlist = new bPlaylist;
		int i = 0;
		for (xml_node nEntry = nDir.first_child(); nEntry; nEntry = nEntry.next_sibling())
		{
			bStation st;
			st.Name = nEntry.child(L"server_name").text().as_string();
			bStream s(nEntry.child(L"listen_url").text().as_string());
			st.Streams.push_back(s);
			Playlist->Stations.push_back(st);
			LV_ITEM *it = new LV_ITEM;
			it->mask = LVIF_TEXT;
			it->iSubItem = 0;
			it->pszText = (LPWSTR)nEntry.child(L"server_name").text().as_string();
			it->lParam = (LPARAM)i;
			it->iItem = i;
			bList.InsertItem(*it);


			LV_ITEM *id = new LV_ITEM;
			id->mask = LVIF_TEXT;
			id->iSubItem = 1;
			CString sID;
			sID.Format(L"%u", i);
			id->pszText = (LPWSTR)sID.c_str();
			id->lParam = (LPARAM)sID.c_str();
			id->iItem = i;
			bList.SetItem(*id);
			i++;
		}

		//Playlist->Sort();

		CString msg;
		msg.Format(L"[ %u ] Station Loaded From yp.xml", Playlist->Stations.size());
		bLog::AddLog(bLogEntry(msg.c_str(), L"bTuner Win", LogType::Info));

	}
	
	*/
	UpdateWindow();

	if (Config.LogWindow)
	{
		bLog::_bLogWin = new bLogWin;
		bLog::_bLogWin->Create();
		bLog::_bLogWin->MoveWindow(GetWindowRect().right, GetWindowRect().top, 500, 500);
		if (!bLog::_bLogWin->GetHwnd())
			bLog::AddLog(bLogEntry(L"Failed to create Log window", L"bTuner App", eLogType::Error));
	}
	SetFocus();
	return 0;
};


void bTWin::OnDraw(CDC& dc)
{
	//Graphics graphics(dc);

	CRect cr=GetClientRect();
	RECT r;
	SetRect(&r, 0, 0, 150, cr.bottom-150);
	dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(20, 20, 20)));

	HDC hdcBuffer = CreateCompatibleDC(dc);  // OFF screen DC
	int h, w;
	h = 150;
	w = GetClientRect().Width();
	HBITMAP hBitmapBuffer = CreateCompatibleBitmap(dc, w, h);  // create memory bitmap for that off screen DC

	SelectObject(hdcBuffer, hBitmapBuffer); // Select the created memory bitmap into the OFF screen DC

	DrawPlayer(CDC(hdcBuffer,*this));								/* Then do your painting job using hdcBuffer over off screen DC */

	BitBlt(dc, 0,GetClientRect().bottom-150, w, h, hdcBuffer, 0, 0, SRCCOPY); // copy the content of OFF screen DC to actual screen DC

	DeleteDC(hdcBuffer); // Release the OFF screen DC

	DeleteObject(hBitmapBuffer); // Free the memory for bitmap
	

};

LRESULT bTWin::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int prv;
	PDRAWITEMSTRUCT pdis;
	PMEASUREITEMSTRUCT pmis;
	switch (uMsg)
	{
	case WM_MEASUREITEM:

		pmis = (PMEASUREITEMSTRUCT)lParam;
		pmis->itemHeight = 30;
		return TRUE;
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
};

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
			GetMenu().CheckMenuItem(ID_HELP_LOGWINDOW,MF_CHECKED);
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
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_OPENURL), this->GetHwnd(), (DLGPROC)&g_Diagproc);
		
		break;
	case ID_HELP_ABOUT:
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_ABOUT), this->GetHwnd(), (DLGPROC)&g_Diagproc);
		break;
		
	}

	return TRUE;
};

INT_PTR bTWin::Diagproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	RECT rcOwner, rcDlg,rc;
	HFONT font;
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
		      break;

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
};

void  bTWin::DrawPlayer(CDC& dc)
{

	Graphics graphics(dc);
	CRect cr(0,0,GetClientRect().right,150);
	RECT r;
	CFont font;
	CBrush brush;

	int id = Playlist->Locate(Player.PlayingNow->Streams.at(Player.PlayingNow->PlayedStreamID).Url);
	if (id >= 0)
	{
		if (id != bList.PlayingNowID)
		{
			bList.PlayingNowID = id;
			bList.RedrawWindow();
		}
	}

	SetRect(&r, 0, cr.bottom - 150, cr.right, cr.bottom);
	dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)));

	if (Player.CoverLoaded&&Player.status == eStatus::Playing)
	{
		Image image(L"Cover.png");
		graphics.DrawImage(&image, 5, cr.bottom - 145, 140, 140);
	}
	else
	{
		SetRect(&r, 5, cr.bottom - 145, 145, cr.bottom - 5);
		//brush.CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 0));
		HBRUSH hBrush = ::CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 0));
		if (hBrush != 0)
			brush.Attach(hBrush);

		
		dc.SelectObject(brush);
		dc.FillRect(r, hBrush);
	}

	SetRect(&r, 150, cr.bottom - 75, cr.right - 5, cr.bottom - 25);
	dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(20, 20, 20)));


	brush.CreateSolidBrush(RGB(200, 200, 200));
	if (Hover == bHover::Play)
		brush.CreateSolidBrush(RGB(255, 255, 255));
	dc.SelectObject(brush);
	SetRect(&r, 160, cr.bottom - 70, 250, cr.bottom - 30);
	dc.RoundRect(r, 8, 8);

	brush.CreateSolidBrush(RGB(0, 0, 0));
	dc.SelectObject(brush);
	SetRect(&r, 163, cr.bottom - 67, 247, cr.bottom - 33);
	dc.RoundRect(r, 8, 8);

	brush.CreateSolidBrush(RGB(200, 200, 200));
	dc.SelectObject(brush);
	SetRect(&r, 165, cr.bottom - 65, 245, cr.bottom - 35);
	dc.RoundRect(r, 5, 5);



	font.CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	dc.SelectObject(font);
	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkMode(TRANSPARENT);
	if (Player.status == eStatus::Stopped)
	{
		dc.TextOut(195, cr.bottom - 65, L"Play", 4);

		brush.CreateSolidBrush(RGB(0, 0, 0));
		dc.SelectObject(brush);
		POINT points[3];
		points[0].x = 172;
		points[0].y = cr.bottom - 58;
		points[1].x = 185;
		points[1].y = cr.bottom - 50;
		points[2].x = 172;
		points[2].y = cr.bottom - 42;

		dc.Polygon(points, 3);
	}

	if (Player.status != eStatus::Stopped)
	{
		dc.TextOut(195, cr.bottom - 65, L"Stop", 4);

		brush.CreateSolidBrush(RGB(0, 0, 0));
		dc.SelectObject(brush);

		SetRect(&r, 172, cr.bottom - 58, 185, cr.bottom - 42);
		dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)));
	}

	// Volume 
	int v = Player.GetVolume();
	if (Clicked&&VolumeRect.PtInRect(Mouse))
		v = 100 - VolumeRect.right + Mouse.x;

	SetRect(&r, cr.right - 120, cr.bottom - 54, cr.right - 20, cr.bottom - 46);
	dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(100, 100, 100)));

	SetRect(&r, cr.right - 120, cr.bottom - 57, cr.right - 20 - (100 - v), cr.bottom - 43);
	dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(200, 200, 200)));

	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkMode(TRANSPARENT);
	font.CreateFont(14, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	dc.SelectObject(font);

	CString bf;
	bf.Format(L"%u", v);
	CSize sv = dc.GetTextExtentPoint32(bf, bf.GetLength());
	dc.TextOut(cr.right - 70 - (sv.cx / 2), cr.bottom - 57, bf, bf.GetLength());



	//Playing Now info

	dc.SetTextColor(RGB(245, 245, 245));
	dc.SetBkColor(RGB(0, 0, 0));

	font.CreateFont(45, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	dc.SelectObject(font);
	if (Player.status != eStatus::Playing)
		dc.SetTextColor(RGB(150, 150, 150));
	if (Player.PlayingNow && Player.PlayingNow->Name.size())
		dc.TextOut(150, cr.bottom - 145, Player.PlayingNow->Name.c_str(), Player.PlayingNow->Name.size());

	font.CreateFont(16, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
	dc.SelectObject(font);
	dc.SetTextColor(RGB(180, 180, 180));
	if (Player.status == eStatus::Buffering)
	{
		CString buf;
		buf.Format(L"[%u %s] Buffering", v,L"%");
		dc.TextOut(150, cr.bottom - 20, buf.c_str(),buf.GetLength());
	}
	if (Player.status == eStatus::Connecting)
		dc.TextOut(150, cr.bottom - 20, L"Connecting...", 13);
	if (Player.status == eStatus::Stopped)
		dc.TextOut(150, cr.bottom - 20, L"Not Connected", 13);


	if (Player.status == eStatus::Playing)
	{
		font.CreateFont(25, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
		dc.SelectObject(font);
		dc.SetTextColor(RGB(200, 200, 200));
		dc.TextOut(150, cr.bottom - 105, Player.PlayingNow->Playing.c_str(), Player.PlayingNow->Playing.size());

		font.CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
		dc.SelectObject(font);
		dc.SetTextColor(RGB(180, 180, 180));
		

		
		CSize s = dc.GetTextExtentPoint32(Player.PlayingNow->Url.c_str(), Player.PlayingNow->Url.size());
		LinkRect.top = GetClientRect().bottom - 20;
		LinkRect.bottom = GetClientRect().bottom;
		LinkRect.left = 150;
		LinkRect.right = 150 + s.cx;
		if (Hover == bHover::Link)
			dc.SetTextColor(RGB(40, 185, 220));
		dc.TextOut(150, cr.bottom - 20, Player.PlayingNow->Url.c_str(), Player.PlayingNow->Url.size());
		font.CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		dc.SelectObject(font);
		dc.SetTextColor(RGB(0, 0, 0));
		dc.SetBkColor(RGB(245, 245, 245));
		dc.SetBkMode(OPAQUE);

		wchar_t *codecT[] = { L" MP3 ",L" AAC ",L" OGG ",L" MPEG ",L" AAC+ ",L" Opus ",L" NSV ",L"" };
		int c = Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding;
		if (c < eCodecs::UNDIFINED)
			dc.TextOut(150 + 10 + s.cx, cr.bottom - 20, codecT[c], wcslen(codecT[c]));

		if (Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate)
		{
			CSize s2 = dc.GetTextExtentPoint32(codecT[c], wcslen(codecT[c]));
			CString bf;
			bf.Format(L" %u Kbps " , Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate);
			dc.TextOut(150 + 10 + s.cx + 5 + s2.cx, cr.bottom - 20, bf.c_str(), bf.GetLength());
		}
	}


}