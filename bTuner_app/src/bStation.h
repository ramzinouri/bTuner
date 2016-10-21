#ifndef BSTATION_H
#define BSTATION_H
#include <vector>
#include <algorithm>

enum eCodecs{MP3,AAC,OGG,MPEG,AACP,OPUS,NSV,UNDIFINED};

class bStream
{
public:
	bStream(std::wstring URL);
	std::wstring  Url;
	unsigned int Bitrate;
	eCodecs Encoding;
};

class bStation
{
public:
	bStation();
	std::wstring Name;
	std::wstring Genre;
	std::wstring Url;
	std::wstring Image;
	unsigned int PlayedStreamID;
	std::wstring Playing;
	std::wstring Artist;
	std::wstring Track;
	unsigned int ID;
	std::vector<bStream> Streams;
	friend bool operator < (const bStation& l, const bStation& r);
};

#endif