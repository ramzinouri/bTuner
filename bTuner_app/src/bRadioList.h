#ifndef BRADIOLIST_H
#define BRADIOLIST_H

#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"


class bRadioList : public CListBox
{
public:
	bRadioList();
	virtual ~bRadioList();
	void DrawItem(WPARAM wParam, LPARAM lParam);
protected:
	void PreCreate(CREATESTRUCT &cs);
	void OnDraw(CDC& dc);
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	bool firstdraw;

};

#endif //BCONFIG_H