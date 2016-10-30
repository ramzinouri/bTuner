#ifndef BMODULESLIST_H
#define BMODULESLIST_H

#include <vector>

#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"
#include "Win32++\wxx_listview.h"

#include "bModule.h"
#include "bFavorites.h"
#include "bHistory.h"
#include "bXiphorg.h"
#include "bLog.h"

class bModulesList : public CListView
{
public:
	bModulesList();
	virtual ~bModulesList();
	void DrawItem(WPARAM wParam, LPARAM lParam);
	void OnCreate();
	std::map<std::wstring,bModule*> Modules;
	std::wstring ActiveModuleName;
	bModule *GetActiveModule();
	void SetActiveModule(std::wstring name);
protected:
	void PreCreate(CREATESTRUCT &cs);

};


#endif //BMODULESLIST_H