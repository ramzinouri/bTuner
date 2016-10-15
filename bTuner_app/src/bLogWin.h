#ifndef BLOGWIN_H
#define BLOGWIN_H


#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"
#include "resource.h"

class bLogList : public CListBox
{
public:
	bLogList();
	virtual ~bLogList();
protected:
	void PreCreate(CREATESTRUCT &cs);

};


class bLogWin : public CWnd
{
public:
	bLogWin();
	virtual ~bLogWin();
	void UpdateLog();
protected:
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void PreRegisterClass(WNDCLASS &wc);
	void PreCreate(CREATESTRUCT &cs);
	int  OnCreate(CREATESTRUCT& cs);
	bLogList _bLogList;
};



#endif