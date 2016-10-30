#ifndef BHISTORY_H
#define BHISTORY_H

#include "bModule.h"

class bHistory :public bModule
{
public:
	bHistory();
	~bHistory();
	bool UpdateStations();
	bool AddStation(bStation& station);
	bool SaveStations();
};


#endif //BHISTORY_H