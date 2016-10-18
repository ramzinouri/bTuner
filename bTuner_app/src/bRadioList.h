#ifndef BRADIOLIST_H
#define BRADIOLIST_H

#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"
#include "Win32++\wxx_listview.h"


class bRadioList : public CListView
{
public:
	bRadioList();
	virtual ~bRadioList();
	void DrawItem(WPARAM wParam, LPARAM lParam);
	void OnCreate();
protected:
	void PreCreate(CREATESTRUCT &cs);

};

#endif //BCONFIG_H