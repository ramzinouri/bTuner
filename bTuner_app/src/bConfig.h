#ifndef BCONFIG_H
#define BCONFIG_H

#include "pugixml/pugixml.hpp"
using namespace pugi;

#include "win32++/wxx_wincore.h"
#include "bStation.h"


class bConfig
{
public:
	bool Load();
	bool Save();
	int LastVolume;
	CPoint LastWindowPos;
	bStation LastPlayed;
	bool LogWindow;
private:
	void Default();
	//char *buf;
	
};



#endif
