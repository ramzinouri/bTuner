#include "bConfig.h"

bool bConfig::Load()
{
	bool Succeeded = false;

	xml_document doc;
	xml_parse_result result = doc.load_file("bTuner.xml");
	
		if (result)
		{
			xml_node nSession = doc.child(L"bTuner").child(L"Session");
			LastVolume = nSession.child(L"Volume").text().as_int();
			LastPlayed.Name = nSession.child(L"Station").child(L"Name").text().as_string();
			LastPlayed.Url = nSession.child(L"Station").child(L"Url").text().as_string();
			LastPlayed.Image = nSession.child(L"Station").child(L"Image").text().as_string();
			LastPlayed.Streams.push_back(bStream(nSession.child(L"Station").child(L"StreamUrl").text().as_string()));
			LastWindowPos.x = nSession.child(L"Window").child(L"left").text().as_int();
			LastWindowPos.y = nSession.child(L"Window").child(L"top").text().as_int();
			LogWindow = nSession.child(L"LogWindow").text().as_bool();
		}
		else
			Default();

		Succeeded = true;


	return Succeeded;

}

bool bConfig::Save()
{
	bool Succeeded = false;


	xml_document doc;
	doc.append_child(L"bTuner");
	xml_node nSession = doc.child(L"bTuner").append_child(L"Session");
	nSession.append_child(L"Station");
	nSession.child(L"Station").append_child(L"Name");
	nSession.child(L"Station").child(L"Name").append_child(pugi::node_pcdata);
	nSession.child(L"Station").append_child(L"Url");
	nSession.child(L"Station").child(L"Url").append_child(pugi::node_pcdata);
	nSession.child(L"Station").append_child(L"Image");
	nSession.child(L"Station").child(L"Image").append_child(pugi::node_pcdata);
	nSession.child(L"Station").append_child(L"StreamUrl");
	nSession.child(L"Station").child(L"StreamUrl").append_child(pugi::node_pcdata);
	nSession.append_child(L"Window");
	nSession.child(L"Window").append_child(L"top");
	nSession.child(L"Window").child(L"top").append_child(pugi::node_pcdata);
	nSession.child(L"Window").append_child(L"left");
	nSession.child(L"Window").child(L"left").append_child(pugi::node_pcdata);
	nSession.append_child(L"Volume");
	nSession.child(L"Volume").append_child(pugi::node_pcdata);
	nSession.child(L"Volume").text() = LastVolume;
	nSession.child(L"Station").child(L"Name").text() = LastPlayed.Name.c_str();
	nSession.child(L"Station").child(L"Url").text() = LastPlayed.Url.c_str();
	nSession.child(L"Station").child(L"Image").text() = LastPlayed.Image.c_str();
	if(LastPlayed.Streams.size())
		nSession.child(L"Station").child(L"StreamUrl").text() = LastPlayed.Streams.at(LastPlayed.PlayedStreamID).Url.c_str();
	nSession.child(L"Window").child(L"left").text() = LastWindowPos.x;
	nSession.child(L"Window").child(L"top").text() = LastWindowPos.y;
	nSession.append_child(L"LogWindow");
	nSession.child(L"LogWindow").append_child(pugi::node_pcdata);
	nSession.child(L"LogWindow").text() = LogWindow;

	Succeeded = doc.save_file("bTuner.xml");

	return Succeeded;

}

void bConfig::Default()
{
	LogWindow = false;
#ifdef _DEBUG
	LogWindow = true;
#else
	LogWindow = false;
#endif

	LastVolume = 100;
	LastWindowPos.x = 0;
	LastWindowPos.y = 0;

	Save();
}