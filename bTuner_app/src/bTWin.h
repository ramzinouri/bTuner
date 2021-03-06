#ifndef BTWIN_H
#define BTWIN_H

#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"

#include <stdio.h>
#include <gdiplus.h>

using namespace Gdiplus;
#include "bVersion.h"
#include "resource.h"
#include "bPlayer.h"
#include "bConfig.h"
#include "bRadioList.h"
#include "bPlaylist.h"
#include "bLog.h"
#include "bModulesList.h"

enum bHover { None, Play, Volume, CoverArt, Link };

class bTWin : public CWnd
{
	
public:
	bTWin();
	virtual ~bTWin();
	INT_PTR Diagproc(HWND h, UINT m, WPARAM w, LPARAM l);
	
protected:
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void PreRegisterClass(WNDCLASS &wc);
	void PreCreate(CREATESTRUCT &cs);
	void OnDraw(HDC dc);
	int  OnCreate(CREATESTRUCT& cs);
	void OnDestroy();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	bPlayer Player;
	//bPlaylist* Playlist;
	//bPlaylist* Favorites;
	void DrawPlayer(HDC dc);
	void  OnClose();
	void OnTimer(int TimerID);
	BOOL OnEraseBkgnd(CDC& dc);
	void OnContextMenu(HWND wnd, CPoint point);
	void OnMenuCommand(HMENU menu, int idx);
private:
	CRect VolumeRect;
	CRect PlayRect;
	CRect LinkRect;
	int Hover;
	POINT Mouse,ClickP;
	BOOL Clicked;
	bConfig Config;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	bRadioList bList;
	CEdit searchbox;
	bModulesList Modulelist;
	HMENU StreamMenu;
	HMENU ContextMenu;
};

#endif