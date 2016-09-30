#include "bPlayer.h"


Status bPlayer::status = Status::Stoped;
bStation *bPlayer::PlayingNow = NULL;

bPlayer::bPlayer()
{
	status = Status::Stoped;
	if (!BASS_Init(-1, 44100, 0, NULL, NULL)) {
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
	BASS_StreamFree(chan);
	chan = BASS_StreamCreateURL((char*)PlayingNow->Streams[PlayingNow->PlayedStreamID].Url, 0, BASS_STREAM_BLOCK | BASS_STREAM_STATUS | BASS_STREAM_AUTOFREE, DownloadProc, 0);
	
	if (!chan)  // failed to open
		MessageBoxA(NULL, "Can't open stream", "ERROR", MB_OK);
	Play();
}

void CALLBACK bPlayer::DownloadProc(const void *buffer, DWORD length, void *user)
{
	char *stype;
	char *uuu = (char*)buffer;
	if (buffer && !length)
	{
		for (; *uuu; uuu += strlen(uuu) + 1) {
			if (!strnicmp(uuu, "icy-name:", 9))
				PlayingNow->Name = (char*)uuu + 9;
			if (!strnicmp(uuu, "icy-url:", 8))
				PlayingNow->Url = (char*)uuu + 8;
			if (!strnicmp(uuu, "icy-br:", 7))
				PlayingNow->Streams[PlayingNow->PlayedStreamID].Bitrate = atoi(uuu + 7);
			if (!strnicmp(uuu, "icy-genre:", 10))
				PlayingNow->Genre = (char*)uuu + 10;

			if (!strnicmp(uuu, "icy-notice1:", 12))
				PlayingNow->Notice1 = (char*)uuu + 12;
			if (!strnicmp(uuu, "icy-notice2:", 12))
				PlayingNow->Notice2 = (char*)uuu + 12;
			if (!strnicmp(uuu, "icy-pub:", 8))
				PlayingNow->Public = atoi(uuu + 8);
			if (!strnicmp(uuu, "Content-Type:", 13))
			{
				stype = (char*)uuu + 14;
				if (!strnicmp(stype, "audio/mp3", 9))
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Encoding = Codecs::MP3;
				if (!strnicmp(stype, "audio/aac", 9))
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Encoding = Codecs::AAC;
				if (!strnicmp(stype, "audio/aacp", 10))
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Encoding = Codecs::AACP;
				if (!strnicmp(stype, "audio/ogg", 9))
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Encoding = Codecs::OGG;
				if (!strnicmp(stype, "audio/mpeg", 10))
					PlayingNow->Streams[PlayingNow->PlayedStreamID].Encoding = Codecs::MPEG;
			}
		}
	}
	if (length)
		status = Status::Buffring;
}

bPlayer::~bPlayer()
{
	BASS_StreamFree(chan);
	BASS_Free();
}