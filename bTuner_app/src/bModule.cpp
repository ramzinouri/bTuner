#include "bModule.h"

bModule::bModule() : Playlist(NULL)
{
}

bModule::~bModule()
{
}

bool bModule::UpdateStations()
{
	return false;
}

std::vector<unsigned int> bModule::Search(std::wstring query)
{
	return Playlist->Search(query);
}
