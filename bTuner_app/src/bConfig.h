#ifndef BCONFIG_H
#define BCONFIG_H
#include "JSON/json.hpp"
#include "win32++/wxx_wincore.h"
#include "win32++/wxx_file.h"

using json = nlohmann::json;

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
	json j;
};



#endif
