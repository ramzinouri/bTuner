#ifndef BRADIOLIST_H
#define BRADIOLIST_H

#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"
#include "Win32++\wxx_listview.h"

#include "bStation.h"


class bRadioList : public CListView
{
public:
	bRadioList();
	virtual ~bRadioList();
	void DrawItem(WPARAM wParam, LPARAM lParam);
	void OnCreate();
	void AddStation(const bStation& station);
protected:
	void PreCreate(CREATESTRUCT &cs);
};

#endif //BCONFIG_H