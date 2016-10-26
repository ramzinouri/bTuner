#include "bPlayer.h"
#include "bLog.h"



namespace
{
	bPlayer *gp_bPlayer = NULL;
}

void CALLBACK  g_MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if (gp_bPlayer)
		return gp_bPlayer->MetaSync(handle, channel, data, user);
	else
		return ;
}
void CALLBACK  g_DownloadProc(const void *buffer, DWORD length, void *user)
{
	if (gp_bPlayer)
		return gp_bPlayer->DownloadProc(buffer,length, user);
	else
		return;
}

void CALLBACK  g_EndSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if (gp_bPlayer)
		return gp_bPlayer->EndSync(handle, channel, data,user);
	else
		return;
}



bPlayer::bPlayer()
{
	gp_bPlayer = this;
	chan = NULL;
	hwnd = NULL;
	Volume = 100;
	PlayingNow = NULL;
	status = eStatus::Stopped;
	CoverLoaded = false;
	StationTime = NULL;
	TrackTime = NULL;
	InitBass();

}

int bPlayer::Play()
{
	bLog::AddLog(bLogEntry(L"Playing : " + PlayingNow->Name, L"bPlayer", eLogType::Player));
	if (BASS_ChannelPlay(chan, FALSE))
	{
		status = eStatus::Playing;
		if (StationTime)
			delete StationTime;
		StationTime = new time_t;
		if (TrackTime)
			delete TrackTime;
		TrackTime = new time_t;
		time(StationTime);
		time(TrackTime);
		MetaSync(NULL, NULL, NULL, NULL);
		UpdateWnd();
		return true;
	}
	return false;
}

void bPlayer::Stop()
{
	BASS_ChannelStop(chan);
	BASS_StreamFree(chan);
	status=eStatus::Stopped;
	UpdateWnd();
	bLog::AddLog(bLogEntry(L"Stopped", L"bPlayer", eLogType::Error));
	
}
void bPlayer::Resume()
{
	hThreadArray[0] = (HANDLE)_beginthreadex(NULL, 0, &bPlayer::StaticThreadEntry, (void*)eThread::Openurl, 0, &threadID[0]);

}

void bPlayer::OpenURL(wstring URL)
{
	if (PlayingNow->Streams[PlayingNow->PlayedStreamID].Url!=URL || status==eStatus::Stopped)
	{
		if (PlayingNow)
			delete PlayingNow;
		PlayingNow = new bStation;
		PlayingNow->Streams.push_back(bStream(URL));
		hThreadArray[0] = (HANDLE)_beginthreadex(NULL, 0, &bPlayer::StaticThreadEntry, (void*)eThread::Openurl, 0, &threadID[0]);
	}
	
}
void bPlayer::OpenStation(const bStation& Station)
{
	if (PlayingNow->ID != Station.ID || status == eStatus::Stopped || PlayingNow->PlayedStreamID != Station.PlayedStreamID)
	{
		PlayingNow->Name = Station.Name;
		PlayingNow->ID = Station.ID;
		PlayingNow->Url = Station.Url;
		PlayingNow->Image = Station.Image;
		PlayingNow->PlayedStreamID = Station.PlayedStreamID;
		PlayingNow->Streams = Station.Streams;

		hThreadArray[0] = (HANDLE)_beginthreadex(NULL, 0, &bPlayer::StaticThreadEntry, (void*)eThread::Openurl, 0, &threadID[0]);
	}

}

unsigned __stdcall bPlayer::StaticThreadEntry(void* c)
{
	UINT_PTR i = (UINT_PTR)c;
	if(i == eThread::Openurl)
		gp_bPlayer->OpenThread();
	if (i == eThread::Fetchurl)
		gp_bPlayer->FetchCover();
	if (i == eThread::Downloadcover)
		gp_bPlayer->DownloadCover();
	if (i == eThread::Downloadimage)
		gp_bPlayer->DownloadImage();
	return  0;
}


void bPlayer::OpenThread()
{
	BASS_ChannelStop(chan);
	BASS_StreamFree(chan);
	BASS_Free();
	InitBass();
	bLog::AddLog(bLogEntry(L"Connecting To: " + PlayingNow->Streams[PlayingNow->PlayedStreamID].Url, L"bPlayer", eLogType::Info));
	status = eStatus::Connecting;
	CoverLoaded = false;
	ImageLoaded = false;
	UpdateWnd();

	KillTimer(hwnd, 0);	

	chan = BASS_StreamCreateURL(PlayingNow->Streams[PlayingNow->PlayedStreamID].Url.c_str(), 0, BASS_STREAM_BLOCK | BASS_STREAM_STATUS | BASS_STREAM_AUTOFREE, g_DownloadProc, 0);
	if (!chan)
	{
		bLog::AddLog(bLogEntry(L"Can't Open Stream : " + PlayingNow->Streams[PlayingNow->PlayedStreamID].Url, L"bPlayer", eLogType::Error));
		status = eStatus::Stopped;
		UpdateWnd();
	}
	else
	{
		SetVolume(Volume);
		SetTimer(hwnd, 0, 50, 0);
		if (PlayingNow->Image.size())
		{
			//WaitForSingleObject(hThreadArray[2], INFINITE);
			hThreadArray[2] = (HANDLE)_beginthreadex(NULL, 0, &bPlayer::StaticThreadEntry, (void*)eThread::Downloadimage, 0, &threadID[2]);
		}

	}


}
bool bPlayer::FetchCover()
{
	std::wstring ser = PlayingNow->Artist + L" ";
	std::wstring tr = PlayingNow->Track;
	if (tr.find(L"(") != std::wstring::npos)
		tr.erase(tr.find(L"("), tr.length());
	std::wstring url = L"http://ws.audioscrobbler.com";
	url += L"/2.0/?method=album.search&album=" ;
	url +=ser+ tr;
	url += L"&api_key=c4eeb5aa39807b0d21d420ab64b42bf6&limit=1&page=1";
	std::string data = bHttp::FetchString(url);
	if (data.find("</lfm>")==std::string::npos)
		return 0;
	xml_document doc;
	xml_parse_result result = doc.load_string(bString::ConvertToW(data).c_str());
	if (result)
	{
		xml_node album= doc.child(L"lfm").child(L"results").child(L"albummatches").first_child();

		CoverUrl=album.find_child_by_attribute(L"size", L"large").text().as_string();

		if (CoverUrl.length() > 0)
		{
			WaitForSingleObject(hThreadArray[2], INFINITE);
			hThreadArray[2] = (HANDLE)_beginthreadex(NULL, 0, &bPlayer::StaticThreadEntry, (void*)eThread::Downloadcover, 0, &threadID[2]);
		}
		else
		{
			CoverLoaded = false;
			if (PlayingNow->Image.size()&& !ImageLoaded)
			{
				WaitForSingleObject(hThreadArray[2], INFINITE);
				hThreadArray[2] = (HANDLE)_beginthreadex(NULL, 0, &bPlayer::StaticThreadEntry, (void*)eThread::Downloadcover, 0, &threadID[2]);
				return true;
			}
			else
				UpdateWnd();

		}
	}

	return true;
}

bool bPlayer::DownloadCover()
{
	CoverLoaded = bHttp::DownloadFile(CoverUrl, L"Cover.png");
	UpdateWnd();
	return CoverLoaded;
}

bool bPlayer::DownloadImage()
{
	ImageLoaded = bHttp::DownloadFile(PlayingNow->Image, L"Station.png");
	UpdateWnd();
	return ImageLoaded;
}

int bPlayer::GetVolume()
{
	return Volume;
}

void bPlayer::SetVolume(int Vol)
{
	if (Vol > 100)
		Vol = 100;
	if (Vol < 0)
		Vol = 0;
	Volume = Vol;
	float v = Vol/(float )100.0;
	BASS_ChannelSetAttribute(chan, BASS_ATTRIB_VOL, v);
}
void bPlayer::UpdateWnd()
{
	HMENU hmenu = GetMenu(hwnd);
	CMenu menu(hmenu);
	if (status != eStatus::Playing)
		::SetWindowText(hwnd, L".:: bTuner ::.");

	if (status != eStatus::Playing)
	{
		if (StationTime)
		{
			delete StationTime;
			StationTime = NULL;
		}
		if (TrackTime)
		{
			delete TrackTime;
			TrackTime = NULL;
		}
	}

	if (status == eStatus::Playing)
	{
		menu.EnableMenuItem(ID_PLAYBACK_STOP, MF_ENABLED);
		menu.EnableMenuItem(ID_PLAYBACK_RESUME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	if (status == eStatus::Stopped)
	{
		menu.EnableMenuItem(ID_PLAYBACK_RESUME, MF_ENABLED);
		menu.EnableMenuItem(ID_PLAYBACK_STOP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	if (status == eStatus::Playing&&PlayingNow->Playing.size())
	{
		std::wstring title = PlayingNow->Playing + L" -- .:: bTuner ::.";
		::SetWindowText(hwnd, title.c_str());
	}
	RECT cr, r;
	GetClientRect(hwnd,&cr);
	SetRect(&r, 0, cr.bottom - 150, cr.right, cr.bottom);
	InvalidateRect(hwnd,&r,FALSE);
}
void bPlayer::ProcessTags(const char * buffer)
{
	if (buffer) {
		for (; *buffer; buffer += strlen(buffer) + 1) {
			if (!strnicmp(buffer, "icy-name:", 9))
				if(strlen(buffer+9)>2)
					PlayingNow->Name = bString::TrimLeft(bString::PointerToString(buffer + 9));
			if (!strnicmp(buffer, "icy-url:", 8))
				if (strlen(buffer + 8)>1)
					PlayingNow->Url = bString::TrimLeft(bString::PointerToString(buffer + 8));
			if (!strnicmp(buffer, "icy-br:", 7))
				if (strlen(buffer + 7)>1)
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Bitrate = atoi(buffer + 7);
			if (!strnicmp(buffer, "icy-genre:", 10))
				if (strlen(buffer + 10)>2)
					PlayingNow->Genre = bString::TrimLeft(bString::PointerToString(buffer + 10));
			
			if (!strnicmp(buffer, "Content-Type:", 13))
			{
				char *stype = (char*)buffer + 14;
				if (!strnicmp(stype, "audio/mp3", 9))
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Encoding = eCodecs::MP3;
				if (!strnicmp(stype, "audio/aac", 9))
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Encoding = eCodecs::AAC;
				if (!strnicmp(stype, "audio/aacp", 10))
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Encoding = eCodecs::AACP;
				if (!strnicmp(stype, "audio/mpeg", 10))
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Encoding = eCodecs::MP3;
				if (!strnicmp(stype, "video/nsv", 9))
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Encoding = eCodecs::NSV;

			}
#ifdef _DEBUG
			bLog::AddLog(bLogEntry(L"TAGS :: " + bString::PointerToString(buffer), L"debug", eLogType::Debug));
#endif	
		}
	}
	UpdateWnd();
}
void bPlayer::DownloadProc(const void *buffer, DWORD length, void *user)
{
	char *http = (char*)buffer;
	if (buffer && !length)
	{
		std::string h(http);
		if (h.find("200")==std::string::npos)
		{
			//bLog::AddLog(bLogEntry(L"Stream Url Error", L"bPlayer", eLogType::Error));
			for (; *http; http += strlen(http) + 1) {
				bLog::AddLog(bLogEntry(L"HTTP :: " + bString::PointerToString(http), L"debug", eLogType::Debug));
			}
			return;
		}
		for (; *http; http += strlen(http) + 1) {
			if (!strnicmp(http, "icy", 3))
			{
				ProcessTags((const char *)buffer);
				return;
			}
		}
		http = (char*)buffer;
		for (; *http; http += strlen(http) + 1) {
			bLog::AddLog(bLogEntry(L"HTTP :: " + bString::PointerToString(http), L"debug", eLogType::Debug));
		}
	}

}

void bPlayer::EndSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	bLog::AddLog(bLogEntry(L"EndSync : " + PlayingNow->Name, L"bPlayer", eLogType::Error));
	status = eStatus::Stopped;
	UpdateWnd();
}
void bPlayer::InitBass()
{
	if (!BASS_Init(-1, 44100, BASS_DEVICE_STEREO, 0, NULL)) {
		bLog::AddLog(bLogEntry(L"Cant Initialize Bass Device", L"bPlayer", eLogType::Error));
	}
	BASS_PluginLoad("bass_aac.dll", 0);
	BASS_PluginLoad("bassopus.dll", 0);
	BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);
	BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);
	BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, (void*)NULL);
}
void bPlayer::MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{	
	char *meta = (char *)BASS_ChannelGetTags(chan, BASS_TAG_META);
	if (meta) { // got Shoutcast metadata
		char *p = strstr(meta, "StreamTitle='"); // locate the title
		if (p) {
			const char *p2 = strstr(p, "';"); // locate the end of it
			if (p2) {
				char *t = _strdup(p + 13);
				t[p2 - (p + 13)] = 0;
				char *pl = new char[strlen(t)+1];
				memcpy(pl, t, strlen(t) + 1);
				PlayingNow->Playing = bString::Trim(bString::PointerToString(pl));
				p = strstr(t, "-");
				if (p) {
					p[-1] = 0;
					PlayingNow->Artist = bString::Trim(bString::PointerToString(t));
					PlayingNow->Track = bString::Trim(bString::PointerToString(p+2));
				}

			}
		}
	}
	else {
		meta = (char *)BASS_ChannelGetTags(chan, BASS_TAG_OGG);
		if (meta) {
			char *artist = NULL, *title = NULL, *p = meta;
			for (; *p; p += strlen(p) + 1) {
				if (!_strnicmp(p, "artist=", 7))
					artist = p + 7;
				if (!_strnicmp(p, "title=", 6))
					title = p + 6;
#ifdef _DEBUG
				bLog::AddLog(bLogEntry(L"OGG META:: " + bString::PointerToString(p), L"debug", eLogType::Debug));
#endif
			}
			if (title) {
				
				PlayingNow->Track = bString::Trim(bString::PointerToString(title));
				if (artist) {
					PlayingNow->Artist = bString::Trim(bString::PointerToString(artist));
					if (PlayingNow->Artist.size())
					{
						PlayingNow->Playing = PlayingNow->Artist + L" - ";
						if (PlayingNow->Track.size())
							PlayingNow->Playing += PlayingNow->Track;
					}
					else
						PlayingNow->Playing = PlayingNow->Track;

					if (!PlayingNow->Name.size())
						PlayingNow->Name = PlayingNow->Artist;
				}
				else
				{
					PlayingNow->Playing = PlayingNow->Track;
				}
 				
			}
		}
	}
	if (!PlayingNow->Playing.size() )
		return;

	if (TrackTime)
		delete TrackTime;
	TrackTime = new time_t;
	time(TrackTime);
	
	//if(hThreadArray[1])
	//	WaitForSingleObject(hThreadArray[1], INFINITE);
	hThreadArray[1] = (HANDLE)_beginthreadex(NULL, 0, &bPlayer::StaticThreadEntry, (void*)eThread::Fetchurl, 0, &threadID[1]);
	
	bLog::AddLog(bLogEntry(L"Track : " + PlayingNow->Playing, L"bPlayer", eLogType::Track));
	UpdateWnd();
}

bPlayer::~bPlayer()
{
	BASS_StreamFree(chan);
	BASS_Free();
}