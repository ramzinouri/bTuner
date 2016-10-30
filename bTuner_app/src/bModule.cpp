#include "bModule.h"

bModule::bModule() : Playlist(NULL), Loading(true)
{
}

bModule::~bModule()
{
}

bool bModule::UpdateStations()
{
	Loading = false;
	return false;
}

bool bModule::AddStation(bStation& station)
{
	Playlist->Stations.push_back(station);
	return true;
}

bool bModule::SaveStations()
{
	return false;
}

std::vector<unsigned int> bModule::Search(std::wstring query)
{
	return Playlist->Search(query);
}
