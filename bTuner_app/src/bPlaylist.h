#ifndef BPLAYLIST_H
#define BPLAYLIST_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "bStation.h"

class bPlaylist 
{
public:
	bPlaylist();
	~bPlaylist();
	bool ParsePLS(std::wistringstream data);
	bool LoadFile(std::wstring path);
	bool SaveFile(std::wstring path);
	std::vector<bStation> Stations;
};

#endif //BPLAYLIST_H