#include "bStation.h"

bStream::bStream(std::wstring URL)
{
	Url = URL;
	Bitrate = 0;
	Encoding = eCodecs::UNDIFINED;
}
bStation::bStation()
{
	int ID=-1;
	Name = L"";
	Genre = L"";
	Url = L"";
	Image = L"";
	Playing = L"";
	Artist = L"";
	Track = L"";
	PlayedStreamID = 0;
}

bool operator< (const bStation& l, const bStation& r)
{
	std::vector<std::wstring> data;
	data.push_back(l.Name);
	data.push_back(r.Name);
	std::sort(data.begin(), data.end());

	if (data[0] != l.Name)
		return false;
	else
		return true;
}
