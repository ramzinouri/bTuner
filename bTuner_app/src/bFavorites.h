#ifndef BFAVORITES_H
#define BFAVORITES_H

#include "bModule.h"

class bFavorites :public bModule
{
public:
	bFavorites();
	~bFavorites();
	bool UpdateStations();
};


#endif //BFAVORITES_H