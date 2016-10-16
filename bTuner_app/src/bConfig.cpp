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

	xml_document doc;
	xml_parse_result result = doc.load_file("Config.xml");
	
		if (result)
		{
			xml_node nSession = doc.child(L"bTuner").child(L"Session");
			LastVolume = nSession.child(L"Volume").text().as_int();
			LastPlayedName = nSession.child(L"Station").child(L"Name").text().as_string();
			LastPlayedUrl = nSession.child(L"Station").child(L"Url").text().as_string();
			LastWindowPos.x = nSession.child(L"Window").child(L"left").text().as_int();
			LastWindowPos.y = nSession.child(L"Window").child(L"top").text().as_int();
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
	xml_parse_result result = doc.load_file("Config.xml");
	xml_node nSession = doc.child(L"bTuner").child(L"Session");

	nSession.child(L"Volume").text() = LastVolume;
	nSession.child(L"Station").child(L"Name").text()=LastPlayedName.c_str();
	nSession.child(L"Station").child(L"Url").text()=LastPlayedUrl.c_str();
	nSession.child(L"Window").child(L"left").text()=LastWindowPos.x;
	nSession.child(L"Window").child(L"top").text() = LastWindowPos.y;

	
	Succeeded = doc.save_file("Config.xml");

	return Succeeded;

}


void bConfig::Default()
{
	LastVolume = 100;
	LastPlayedName =L"";
	LastPlayedUrl = L"";
	LastWindowPos.x = 0;
	LastWindowPos.y = 0;
	xml_document doc;
	doc.append_child(L"bTuner");
	xml_node nSession = doc.child(L"bTuner").append_child(L"Session");
	nSession.append_child(L"Station");
	nSession.child(L"Station").append_child(L"Name");
	nSession.child(L"Station").child(L"Name").append_child(pugi::node_pcdata);
	nSession.child(L"Station").append_child(L"Url");
	nSession.child(L"Station").child(L"Url").append_child(pugi::node_pcdata);
	nSession.append_child(L"Window");
	nSession.child(L"Window").append_child(L"top");
	nSession.child(L"Window").child(L"top").append_child(pugi::node_pcdata);
	nSession.child(L"Window").append_child(L"left");
	nSession.child(L"Window").child(L"left").append_child(pugi::node_pcdata);
	nSession.append_child(L"Volume");
	nSession.child(L"Volume").append_child(pugi::node_pcdata);
	nSession.child(L"Volume").text() = LastVolume;
	nSession.child(L"Station").child(L"Name").text() = LastPlayedName.c_str();
	nSession.child(L"Station").child(L"Url").text() = LastPlayedUrl.c_str();
	nSession.child(L"Window").child(L"left").text() = LastWindowPos.x;
	nSession.child(L"Window").child(L"top").text() = LastWindowPos.y;

	doc.save_file("Config.xml");
}