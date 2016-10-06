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
		Json::Value jRoot;
		Json::Reader jReader;
		File.Open("Config.json", CFile::modeNoTruncate|CFile::modeRead);
		int len = File.GetLength();
		if (len > 0)
		{
			buf = new char(len + 1);
			buf[len] = 0;
			File.Read(buf, len);
			File.Close();
			std::string data(buf);
			jReader.parse(buf,jRoot);

			LastVolume = jRoot["Session"]["Volume"].asInt();
			LastPlayedName = jRoot["Session"]["Station"]["Name"].asString();
			LastPlayedUrl = jRoot["Session"]["Station"]["Url"].asString();
			LastWindowPos.x = jRoot["Session"]["Window"]["top"].asInt();
			LastWindowPos.y = jRoot["Session"]["Window"]["left"].asInt();
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
		Json::Value jRoot;
		Json::FastWriter jWriter;
		File.Open("Config.json", CFile::modeCreate | CFile::modeWrite);

		jRoot["Session"]["Volume"]= LastVolume;
		jRoot["Session"]["Station"]["Name"]= LastPlayedName;
		jRoot["Session"]["Station"]["Url"]= LastPlayedUrl;
		jRoot["Session"]["Window"]["top"]= LastWindowPos.x;
		jRoot["Session"]["Window"]["left"]= LastWindowPos.y;

		std::string data = jWriter.write(jRoot);
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