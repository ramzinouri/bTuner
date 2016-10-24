#ifndef BRADIOLIST_H
#define BRADIOLIST_H

#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"
#include "Win32++\wxx_listview.h"
#include <thread>
#include <process.h>
#include <mutex>
#include <condition_variable>

#include "bStation.h"
#include "bPlaylist.h"


enum eRadioThread { DrawOnly,RedrawPlaylist };



class bRadioList : public CListView
{
public:
	bRadioList();
	virtual ~bRadioList();
	void DrawItem(WPARAM wParam, LPARAM lParam);
	void OnCreate();
	void AddStation(const bStation& station);
	void RedrawPlaylist();
	void DrawOnly(std::vector<unsigned int> items);
	void T_RedrawPlaylist();
	void T_DrawOnly();
	int PlayingNowID;
	bPlaylist* Playlist;
	static unsigned __stdcall StaticThreadEntry(void* c);
	
protected:
	void PreCreate(CREATESTRUCT &cs);
	HANDLE  hThread;
	unsigned threadID;
	std::vector<unsigned int> m_items;
	static bool killthread;
	std::mutex m;
	std::condition_variable cv;
};


#endif //BCONFIG_H