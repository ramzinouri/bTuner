#ifndef BPLAYER_H
#define BPLAYER_H

#include "bass/bass.h"
#include "bass/bass_aac.h"
#include <iostream>
#include <thread>
#include <process.h>

#include "bStation.h"

enum Status {Playing,Stoped,Connecting,Buffring};

class bPlayer 
{
public:
	static HWND hwnd;
	static HSTREAM chan;
	static Status status;
	static bStation *PlayingNow;
	int BuffProgress;
	bPlayer();
	virtual ~bPlayer();
	int Play();
	void Stop();
	void Resume();
	void OpenURL(char *URL);
	void OpenThread();
	static void StaticThreadEntry(void* c);
	int GetVolume();
	void SetVolume(int Vol);

	static void CALLBACK DownloadProc(const void *buffer, DWORD length, void *user);
	static void CALLBACK MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user);

};

#endif