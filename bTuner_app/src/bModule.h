#ifndef BMODULE_H
#define BMODULE_H

#include "bPlaylist.h"
#include "bRadioList.h"

class bModule
{
public:
	bModule();
	virtual ~bModule();
	virtual bool UpdateStations();
	virtual bool AddStation(bStation& station);
	virtual bool SaveStations();
	virtual std::vector<unsigned int> Search(std::wstring query);
	std::wstring Name;
	bPlaylist *Playlist;
	bool Loading;
};


#endif //BMODULE_H