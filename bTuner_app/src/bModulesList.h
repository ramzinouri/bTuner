#ifndef BMODULESLIST_H
#define BMODULESLIST_H

#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"
//#include "Win32++\wxx_listview.h"


class bModulesList : public CListBox
{
public:
	bModulesList();
	virtual ~bModulesList();
	void DrawItem(WPARAM wParam, LPARAM lParam);
	void OnCreate();

protected:
	void PreCreate(CREATESTRUCT &cs);

};


#endif //BMODULESLIST_H