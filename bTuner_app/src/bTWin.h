#ifndef BTWIN_H
#define BTWIN_H


#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"
#include "win32++\wxx_listview.h"
#include <stdio.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "resource.h"
#include "bPlayer.h"
#include "bConfig.h"


enum bHover { None, Play, Volume, CoverArt };

class bTWin : public CWnd
{
public:
	bTWin();
	virtual ~bTWin();
	INT_PTR AboutDiagproc(HWND h, UINT m, WPARAM w, LPARAM l);
	
protected:

	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void PreRegisterClass(WNDCLASS &wc);
	void PreCreate(CREATESTRUCT &cs);
	void OnDraw(CDC& dc);
	int  OnCreate(CREATESTRUCT& cs);
	void OnDestroy();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	bPlayer Player;
	void DrawPlayer(CDC& dc);

private:
	CRect VolumeRect;
	CRect PlayRect;
	int Hover;
	POINT Mouse,ClickP;
	BOOL Clicked;
	bConfig Config;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	CListBox bList;
};

#endif