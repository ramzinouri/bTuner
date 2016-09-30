#ifndef BTWIN_H
#define BTWIN_H


#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"

#include "resource.h"
#include "bPlayer.h"


class bTWin : public CWnd
{
public:
	bTWin();
	virtual ~bTWin();
	
protected:
	static INT_PTR _stdcall  AboutDiagproc(HWND h, UINT m, WPARAM w, LPARAM l);
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void PreRegisterClass(WNDCLASS &wc);
	void PreCreate(CREATESTRUCT &cs);
	void OnDraw(CDC& dc);
	int  OnCreate(CREATESTRUCT& cs);
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	bPlayer Player;
private:
	ULONG_PTR	m_gdiplusToken;
	CMenu *MainMenu;
};

#endif