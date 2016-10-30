#include "bFavorites.h"


bFavorites::bFavorites()
{
	Name = L"Favorites";
}

bFavorites::~bFavorites()
{
	if (Playlist)
		delete Playlist;
}

bool bFavorites::UpdateStations()
{
	if (!Playlist)
		Playlist = new bPlaylist;
	Playlist->LoadFile(L"bFavorites.xml");
	return true;
}

bool bFavorites::AddStation(bStation & station)
{
	if (Playlist->Locate(station.Name)<0)
		Playlist->Stations.push_back(station);
	SaveStations();
	UpdateStations();
	return true;
}

bool bFavorites::SaveStations()
{
	return Playlist->SaveFile(L"bFavorites.xml");
}