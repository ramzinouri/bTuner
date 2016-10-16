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
	if (!BASS_Init(-1, 44100, BASS_DEVICE_STEREO, 0, NULL)) {
			bLog::AddLog(bLogEntry(L"Cant Initialize Bass Device", L"bPlayer", LogType::Error));
	}
	BASS_PluginLoad("bass_aac.dll", 0);
	BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1); // enable playlist processing
	BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0); // minimize automatic pre-buffering, so we can do it (and display it) instead
	BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, (void*)NULL);

}

int bPlayer::Play()
{
	wstring msg = L"Playing : ";
	msg += PlayingNow->Name;
	bLog::AddLog(bLogEntry(msg, L"bPlayer", LogType::Info));
	return BASS_ChannelPlay(chan, FALSE);
}

void bPlayer::Stop()
{
	BASS_ChannelStop(chan);
	BASS_StreamFree(chan);
	status=eStatus::Stopped;
	bLog::AddLog(bLogEntry(L"Stopped", L"bPlayer", LogType::Info));
}
void bPlayer::Resume()
{
	_beginthread(&bPlayer::StaticThreadEntry, 0, (void*)eThread::Openurl);
}

void bPlayer::OpenURL(wstring URL)
{
	if (PlayingNow)
		delete PlayingNow;
	PlayingNow = new bStation;
	PlayingNow->Streams.push_back(bStream(URL));
	_beginthread(&bPlayer::StaticThreadEntry, 0, (void*)eThread::Openurl);
	
}

void bPlayer::StaticThreadEntry(void* c)
{
	int i = (int)c;
	if(i == eThread::Openurl)
		return gp_bPlayer->OpenThread();
	if (i == eThread::Fetchurl)
		gp_bPlayer->FetchCover();
	if (i == eThread::Downloadcover)
		gp_bPlayer->DownloadCover();
	return;
}


void bPlayer::OpenThread()
{
	std::wstring u = PlayingNow->Streams[PlayingNow->PlayedStreamID].Url;
	wstring msg = L"Connecting To: ";
	msg += u;
	bLog::AddLog(bLogEntry(msg, L"bPlayer", LogType::Info));
	status = eStatus::Connecting;
	RECT cr;
	GetClientRect(hwnd, &cr);
	InvalidateRect(hwnd, &cr, TRUE);
	CoverLoaded = false;
	KillTimer(hwnd, 0);
	BASS_StreamFree(chan);
	chan = BASS_StreamCreateURL(u.c_str(), 0, BASS_STREAM_BLOCK | BASS_STREAM_STATUS | BASS_STREAM_AUTOFREE, g_DownloadProc, 0);
	SetVolume(Volume);
	if (!chan)  // failed to open
	{
		wstring msg = L"Can't Open Stream : ";
		msg += u;
		bLog::AddLog(bLogEntry(msg, L"bPlayer", LogType::Error));
		status = eStatus::Stopped;
		RECT cr;
		GetClientRect(hwnd, &cr);
		InvalidateRect(hwnd, &cr, TRUE);
	
	}
	else
		SetTimer(hwnd, 0, 50, 0); // start prebuffer monitoring

}
bool bPlayer::FetchCover()
{
	CSocket soc;
	std::string req;
	req += "GET /2.0/?method=album.search&album=";
	req += url_encode(PlayingNow->Playing);
	req += "&api_key=c4eeb5aa39807b0d21d420ab64b42bf6&limit=1&page=1";
	req += " HTTP/1.1\r\nHost: ws.audioscrobbler.com\r\nConnection: Close\r\n\r\n";
	soc.Create(AF_INET,SOCK_STREAM, IPPROTO_TCP);
	ADDRINFOW Hints;
	ZeroMemory(&Hints, sizeof(ADDRINFO));
	Hints.ai_family = AF_UNSPEC;
	Hints.ai_socktype = SOCK_STREAM;
	Hints.ai_protocol = IPPROTO_TCP;
	ADDRINFOW *AddrInfo;

	int RetVal = GetAddrInfo(L"ws.audioscrobbler.com", L"80", &Hints, &AddrInfo);


	RetVal = soc.Connect(AddrInfo->ai_addr, (int)AddrInfo->ai_addrlen);
	soc.Send(req.c_str(), req.length(), NULL);
	char buffer[2048];
	memset(buffer,0,2048);
	int nDataLength,totalrec=0;
	while ((nDataLength = soc.Receive(buffer+totalrec, 2048-totalrec, 0)) > 0) {
		totalrec += nDataLength;
	};
	char *p = strstr(buffer, "\r\n\r\n");
	xml_document doc;
	xml_parse_result result = doc.load_buffer(p+4,strlen(p+4));
	if (result)
	{
		xml_node album= doc.child(L"lfm").child(L"results").child(L"albummatches").first_child();

		CoverUrl=album.find_child_by_attribute(L"size", L"large").text().as_string();

		if (CoverUrl.length() > 0)
			_beginthread(&bPlayer::StaticThreadEntry, 0, (void*)eThread::Downloadcover);
		else
		{
			CoverLoaded = false;
			RECT cr, r;
			GetClientRect(hwnd, &cr);
			SetRect(&r, 5, cr.bottom - 145, 145, cr.bottom - 5);
			InvalidateRect(hwnd, &r, TRUE);
		}
	}

	return true;
}

bool bPlayer::DownloadCover()
{
	CSocket soc;
	std::string host,get;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
	std::string u8str = conv1.to_bytes(CoverUrl);
	host = u8str;
	host.erase(0, u8str.find(":")+3);
	get = host;
	host.erase(host.find("/"),host.length());
	get.erase(0,get.find("/") );
	
	std::string req;
	req += "GET "+get;
	req += " HTTP/1.1\r\nHost: "+host+"\r\nConnection: keep-alive\r\n\r\n";

	soc.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ADDRINFOA Hints;
	ZeroMemory(&Hints, sizeof(ADDRINFO));
	Hints.ai_family = AF_UNSPEC;
	Hints.ai_socktype = SOCK_STREAM;
	Hints.ai_protocol = IPPROTO_TCP;
	ADDRINFOA *AddrInfo;


	int RetVal = GetAddrInfoA(host.c_str(), "80", &Hints, &AddrInfo);

	RetVal = soc.Connect(AddrInfo->ai_addr, (int)AddrInfo->ai_addrlen);
	soc.Send(req.c_str(), req.length(), NULL);
	char buffer[2048];
	memset(buffer, 0, 2048);
	int nDataLength, st, trec = 0, datalen = 0;
	CFile file;
	file.Open(L"cover.png", CFile::modeCreate | CFile::modeWrite);

	st = soc.Receive(buffer, 2048, 0);
	nDataLength = soc.Receive(buffer+st, 2048-st, 0);
	char *p = strstr(buffer, "\r\n\r\n") + 4;
	trec = nDataLength + st - (p - &buffer[0]);
	file.Write(p,trec);
	p=strstr(buffer, "Content-Length: ") +16 ;
	char *p2 = strstr(p, "\r\n");
	p2[0] = 0;
	datalen = atoi(p);
	memset(buffer, 0, 2048);
	for (;;) {
		if (trec>=datalen)
		{
			file.Close();
			break;
		}
		nDataLength = soc.Receive(buffer, 2048, 0);
		trec += nDataLength;
		file.Write(buffer, nDataLength);
		memset(buffer, 0, 2048);
	};

	CoverLoaded = true;
	RECT cr,r;
	GetClientRect(hwnd, &cr);
	SetRect(&r, 5, cr.bottom - 145, 145, cr.bottom - 5);
	InvalidateRect(hwnd, &r, TRUE);
	return true;
}

std::string bPlayer::url_encode(const std::wstring &input) {
	std::string output;
	int cbNeeded = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, NULL, 0, NULL, NULL);
	if (cbNeeded > 0) {
		char *utf8 = new char[cbNeeded];
		if (WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, utf8, cbNeeded, NULL, NULL) != 0) {
			for (char *p = utf8; *p; *p++) {
				char onehex[5];
				_snprintf(onehex, sizeof(onehex), "%%%02.2X", (unsigned char)*p);
				output.append(onehex);
			}
		}
		delete[] utf8;
	}
	return output;
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
void bPlayer::DownloadProc(const void *buffer, DWORD length, void *user)
{

}

void bPlayer::EndSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	wstring msg = L"EndSync : ";
	msg += PlayingNow->Name;
	bLog::AddLog(bLogEntry(msg, L"bPlayer", LogType::Info));
	status = eStatus::Stopped;
	RECT r;
	GetClientRect(hwnd, &r);
	InvalidateRect(hwnd, &r, TRUE);
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
				PlayingNow->Playing = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(pl);
				p = strstr(t, "-");
				if (p) {
					p[-1] = 0;
					PlayingNow->Artist = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(t);
					PlayingNow->Track = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(p+2);
				}

			}
		}
	}
	else {
		meta = (char *)BASS_ChannelGetTags(chan, BASS_TAG_OGG);
		if (meta) { // got Icecast/OGG tags
			char *artist = NULL, *title = NULL, *p = meta;
			for (; *p; p += strlen(p) + 1) {
				if (!_strnicmp(p, "artist=", 7)) // found the artist
					artist = p + 7;
				if (!_strnicmp(p, "title=", 6)) // found the title
					title = p + 6;
			}
			if (title) {
				
				PlayingNow->Track = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(title);
				if (artist) {
					PlayingNow->Artist = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(artist);
					PlayingNow->Playing= PlayingNow->Artist+L" - "+ PlayingNow->Track;
				}
 				else
					PlayingNow->Playing = PlayingNow->Track;
			}
		}
	}
	if (!PlayingNow->Artist.size() || !PlayingNow->Track.size())
		return;
	_beginthread(&bPlayer::StaticThreadEntry, 0, (void*)eThread::Fetchurl);
	wstring msg = L"Track : ";
	msg += PlayingNow->Playing;
	bLog::AddLog(bLogEntry(msg, L"bPlayer", LogType::Info));
	RECT r;
	GetClientRect(hwnd, &r);
	InvalidateRect(hwnd,&r, TRUE);
	
}

bPlayer::~bPlayer()
{
	BASS_StreamFree(chan);
	BASS_Free();
}