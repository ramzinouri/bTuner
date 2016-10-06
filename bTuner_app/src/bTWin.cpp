#include "bTWin.h"


namespace
{
	bTWin *gp_bTWin = NULL;
}

INT_PTR CALLBACK  g_AboutDiagproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	if (gp_bTWin)
		return gp_bTWin->AboutDiagproc(h, m, w, l);
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
};
void bTWin::OnDestroy()
{
	
	Config.LastVolume= Player.GetVolume();
	Config.LastPlayedName = Player.PlayingNow->Name;
	Config.LastPlayedUrl = Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Url;
	Config.LastWindowPos.x = GetWindowRect().top;
	Config.LastWindowPos.y = GetWindowRect().left;
	
	Config.Save();
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
	wc.lpszClassName="bTuner";
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
};

void bTWin::PreCreate(CREATESTRUCT &cs)
{

	cs.dwExStyle = WS_EX_ACCEPTFILES;
	cs.lpszClass="bTuner";
	cs.lpszName = ".:: bTuner ::. V 0.0.0.1";
#ifdef  _DEBUG
	cs.lpszName = "[Debug] .:: bTuner ::. V 0.0.0.1";
#endif //  _DEBUG
	cs.style=WS_OVERLAPPED|WS_DLGFRAME|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE;
	cs.cx=700;
	cs.cy=500;
};
int bTWin::OnCreate(CREATESTRUCT& cs)
{
	Player.hwnd = this->GetHwnd();
	this->GetMenu().EnableMenuItem(ID_PLAYBACK_RESUME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	this->GetMenu().EnableMenuItem(ID_PLAYBACK_STOP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	SetRect(&VolumeRect, GetClientRect().right - 120, GetClientRect().bottom - 57, GetClientRect().right - 20, GetClientRect().bottom - 43);
	SetRect(&PlayRect, 160, GetClientRect().bottom - 70, 250, GetClientRect().bottom - 30);
	Config.Load();

	Player.SetVolume(Config.LastVolume);
	//MoveWindow(Config.LastWindowPos.x,Config.LastWindowPos.y,700,500);
	if (Player.PlayingNow)
		delete Player.PlayingNow;
	Player.PlayingNow = new bStation;
	Player.PlayingNow->Streams.push_back(bStream((char*)Config.LastPlayedUrl.c_str()));
	Player.PlayingNow->Name = (char*)Config.LastPlayedName.c_str();



#ifdef  _DEBUG
	//Player.OpenURL("http://stream12.iloveradio.de/iloveradio5-aac.mp3");
#endif //  _DEBUG
	//Player.OpenURL("http://dir.xiph.org/listen/370585/listen.m3u");
	//Player.OpenURL("http://jil-fm.ice.infomaniak.ch/jilfm.aac");
	//Player.OpenURL("http://7619.live.streamtheworld.com:80/977_HITS_SC");
	
	
	return 0;
};


void bTWin::OnDraw(CDC& dc)
{
	CRect cr = GetClientRect();
	RECT r;
	CFont font;
	CBrush brush;

	SetRect(&r, 0, cr.bottom - 150, cr.right, cr.bottom);
	dc.FillRect(r,(HBRUSH)CreateSolidBrush(RGB(0, 0, 0)));

	SetRect(&r, 5, cr.bottom - 145, 145, cr.bottom-5);

	brush.CreateHatchBrush(HS_BDIAGONAL,RGB(0,0,0));
	dc.SelectObject(brush);
	dc.RoundRect(r, 10, 10);





	SetRect(&r, 150, cr.bottom - 75, cr.right -5, cr.bottom - 25);
	dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(20, 20, 20)));


	brush.CreateSolidBrush( RGB(200, 200, 200));
	if(Hover==bHover::Play)
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
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	dc.SelectObject(font);
	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkMode(TRANSPARENT);
	if (Player.status == Status::Stoped)
	{
		dc.TextOut(195, cr.bottom - 65, "Play", 4);

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

	if (Player.status == Status::Playing)
	{
		dc.TextOut(195, cr.bottom - 65, "Stop", 4);

		brush.CreateSolidBrush(RGB(0, 0, 0));
		dc.SelectObject(brush);

		SetRect(&r, 172, cr.bottom - 58, 185, cr.bottom - 42);
		dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)));
	}

	// Volume 
	int v = Player.GetVolume();
	if(Clicked&&VolumeRect.PtInRect(Mouse))
		v = 100 - VolumeRect.right + Mouse.x;
	
	SetRect(&r, cr.right-120, cr.bottom - 54, cr.right-20, cr.bottom - 46);
	dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(100, 100, 100)));

	SetRect(&r, cr.right-120, cr.bottom - 57, cr.right-20-(100-v), cr.bottom - 43);
	dc.FillRect(r, (HBRUSH)CreateSolidBrush(RGB(200, 200, 200)));

	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkMode(TRANSPARENT);
	font.CreateFontA(14, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	dc.SelectObject(font);

	char bf[10];
	sprintf(bf, "%d", v);
	CSize sv= dc.GetTextExtentPoint32A(bf, strlen(bf));
	dc.TextOut(cr.right-70-(sv.cx/2), cr.bottom - 57, bf, strlen(bf));



	//Playing Now info

	dc.SetTextColor(RGB(245, 245, 245));
	dc.SetBkColor(RGB(0, 0, 0));

	font.CreateFontA(45, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	dc.SelectObject(font);
	if(Player.status != Status::Playing)
		dc.SetTextColor(RGB(150, 150, 150));
	if(Player.PlayingNow && Player.PlayingNow->Name)
		dc.TextOutA(150, cr.bottom - 145, Player.PlayingNow->Name, strlen(Player.PlayingNow->Name));

	font.CreateFontA(16, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
	dc.SelectObject(font);
	dc.SetTextColor(RGB(180, 180, 180));
	if (Player.status == Status::Buffring)
	{
		char buf[30];
		sprintf(buf, "[%d %s] Buffuring",Player.BuffProgress,"%");
		dc.TextOutA(150, cr.bottom - 20, buf, strlen(buf));
	}
	if (Player.status == Status::Connecting)
		dc.TextOutA(150, cr.bottom - 20, "Connecting...",13 );
	if (Player.status == Status::Stoped)
		dc.TextOutA(150, cr.bottom - 20, "Not Connected", 13);


	if (Player.status == Status::Playing)
	{
		font.CreateFontA(25, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
		dc.SelectObject(font);
		dc.SetTextColor(RGB(200, 200, 200));
		dc.TextOutA(150, cr.bottom - 110, Player.PlayingNow->Playing, strlen(Player.PlayingNow->Playing));

		font.CreateFontA(16, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"));
		dc.SelectObject(font);
		dc.SetTextColor(RGB(180, 180, 180));
		dc.TextOutA(150, cr.bottom - 20, Player.PlayingNow->Url, strlen(Player.PlayingNow->Url));

		CSize s = dc.GetTextExtentPoint32A(Player.PlayingNow->Url, strlen(Player.PlayingNow->Url));
		font.CreateFontA(14, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		dc.SelectObject(font);
		dc.SetTextColor(RGB(0, 0, 0));
		dc.SetBkColor(RGB(245, 245, 245));
		dc.SetBkMode(OPAQUE);

		char *codecT[] = { "MP3","AAC","OGG","MPEG","AAC+" };
		int c = Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding;
		if (c < Codecs::UNDIFINED)
		{
			dc.TextOutA(150 + 10 + s.cx, cr.bottom - 20, codecT[c], strlen(codecT[c]));
		}

		if (Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate)
		{
			CSize s2 = dc.GetTextExtentPoint32A(codecT[c], strlen(codecT[c]));
			char b[10];
			sprintf(b, "%d Kbps", Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate);
			dc.TextOutA(150 + 10 + s.cx + 5 + s2.cx, cr.bottom - 20, b, strlen(b));
		}
	}


};

LRESULT bTWin::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int prv;
	switch (uMsg)
	{
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
			Player.SetVolume(100 - VolumeRect.right + Mouse.x);
		if (PlayRect.PtInRect(Mouse) && !Clicked)
		{
			if (Player.status == Status::Playing)
				Player.Stop();
			else
			{
				if (Player.status == Status::Stoped&& Player.PlayingNow)
					Player.Resume();
			}
			InvalidateRect(GetClientRect(), TRUE);
		}

		InvalidateRect(VolumeRect, TRUE);

		break;
	case WM_MOUSEMOVE:

		Mouse.x = LOWORD(lParam);
		Mouse.y = HIWORD(lParam);
		prv = Hover;
		if (VolumeRect.PtInRect(Mouse))
			Hover = bHover::Volume;
		else
			Hover = bHover::None;
		if (PlayRect.PtInRect(Mouse))
			Hover = bHover::Play;
		else
			Hover = bHover::None;
		if (Clicked&&VolumeRect.PtInRect(Mouse)&& VolumeRect.PtInRect(ClickP))
		{
			InvalidateRect(VolumeRect, TRUE);
			Player.SetVolume(100 - VolumeRect.right + Mouse.x);
		}

		if (prv != Hover)
		{
			InvalidateRect(VolumeRect, TRUE);
			InvalidateRect(PlayRect, TRUE);
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
	{ // monitor prebuffering progress
		DWORD progress = BASS_StreamGetFilePosition(Player.chan, BASS_FILEPOS_BUFFER)
			* 100 / BASS_StreamGetFilePosition(Player.chan, BASS_FILEPOS_END); // percentage of buffer filled
		Player.BuffProgress = progress;
		if (progress>90 || !BASS_StreamGetFilePosition(Player.chan, BASS_FILEPOS_CONNECTED)) { // over 75% full (or end of download)
			::KillTimer(this->GetHwnd(), 0); // finished prebuffering, stop monitoring
			{ // get the broadcast name and URL
				const char *icy = BASS_ChannelGetTags(Player.chan, BASS_TAG_ICY);
				if (!icy) icy = BASS_ChannelGetTags(Player.chan, BASS_TAG_HTTP); // no ICY tags, try HTTP
				if (icy) {
					for (; *icy; icy += strlen(icy) + 1) {
						if (!strnicmp(icy, "icy-name:", 9))
							Player.PlayingNow->Name = (char*)icy + 9;
						if (!strnicmp(icy, "icy-url:", 8))
							Player.PlayingNow->Url = (char*)icy + 8;
						if (!strnicmp(icy, "icy-br:", 7))
							Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate = atoi(icy + 7);
						if (!strnicmp(icy, "icy-genre:", 10))
							Player.PlayingNow->Genre = (char*)icy + 10;
						if (!strnicmp(icy, "icy-notice1:", 12))
							Player.PlayingNow->Notice1 = (char*)icy + 12;
						if (!strnicmp(icy, "icy-notice2:", 12))
							Player.PlayingNow->Notice2 = (char*)icy + 12;
						if (!strnicmp(icy, "icy-pub:", 8))
							Player.PlayingNow->Public = atoi(icy + 8);

						if (!strnicmp(icy, "icy-name: ", 10))
							Player.PlayingNow->Name = (char*)icy + 10;
						if (!strnicmp(icy, "icy-url: ", 9))
							Player.PlayingNow->Url = (char*)icy + 9;
						if (!strnicmp(icy, "icy-br: ", 8))
							Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Bitrate = atoi(icy + 8);
						if (!strnicmp(icy, "icy-genre: ", 11))
							Player.PlayingNow->Genre = (char*)icy + 11;
						if (!strnicmp(icy, "icy-notice1: ", 13))
							Player.PlayingNow->Notice1 = (char*)icy + 13;
						if (!strnicmp(icy, "icy-notice2 :", 13))
							Player.PlayingNow->Notice2 = (char*)icy + 13;
						if (!strnicmp(icy, "icy-pub: ", 9))
							Player.PlayingNow->Public = atoi(icy + 9);

						if (!strnicmp(icy, "Content-Type:", 13))
						{
							char *stype = (char*)icy + 14;
							if (!strnicmp(stype, "audio/mp3", 9))
								Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = Codecs::MP3;
							if (!strnicmp(stype, "audio/aac", 9))
								Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = Codecs::AAC;
							if (!strnicmp(stype, "audio/aacp", 10))
								Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = Codecs::AACP;
							if (!strnicmp(stype, "audio/ogg", 9))
								Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = Codecs::OGG;
							if (!strnicmp(stype, "audio/mpeg", 10))
								Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = Codecs::MPEG;

							BASS_CHANNELINFO  info;
							BASS_ChannelGetInfo(Player.chan, &info); // get info
							if (info.ctype == BASS_CTYPE_STREAM_MP3)
								Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = Codecs::MP3;
							if (info.ctype == BASS_CTYPE_STREAM_OGG)
								Player.PlayingNow->Streams[Player.PlayingNow->PlayedStreamID].Encoding = Codecs::OGG;
						}
					}
				}
			}
			// get the stream title and set sync for subsequent titles
			Player.MetaSync(NULL, NULL, NULL, NULL);
			BASS_ChannelSetSync(Player.chan, BASS_SYNC_META, 0, &bPlayer::MetaSync, 0); // Shoutcast
			BASS_ChannelSetSync(Player.chan, BASS_SYNC_OGG_CHANGE, 0, &bPlayer::MetaSync, 0); // Icecast/OGG
																			  // set sync for end of stream
			//BASS_ChannelSetSync(Player.chan, BASS_SYNC_END, 0, &EndSync, 0);
			// play it!

			Player.Play();
			Player.status = Status::Playing;
			this->GetMenu().EnableMenuItem(ID_PLAYBACK_STOP, MF_ENABLED );

		
		}
		else
			Player.status = Status::Buffring;
		InvalidateRect(this->GetClientRect(), TRUE);
		
	}
	break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	}

	return WndProcDefault(uMsg, wParam, lParam);
};

BOOL bTWin::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_PLAYBACK_RESUME:
		Player.Resume();
		this->GetMenu().EnableMenuItem(ID_PLAYBACK_RESUME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		this->GetMenu().EnableMenuItem(ID_PLAYBACK_STOP, MF_ENABLED);
		InvalidateRect(this->GetClientRect(), TRUE);
		break;

	case ID_PLAYBACK_STOP:
		Player.Stop();
		this->GetMenu().EnableMenuItem(ID_PLAYBACK_RESUME, MF_ENABLED);
		this->GetMenu().EnableMenuItem(ID_PLAYBACK_STOP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		InvalidateRect(this->GetClientRect(), TRUE);
		break;


	case ID_FILE_EXIT:
		PostQuitMessage(0);
		break;

	case ID_FILE_OPEN_FILE:
		break;
	case ID_FILE_OPEN_URL:
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_OPENURL), this->GetHwnd(), (DLGPROC)&g_AboutDiagproc);
		
		break;
	case ID_HELP_ABOUT:
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_ABOUT), this->GetHwnd(), (DLGPROC)&g_AboutDiagproc);
		break;
		
	}
	return TRUE;
};

INT_PTR bTWin::AboutDiagproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m) {
	case WM_INITDIALOG:
		//SendMessage(h, WM_NEXTDLGCTL, (WPARAM)::GetDlgItem(h,IDC_LINK), TRUE);
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
		break;
	case WM_CLOSE:
		EndDialog(h, NULL);
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)l)->code)
		{
		  case NM_CLICK:
			  if (((LPNMHDR)l)->idFrom == IDC_SYSLINK2)
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
			char *u = new char[256];
			memset(u, 0, 256);
			::GetDlgItemText(h, IDC_LINK, u, 256);
			if (!strnicmp(u, "http://", 7))
			{
				Player.OpenURL(u);
				EndDialog(h, NULL);
			}
		}
			
		break;
	}
	return 0;
}
