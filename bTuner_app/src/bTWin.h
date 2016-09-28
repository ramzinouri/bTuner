#ifndef BTWIN_H
#define BTWIN_H


#include "Win32++\wxx_wincore.h"
#include "Win32++\wxx_controls.h"

#include "resource.h"
#include <GdiPlus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")


class bTWin : public CWnd
{
public:
	bTWin();
	virtual ~bTWin();
protected:
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void PreRegisterClass(WNDCLASS &wc);
	void PreCreate(CREATESTRUCT &cs);
	void OnDraw(CDC& dc);
private:
	ULONG_PTR	m_gdiplusToken;
};

#endif