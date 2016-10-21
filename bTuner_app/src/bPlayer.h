#ifndef BPLAYER_H
#define BPLAYER_H
#include "winsock2.h"
#include "bass/bass.h"
#include "bass/bass_aac.h"
#include "bass/bassopus.h"
#include <iostream>
#include <thread>
#include <process.h>
#include <cctype>
#include <iomanip>


#include "pugixml/pugixml.hpp"
using namespace pugi;

#include "win32++/wxx_socket.h"
#include "win32++/wxx_file.h"

using namespace std;
using namespace Win32xx;

#include "bStation.h"
#include "bString.h"
#include "bHttp.h"

enum eStatus { Playing,Stopped,Connecting,Buffering};
enum eThread { Openurl,Fetchurl,Downloadcover};
void CALLBACK  g_MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user);
void CALLBACK g_DownloadProc(const void *buffer, DWORD length, void *user);
void CALLBACK g_EndSync(HSYNC handle, DWORD channel, DWORD data, void *user);

class bPlayer 
{
public:
	HWND hwnd;
	HSTREAM chan;
	eStatus status;
	bStation *PlayingNow;
	HANDLE  hThreadArray[3];
	unsigned threadID[3];
	bool CoverLoaded;
	int BuffProgress;
	bPlayer();
	virtual ~bPlayer();
	int Play();
	void Stop();
	void Resume();
	void OpenURL(std::wstring URL);
	void OpenStation(const bStation& Station);
	int GetVolume();
	void SetVolume(int Vol);
	void UpdateWnd();
	void ProcessTags(const char *buffer);
	void DownloadProc(const void *buffer, DWORD length, void *user);
	void MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user);
	void EndSync(HSYNC handle, DWORD channel, DWORD data, void *user);
private:
	void OpenThread();
	bool FetchCover();
	bool DownloadCover();
	static unsigned __stdcall StaticThreadEntry(void* c);
	int Volume;
	std::string url_encode(const std::wstring &input);
	std::wstring CoverUrl;
};

#endif