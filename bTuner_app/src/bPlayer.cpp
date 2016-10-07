#include "bPlayer.h"




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


bPlayer::bPlayer()
{
	gp_bPlayer = this;
	chan = NULL;
	hwnd = NULL;
	Volume = 100;
	PlayingNow = NULL;
	status = eStatus::Stoped;
	if (!BASS_Init(-1, 44100, BASS_DEVICE_STEREO, hwnd, NULL)) {
		MessageBoxA(NULL,"Can't initialize device","ERROR",MB_OK);
	}
	BASS_PluginLoad("bass_aac.dll", 0);
	BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1); // enable playlist processing
	BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0); // minimize automatic pre-buffering, so we can do it (and display it) instead
	BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, (void*)NULL);

}

int bPlayer::Play()
{
	return BASS_ChannelPlay(chan, FALSE);
}

void bPlayer::Stop()
{
	if(BASS_ChannelStop(chan))
		status=eStatus::Stoped;
}
void bPlayer::Resume()
{
	_beginthread(&bPlayer::StaticThreadEntry, 0, (void*)eThread::Openurl);
}

void bPlayer::OpenURL(char *URL)
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
	status = eStatus::Connecting;
	BASS_StreamFree(chan);
	chan = BASS_StreamCreateURL((char*)PlayingNow->Streams[PlayingNow->PlayedStreamID].Url, 0, BASS_STREAM_BLOCK | BASS_STREAM_STATUS | BASS_STREAM_AUTOFREE, NULL, 0);
	SetVolume(Volume);
	if (!chan)  // failed to open
		MessageBoxA(NULL, "Can't open stream", "ERROR", MB_OK);
	else
		SetTimer(hwnd, 0, 50, 0); // start prebuffer monitoring

}
bool bPlayer::FetchCover()
{
	CSocket soc;
	std::string req;
	req += "GET /2.0/?method=album.search&album=";
	string pl = PlayingNow->Artist;
	pl += " ";
	pl+=PlayingNow->Track;
	req += url_encode(pl);
	req+="&api_key=c4eeb5aa39807b0d21d420ab64b42bf6&limit=1&page=1";
	req += " HTTP/1.1\r\nHost: ws.audioscrobbler.com\r\nConnection: Close\r\n\r\n";
	soc.Create(AF_INET,SOCK_STREAM, IPPROTO_TCP);
	ADDRINFO Hints;
	ZeroMemory(&Hints, sizeof(ADDRINFO));
	Hints.ai_family = AF_UNSPEC;
	Hints.ai_socktype = SOCK_STREAM;
	Hints.ai_protocol = IPPROTO_TCP;
	ADDRINFO *AddrInfo;

	CString csPort;
	csPort.Format(_T("%u"), 80);
	int RetVal = GetAddrInfo("ws.audioscrobbler.com", csPort, &Hints, &AddrInfo);


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
		xml_node album= doc.child("lfm").child("results").child("albummatches").first_child();
		CoverUrl=album.find_child_by_attribute("size", "large").text().as_string();
		if(CoverUrl.length()>0)
			_beginthread(&bPlayer::StaticThreadEntry, 0, (void*)eThread::Downloadcover);
	}

	return true;
}

bool bPlayer::DownloadCover()
{
	CSocket soc;
	std::string host=CoverUrl,get;
	host.erase(0,CoverUrl.find(":")+3);
	get = host;
	host.erase(host.find("/"),host.length());
	get.erase(0,get.find("/") );
	
	std::string req;
	req += "GET "+get;
	req += " HTTP/1.1\r\nHost: "+host+"\r\nConnection: keep-alive\r\n\r\n";
	soc.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ADDRINFO Hints;
	ZeroMemory(&Hints, sizeof(ADDRINFO));
	Hints.ai_family = AF_UNSPEC;
	Hints.ai_socktype = SOCK_STREAM;
	Hints.ai_protocol = IPPROTO_TCP;
	ADDRINFO *AddrInfo;

	CString csPort;
	csPort.Format(_T("%u"), 80);
	int RetVal = GetAddrInfo(host.c_str(), csPort, &Hints, &AddrInfo);

	RetVal = soc.Connect(AddrInfo->ai_addr, (int)AddrInfo->ai_addrlen);
	soc.Send(req.c_str(), req.length(), NULL);
	char buffer[2048];
	memset(buffer, 0, 2048);
	int nDataLength, st, trec = 0, datalen = 0;
	CFile file;
	file.Open("cover.png", CFile::modeCreate | CFile::modeWrite);
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

	//ShellExecute(NULL, "open", CoverUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
	CoverLoaded = true;
	RECT cr,r;
	GetClientRect(hwnd, &cr);
	SetRect(&r, 5, cr.bottom - 145, 145, cr.bottom - 5);
	InvalidateRect(hwnd, &r, TRUE);
	return true;
}

std::string bPlayer::url_encode(const std::string &value) {
	std::ostringstream escaped;
	escaped.fill('0');
	escaped << hex;

	for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		std::string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << uppercase;
		escaped << '%' << setw(2) << int((unsigned char)c);
		escaped << nouppercase;
	}

	return escaped.str();
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

void CALLBACK bPlayer::MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user)
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
				PlayingNow->Playing = pl;
				
				p = strstr(t, "-");
				if (p) {
					p[-1] = 0;
					PlayingNow->Artist = t;
					PlayingNow->Track = p + 2;
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
				PlayingNow->Track = title;
				if (artist) {
					PlayingNow->Artist = artist;
					char text[100];
					memset(text, 0, 100);
					_snprintf_s(text, sizeof(text), "%s - %s", artist, title);
					char *pl = new char[strlen(text) + 1];
					memcpy(pl, text, strlen(text) + 1);
					PlayingNow->Playing=pl;
				}
 				else
					PlayingNow->Playing = title;
			}
		}
	}
	_beginthread(&bPlayer::StaticThreadEntry, 0, (void*)eThread::Fetchurl);
	CoverLoaded = false;
	RECT r;
	GetClientRect(hwnd, &r);
	InvalidateRect(hwnd,&r, TRUE);
	
}

bPlayer::~bPlayer()
{
	BASS_StreamFree(chan);
	BASS_Free();
}