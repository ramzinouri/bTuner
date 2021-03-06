#ifndef BPLAYLIST_H
#define BPLAYLIST_H

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <locale>
#include <codecvt>
#include <vector>
#include <algorithm>


#include "bStation.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

class bPlaylist 
{
public:
	bPlaylist();
	~bPlaylist();
	bool ParsePLS(std::wstringstream* data);
	bool ParseXSPF(std::wstringstream* data);
	bool ParseXML(std::wstringstream* data);
	bool LoadFile(std::wstring path);
	bool SaveFile(std::wstring path);
	bool SaveXSPF(std::wstring path);
	bool SaveXML(std::wstring path);
	bool SavePLS(std::wstring path);
	std::vector<bStation> Stations;
	std::wstring title;
	int Locate(std::wstring name);
	std::vector<unsigned int> Search(std::wstring name);
	void Sort();

};

#endif //BPLAYLIST_H