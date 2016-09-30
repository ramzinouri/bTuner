#include "bStation.h"

bStream::bStream(char *URL)
{
	Url = URL;
	Bitrate = 0;
	Encoding = Codecs::UNDIFINED;
}
bStation::bStation()
{
	PlayedStreamID = 0;
}

bStation::~bStation()
{

}