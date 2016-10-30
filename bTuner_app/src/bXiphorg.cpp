#include "bXiphorg.h"
#include "bHttp.h"
#include <thread>

bXiphorg::bXiphorg()
{
	Name = L"Xiph.org";
}

bXiphorg::~bXiphorg()
{
	if (Playlist)
		delete Playlist;
}

bool bXiphorg::UpdateStations()
{
	if (!Playlist)
		Playlist = new bPlaylist;

	std::thread th(std::bind(&bXiphorg::UpdateThread, this));
	th.detach();
	return true;
}

void bXiphorg::UpdateThread()
{
	Loading = true;
	if (Playlist)
		delete Playlist;
	Playlist = new bPlaylist;
	Playlist->title = L"Loading";
	bPlaylist *_Playlist = new bPlaylist;
	bLog::AddLog(bLogEntry(L"Start Downloading .... [http://dir.xiph.org/yp.xml]", L"bTuner Win", eLogType::Info));
	if (bHttp::DownloadFile(L"http://dir.xiph.org/yp.xml", L"yp.xml"))
	{
		bLog::AddLog(bLogEntry(L"Finish Downloading [http://dir.xiph.org/yp.xml]", L"bTuner Win", eLogType::Info));
		xml_document doc;
		bLog::AddLog(bLogEntry(L"Loading .... yp.xml", L"bTuner Win", eLogType::Info));
		xml_parse_result result = doc.load_file("yp.xml");

		if (result)
		{
			xml_node nDir = doc.child(L"directory");
			
			_Playlist->title = L"Xiph.org";
			int i = 0;
			for (xml_node nEntry = nDir.first_child(); nEntry; nEntry = nEntry.next_sibling())
			{

				int pos = _Playlist->Locate(nEntry.child(L"server_name").text().as_string());
				if (pos > 0)
				{
					bStream s(nEntry.child(L"listen_url").text().as_string());
					s.Bitrate = nEntry.child(L"bitrate").text().as_int();
					_Playlist->Stations.at(pos).Streams.push_back(s);
					continue;
				}

				bStation st;
				st.Name = nEntry.child(L"server_name").text().as_string();
				st.Genre = nEntry.child(L"genre").text().as_string();
				bStream s(nEntry.child(L"listen_url").text().as_string());
				s.Bitrate = nEntry.child(L"bitrate").text().as_int();
				st.Streams.push_back(s);
				st.ID = i;
				_Playlist->Stations.push_back(st);

				i++;
			}
			delete Playlist;
			Playlist = _Playlist;

			CString msg;
			msg.Format(L"[ %u ] Station Loaded From yp.xml", _Playlist->Stations.size());
			bLog::AddLog(bLogEntry(msg.c_str(), L"bTuner Win", eLogType::Info));
		}
	}
	Loading = false;
}

