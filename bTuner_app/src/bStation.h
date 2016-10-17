#ifndef BSTATION_H
#define BSTATION_H
#include <vector>

enum Codecs{MP3,AAC,OGG,MPEG,AACP,UNDIFINED};

class bStream
{
public:
	bStream(std::wstring URL);
	std::wstring  Url;
	int Bitrate;
	Codecs Encoding;
};

class bStation
{
public:
	bStation();
	virtual ~bStation();
	std::wstring Name;
	std::wstring Genre;
	std::wstring Url;
	int PlayedStreamID;
	std::wstring Playing;
	std::wstring Artist;
	std::wstring Track;
	int ID;
	std::vector<bStream> Streams;
};

#endif