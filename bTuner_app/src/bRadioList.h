#ifndef BRADIOLIST_H
#define BRADIOLIST_H

#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"


class bRadioList : public CListBox
{
public:
	bRadioList();
	virtual ~bRadioList();
protected:
	void PreCreate(CREATESTRUCT &cs);
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

};

#endif //BCONFIG_H