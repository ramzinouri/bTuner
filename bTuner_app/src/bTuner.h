#ifndef BTUNER_H
#define BTUNER_H

#include "bTWin.h"
#include "bVersion.h"


class bTuner : public CWinApp
{
public:
	bTuner();
	virtual BOOL InitInstance();
	bool bDebug;
private:
	bTWin *_bTWin;


};

#endif