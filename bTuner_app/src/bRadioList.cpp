#include"bRadioList.h"

bRadioList::bRadioList()
{

};

bRadioList::~bRadioList()
{

};

void bRadioList::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS;
};

LRESULT bRadioList::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		this->CloseWindow();
		break;
	}

	return WndProcDefault(uMsg, wParam, lParam);
}