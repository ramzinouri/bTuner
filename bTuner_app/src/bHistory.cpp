#include "bHistory.h"


bHistory::bHistory() 
{
	Name = L"History";
}

bHistory::~bHistory()
{
	if (Playlist)
		delete Playlist;
}

bool bHistory::UpdateStations()
{
	if (!Playlist)
		Playlist = new bPlaylist;
	Playlist->title = L"History";

	Playlist->LoadFile(L"bHistory.xml");

	return true;
}

bool bHistory::AddStation(bStation & station)
{
	if (Playlist->Stations.size())
	{
		if (Playlist->Stations.at(Playlist->Stations.size() - 1).Name != station.Name)
		{
			Playlist->Stations.push_back(station);
			if (Playlist->Stations.size() > 8)
				Playlist->Stations.erase(Playlist->Stations.begin());
		}
		else
			Playlist->Stations.at(Playlist->Stations.size() - 1).PlayedStreamID = station.PlayedStreamID;
	}
	else
		Playlist->Stations.push_back(station);
	
	SaveStations();
	UpdateStations();
	return true;
}

bool bHistory::SaveStations()
{
	return Playlist->SaveFile(L"bHistory.xml");
}
