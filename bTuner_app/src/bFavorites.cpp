#include "bFavorites.h"


bFavorites::bFavorites()
{

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
	return Playlist->LoadFile(L"bFavorites.xspf");
}
