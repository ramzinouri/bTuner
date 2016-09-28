#ifndef BTUNER_H
#define BTUNER_H

#include "Win32++\wxx_wincore.h"
#include "API/TuneIn.h"
#include "bTWin.h"
#include "bLogWin.h"


class bTuner : public CWinApp
{
public:
	bTuner();
	virtual ~bTuner();
	virtual BOOL InitInstance();
	bool bDebug;
private:
	bTWin *_bTWin;
	bLogWin *_bLogWin;

};

#endif