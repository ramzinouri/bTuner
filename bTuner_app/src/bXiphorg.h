#ifndef BXIPHORG_H
#define BXIPHORG_H

#include "bModule.h"

class bXiphorg :public bModule
{
public:
	bXiphorg();
	~bXiphorg();
	bool UpdateStations();
	void UpdateThread();
};


#endif //BXIPHORG_H