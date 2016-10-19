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
	xml_parse_result result = doc.load_file("bTuner.xml");
	
		if (result)
		{
			xml_node nSession = doc.child(L"bTuner").child(L"Session");
			LastVolume = nSession.child(L"Volume").text().as_int();
			LastPlayedName = nSession.child(L"Station").child(L"Name").text().as_string();
			LastPlayedUrl = nSession.child(L"Station").child(L"Url").text().as_string();
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
	LastPlayedName =L"";
	LastPlayedUrl = L"";
	LastWindowPos.x = 0;
	LastWindowPos.y = 0;

	Save();
}