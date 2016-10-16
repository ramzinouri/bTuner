#ifndef BCONFIG_H
#define BCONFIG_H

#include "pugixml/pugixml.hpp"
using namespace pugi;

#include "win32++/wxx_wincore.h"


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
	std::wstring LastPlayedName;
	std::wstring LastPlayedUrl;
	char *buf;
	
};



#endif
