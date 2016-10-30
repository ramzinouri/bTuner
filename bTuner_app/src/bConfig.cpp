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
			LastWindowPos.x = nSession.child(L"Window").child(L"left").text().as_int();
			LastWindowPos.y = nSession.child(L"Window").child(L"top").text().as_int();
			LogWindow = doc.child(L"bTuner").child(L"LogWindow").text().as_bool();
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
	nSession.append_child(L"Window");
	nSession.child(L"Window").append_child(L"top");
	nSession.child(L"Window").child(L"top").append_child(pugi::node_pcdata);
	nSession.child(L"Window").append_child(L"left");
	nSession.child(L"Window").child(L"left").append_child(pugi::node_pcdata);
	nSession.append_child(L"Volume");
	nSession.child(L"Volume").append_child(pugi::node_pcdata);
	nSession.child(L"Volume").text() = LastVolume;
	nSession.child(L"Window").child(L"left").text() = LastWindowPos.x;
	nSession.child(L"Window").child(L"top").text() = LastWindowPos.y;
	doc.child(L"bTuner").append_child(L"LogWindow");
	doc.child(L"bTuner").child(L"LogWindow").append_child(pugi::node_pcdata);
	doc.child(L"bTuner").child(L"LogWindow").text() = LogWindow;

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