#include "bPlaylist.h"

bPlaylist::bPlaylist()
{

}

bPlaylist::~bPlaylist()
{

}


bool bPlaylist::ParsePLS(std::wistringstream data)
{
	bool result = false;
	std::wstring line;
	std::getline(data, line);
	if (line.find(L"[playlist]") == std::wstring::npos)
		return false;

	return result;
}

bool bPlaylist::SaveFile(std::wstring path)
{
	bool result = false;


	return result;
}

bool bPlaylist::LoadFile(std::wstring path)
{
	bool result = false;


	return result;
}
