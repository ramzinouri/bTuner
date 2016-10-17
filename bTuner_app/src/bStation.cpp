#include "bStation.h"

bStream::bStream(std::wstring URL)
{
	Url = URL;
	Bitrate = 0;
	Encoding = Codecs::UNDIFINED;
}
bStation::bStation()
{
	int ID=-1;
	Name = L"";
	Genre = L"";
	Url = L"";
	Playing = L"";
	Artist = L"";
	Track = L"";
	PlayedStreamID = 0;
}

bStation::~bStation()
{

}