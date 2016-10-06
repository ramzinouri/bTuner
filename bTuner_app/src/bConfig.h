#ifndef BCONFIG_H
#define BCONFIG_H
#define JSON_IS_AMALGAMATION 1
#include "JSON/json.h" // 0.6 need to upgrade it

#include "win32++/wxx_wincore.h"
#include "win32++/wxx_file.h"
#include <fstream>


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
