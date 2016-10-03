#include "bStation.h"

bStream::bStream(char *URL)
{
	Url = URL;
	Bitrate = 0;
	Encoding = Codecs::UNDIFINED;
}
bStation::bStation()
{

	Name = NULL;
	Genre = NULL;
	Url = NULL;
	Notice1 = NULL;
	Notice2 = NULL;
	Public = 0;
	Playing = NULL;
	Artist = 0;
	Track = 0;
	PlayedStreamID = 0;
}

bStation::~bStation()
{

}