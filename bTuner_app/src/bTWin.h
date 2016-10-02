#ifndef BTWIN_H
#define BTWIN_H


#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"
#include <stdio.h>

#include "resource.h"
#include "bPlayer.h"



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
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	bPlayer Player;
};

#endif