#include "bPlayer.h"


Status bPlayer::status = Status::Stoped;
bStation *bPlayer::PlayingNow = NULL;
HSTREAM bPlayer::chan = NULL;
HWND bPlayer::hwnd = NULL;


bPlayer::bPlayer()
{
	PlayingNow = NULL;
	status = Status::Stoped;
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
		status=Status::Stoped;
}
void bPlayer::Resume()
{
	_beginthread(&bPlayer::StaticThreadEntry, 0, this);
}

void bPlayer::OpenURL(char *URL)
{
	if (PlayingNow)
		delete PlayingNow;
	PlayingNow = new bStation;
	PlayingNow->Streams.push_back(bStream(URL));
	_beginthread(&bPlayer::StaticThreadEntry, 0, this);
	
}

void bPlayer::StaticThreadEntry(void* c)
{
	bPlayer* p = static_cast<bPlayer*>(c);
	return p->OpenThread();
}


void bPlayer::OpenThread()
{
	status = Status::Connecting;
	int v = GetVolume();
	BASS_StreamFree(chan);
	chan = BASS_StreamCreateURL((char*)PlayingNow->Streams[PlayingNow->PlayedStreamID].Url, 0, BASS_STREAM_BLOCK | BASS_STREAM_STATUS | BASS_STREAM_AUTOFREE, DownloadProc, 0);
	if(v)
		SetVolume(v);
	if (!chan)  // failed to open
		MessageBoxA(NULL, "Can't open stream", "ERROR", MB_OK);
	else
		SetTimer(hwnd, 0, 50, 0); // start prebuffer monitoring

}

int bPlayer::GetVolume()
{
	float vol = 0;
	BASS_ChannelGetAttribute(chan, BASS_ATTRIB_VOL, &vol);
	return (vol * 100);
}

void bPlayer::SetVolume(int Vol)
{
	if (Vol > 100)
		Vol = 100;
	if (Vol < 0)
		Vol = 0;
	float v = Vol/100.0;
	BASS_ChannelSetAttribute(chan, BASS_ATTRIB_VOL, v);
}

void CALLBACK bPlayer::DownloadProc(const void *buffer, DWORD length, void *user)
{

	//if (length)
	//	status = Status::Buffring;
}

void CALLBACK bPlayer::MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{	
	char *meta = (char *)BASS_ChannelGetTags(chan, BASS_TAG_META);
	if (meta) { // got Shoutcast metadata
		char *p = strstr(meta, "StreamTitle='"); // locate the title
		if (p) {
			const char *p2 = strstr(p, "';"); // locate the end of it
			if (p2) {
				char *t = strdup(p + 13);
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
				if (!strnicmp(p, "artist=", 7)) // found the artist
					artist = p + 7;
				if (!strnicmp(p, "title=", 6)) // found the title
					title = p + 6;

			}
			if (title) {
				PlayingNow->Track = title;
				if (artist) {
					PlayingNow->Artist = artist;
					char text[100];
					memset(text, 0, 100);
					_snprintf(text, sizeof(text), "%s - %s", artist, title);
					char *pl = new char[strlen(text) + 1];
					memcpy(pl, text, strlen(text) + 1);
					PlayingNow->Playing=pl;
				}
 				else
					PlayingNow->Playing = title;
			}
		}
	}
	RECT r;
	GetClientRect(hwnd, &r);
	InvalidateRect(hwnd,&r, TRUE);
}

bPlayer::~bPlayer()
{
	BASS_StreamFree(chan);
	BASS_Free();
}