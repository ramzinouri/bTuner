#ifndef BPLAYER_H
#define BPLAYER_H
#include "winsock2.h"
#include "bass/bass.h"
#include "bass/bass_aac.h"
#include <iostream>
#include <thread>
#include <process.h>
#include <cctype>
#include <iomanip>
#include <string>
#include <locale>
#include <codecvt>

#include "pugixml/pugixml.hpp"
using namespace pugi;

#include "win32++/wxx_socket.h"
#include "win32++/wxx_file.h"

using namespace std;
using namespace Win32xx;

#include "bStation.h"

enum eStatus { Playing,Stopped,Connecting,Buffring};
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
	std::wstring CoverUrl;
	bool CoverLoaded;
	int BuffProgress;
	bPlayer();
	virtual ~bPlayer();
	int Play();
	void Stop();
	void Resume();
	void OpenURL(std::wstring URL);
	void OpenThread();
	static void StaticThreadEntry(void* c);
	int GetVolume();
	void SetVolume(int Vol);
	bool FetchCover();
	bool DownloadCover();
	void DownloadProc(const void *buffer, DWORD length, void *user);
	void MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user);
	void EndSync(HSYNC handle, DWORD channel, DWORD data, void *user);
private:
	int Volume;
	std::string url_encode(const std::wstring &input);
	std::wostringstream playlistbuffer;
};

#endif