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
	static Status status;
	static bStation *PlayingNow;
	bPlayer();
	virtual ~bPlayer();
	int Play();
	void Stop();
	void OpenURL(char *URL);
	void OpenThread();
	static void StaticThreadEntry(void* c);


private:
	HSTREAM chan;
	char * surl;
	static void CALLBACK DownloadProc(const void *buffer, DWORD length, void *user);

};

#endif