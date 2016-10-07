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
			xml_node nSession = doc.child("bTuner").child("Session");
			LastVolume = nSession.child("Volume").text().as_int();
			LastPlayedName = nSession.child("Station").child("Name").text().as_string();
			LastPlayedUrl = nSession.child("Station").child("Url").text().as_string();
			LastWindowPos.x = nSession.child("Window").child("left").text().as_int();
			LastWindowPos.y = nSession.child("Window").child("top").text().as_int();
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
	xml_node nSession = doc.child("bTuner").child("Session");

	nSession.child("Volume").text() = LastVolume;
	nSession.child("Station").child("Name").text()=LastPlayedName.c_str();
	nSession.child("Station").child("Url").text()=LastPlayedUrl.c_str();
	nSession.child("Window").child("left").text()=LastWindowPos.x;
	nSession.child("Window").child("top").text() = LastWindowPos.y;

	doc.save_file("Config.xml");
	Succeeded = true;

	return Succeeded;

}


void bConfig::Default()
{
	LastVolume = 100;
	LastPlayedName ="";
	LastPlayedUrl = "";
	LastWindowPos.x = 0;
	LastWindowPos.y = 0;
	xml_document doc;
	doc.append_child("bTuner");
	xml_node nSession = doc.child("bTuner").append_child("Session");
	nSession.append_child("Station");
	nSession.child("Station").append_child("Name");
	nSession.child("Station").child("Name").append_child(pugi::node_pcdata);
	nSession.child("Station").append_child("Url");
	nSession.child("Station").child("Url").append_child(pugi::node_pcdata);
	nSession.append_child("Window");
	nSession.child("Window").append_child("top");
	nSession.child("Window").child("top").append_child(pugi::node_pcdata);
	nSession.child("Window").append_child("left");
	nSession.child("Window").child("left").append_child(pugi::node_pcdata);
	nSession.append_child("Volume");
	nSession.child("Volume").append_child(pugi::node_pcdata);
	nSession.child("Volume").text() = LastVolume;
	nSession.child("Station").child("Name").text() = LastPlayedName.c_str();
	nSession.child("Station").child("Url").text() = LastPlayedUrl.c_str();
	nSession.child("Window").child("left").text() = LastWindowPos.x;
	nSession.child("Window").child("top").text() = LastWindowPos.y;

	doc.save_file("Config.xml");
}