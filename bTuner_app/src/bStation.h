#ifndef BSTATION_H
#define BSTATION_H
#include <vector>

enum Codecs{MP3,AAC,OGG,MPEG,AACP,UNDIFINED};

class bStream
{
public:
	bStream(char *URL);
	char * Url;
	int Bitrate;
	Codecs Encoding;
};

class bStation
{
public:
	bStation();
	virtual ~bStation();
	char *Name;
	char *Genre;
	char *Url;
	char *Notice1;
	char *Notice2;
	int Public;
	int PlayedStreamID;
	char *Playing;
	char *Artist;
	char *Track;

	std::vector<bStream> Streams;

};

#endif