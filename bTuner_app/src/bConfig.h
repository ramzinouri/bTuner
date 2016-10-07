#ifndef BCONFIG_H
#define BCONFIG_H

#include "pugixml/pugixml.hpp"
using namespace pugi;

#include "win32++/wxx_wincore.h"
#include "win32++/wxx_file.h"



class bConfig
{
public:
	bConfig();
	virtual ~bConfig();
	bool Load();
	bool Save();
	void Default();
	int LastVolume;
	CPoint LastWindowPos;
	std::string LastPlayedName;
	std::string LastPlayedUrl;
	char *buf;
	
};



#endif
