#ifndef BFAVORITES_H
#define BFAVORITES_H

#include "bModule.h"

class bFavorites :public bModule
{
public:
	bFavorites();
	~bFavorites();
	bool UpdateStations();
	bool AddStation(bStation& station);
	bool SaveStations();
};


#endif //BFAVORITES_H