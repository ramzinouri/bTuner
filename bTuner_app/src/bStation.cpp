#include "bStation.h"

bStream::bStream(std::wstring URL)
{
	Url = URL;
	Bitrate = 0;
	Encoding = Codecs::UNDIFINED;
}
bStation::bStation()
{

	Name = L"";
	Genre = L"";
	Url = L"";
	Notice1 = L"";
	Notice2 =L"";
	Public = 0;
	Playing = L"";
	Artist = L"";
	Track = L"";
	PlayedStreamID = 0;
}

bStation::~bStation()
{

}