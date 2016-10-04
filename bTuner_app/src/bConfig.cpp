#include "bConfig.h"


bConfig::bConfig()
{

}

bConfig::~bConfig()
{

}

bool bConfig::Load()
{
	bool Succeeded = false;

		CFile File;
		File.Open("Config.json", CFile::modeNoTruncate|CFile::modeRead);
		int len = File.GetLength();
		if (len > 0)
		{
			char *buf = new char(len + 1);
			buf[len] = 0;
			File.Read(buf, len);
			File.Close();
			std::string data(buf);
			j = json::parse(data.c_str());

			LastVolume = j["Session"]["Volume"];
			LastPlayedName = j["Session"]["Station"]["Name"].get<std::string>().c_str();
			LastPlayedUrl = j["Session"]["Station"]["Url"].get<std::string>().c_str();
			LastWindowPos.x = j["Session"]["Window"]["top"];
			LastWindowPos.y = j["Session"]["Window"]["left"];
		}
		else
			Default();

		Succeeded = true;


	return Succeeded;

}

bool bConfig::Save()
{
	bool Succeeded = false;


		CFile File;
		File.Open("Config.json", CFile::modeCreate | CFile::modeWrite);

			json j2;
			j2["Session"]["Volume"]= LastVolume;
			j2["Session"]["Station"]["Name"]= LastPlayedName;
			j2["Session"]["Station"]["Url"]= LastPlayedUrl;
			j2["Session"]["Window"]["top"]= LastWindowPos.x;
			j2["Session"]["Window"]["left"]= LastWindowPos.y;

			std::string data = j2.dump();
			File.Write(data.c_str(), data.length());

		File.Close();
		Succeeded = true;

	return Succeeded;

}


void bConfig::Default()
{
	LastVolume = 100;
	LastPlayedName = "";
	LastPlayedUrl = "";
	LastWindowPos.x = 0;
	LastWindowPos.y = 0;
}