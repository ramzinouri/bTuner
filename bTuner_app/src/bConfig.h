#ifndef BCONFIG_H
#define BCONFIG_H
#include "JSON/json.hpp"

using json = nlohmann::json;

class bConfig
{
public:
	bConfig();
	virtual ~bConfig();
};



#endif
