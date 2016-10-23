#ifndef BLOGWIN_H
#define BLOGWIN_H


#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"
#include "Win32++\wxx_listview.h"
#include "resource.h"

class bLogList : public CListView
{
public:
	bLogList();
	virtual ~bLogList();
	void DrawItem(WPARAM wParam, LPARAM lParam);
	void OnCreate();
protected:
	void PreCreate(CREATESTRUCT &cs);

};


class bLogWin : public CWnd
{
public:
	bLogWin();
	virtual ~bLogWin();
	void UpdateLog();
	void AddLog(int pos, std::wstring _log);
protected:
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void PreRegisterClass(WNDCLASS &wc);
	void PreCreate(CREATESTRUCT &cs);
	int  OnCreate(CREATESTRUCT& cs);
	bLogList _bLogList;
};



#endif