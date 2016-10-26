#include "bPlaylist.h"

bPlaylist::bPlaylist()
{

}

bPlaylist::~bPlaylist()
{

}

int bPlaylist::Locate(std::wstring name)
{
	for (unsigned int i=0 ;i < (int)Stations.size(); i++ )
	{
		if (Stations.at(i).Name == name)
			return i;
	}
	return -1;
}
std::vector<unsigned int> bPlaylist::Search(std::wstring name)
{
	std::vector<unsigned int> result;
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);


	for (unsigned int i = 0; i < (int)Stations.size(); i++)
	{
		std::wstring LName;
		std::wstring LGenre;
		LName.resize(Stations.at(i).Name.size());
		LGenre.resize(Stations.at(i).Genre.size());

		std::transform(Stations.at(i).Name.begin(), Stations.at(i).Name.end(), LName.begin(),::tolower);
		std::transform(Stations.at(i).Genre.begin(), Stations.at(i).Genre.end(), LGenre.begin(), ::tolower);

		if (LName.find(name) != std::wstring::npos)
		{
			result.push_back(i);
			continue;
		}
		if (LGenre.find(name) != std::wstring::npos)
			result.push_back(i);
	}
	return result;
}

void bPlaylist::Sort()
{
	std::sort(Stations.begin(), Stations.end());
}


bool bPlaylist::ParsePLS(std::wstringstream* data)
{
	bool result = false;
	std::wstring line;
	std::getline(*data, line);
	if (line.find(L"[playlist]") == std::wstring::npos)
		return false;
	for (;;)
	{
		std::getline(*data, line);
		if (data->eof())
			break;
		if (line.size() < 5)
			continue;
		if (line.find(L"Number") != std::wstring::npos)
			continue;
		if (line.find(L"number") != std::wstring::npos)
			continue;
		if (line.find(L"Version") != std::wstring::npos)
			continue;
		if (line.find(L"version") != std::wstring::npos)
			continue;
		if (line.find(L"Length") != std::wstring::npos)
			continue;
		if (line.find(L"length") != std::wstring::npos)
			continue;
		std::wstringstream l(line);
		std::wstring p1, p2;
		std::getline(l, p1, L'=');
		if (line.find(L"=") != std::wstring::npos)
			p2 = line.substr(line.find(L"=")+1,line.size());

		if (p1.find(L"Title")!= std::wstring::npos ||p1.find(L"title")!= std::wstring::npos)
		{
			p1.replace(0, 5, L"");
			unsigned int num = std::stoi(p1);
			if (num > Stations.size())
			{
				bStation s;
				s.Name = p2;
				Stations.push_back(s);
			}
			else
				Stations[num-1].Name=p2;
		}
		if (p1.find(L"File")!= std::wstring::npos || p1.find(L"file")!= std::wstring::npos)
		{
			p1.replace(0, 4, L"");
			unsigned int num = std::stoi(p1);
			if (num > Stations.size())
			{
				bStation s;
				bStream st(p2);
				s.Streams.push_back(st);
				Stations.push_back(s);
			}
			else
			{
				bStream st(p2);
				Stations[num].Streams.push_back(st);
			}

		}
	}
	return result;
}



bool bPlaylist::LoadFile(std::wstring path)
{
	bool result = false;
	std::wifstream file;

	file.open(path, std::ifstream::in);
	if (!file.is_open())
		return false;
	file.seekg(0, file.end);
	unsigned int length =(unsigned int) file.tellg();
	file.seekg(0, file.beg);
	wchar_t * buffer = new wchar_t[length+1];
	buffer[length] = 0;

	file.read(buffer, length);
	file.close();
	std::wstringstream data(buffer);
	std::wstring line;
	std::getline(data, line);
	if (line.find(L"[playlist]") != std::wstring::npos)
		return ParsePLS(new std::wstringstream(buffer));
	if (line.find(L"<?xml") != std::wstring::npos)
	{
		std::getline(data, line);
		if (line.find(L"<playlist") != std::wstring::npos)
			return ParseXSPF(new std::wstringstream(buffer));
	}

		

	return result;
}

bool bPlaylist::ParseXSPF(std::wstringstream* data)
{
	std::wstring line;
	std::getline(*data, line);

	xml_document doc;
	xml_parse_result result = doc.load_string(data->str().c_str());

	if (!result)
		return false;

	xml_node nTrackList = doc.child(L"playlist").child(L"trackList");
	title = doc.child(L"playlist").child(L"title").text().as_string();
	int i = 0;
	for (xml_node nTrack = nTrackList.first_child(); nTrack; nTrack = nTrack.next_sibling())
	{
		bStation st;
		if (nTrack.child(L"title"))
			st.Name = nTrack.child(L"title").text().as_string();
		else
			st.Name = doc.child(L"playlist").child(L"title").text().as_string();

		if (nTrack.child(L"info"))
			st.Url = nTrack.child(L"info").text().as_string();
		else
			st.Url = doc.child(L"playlist").child(L"info").text().as_string();

		if (nTrack.child(L"image"))
			st.Image = nTrack.child(L"image").text().as_string();
		else
			st.Image = doc.child(L"playlist").child(L"image").text().as_string();
		st.ID = i;

		for (xml_node location = nTrack.child(L"location"); location; location = location.next_sibling(L"location"))
		{
			bStream s(location.text().as_string());
			st.Streams.push_back(s);
		}
		Stations.push_back(st);
		i++;
	}
	
	return true;
}

bool bPlaylist::SaveFile(std::wstring path)
{
	bool result = false;


	return result;
}

bool bPlaylist::SaveXSPF(std::wstring path)
{
	bool result = false;
	if (!Stations.size())
		return false;
	xml_document doc;
	xml_node playlist=doc.append_child(L"playlist");
	playlist.append_attribute(L"version");
	playlist.attribute(L"version") = 1;
	playlist.append_attribute(L"xmlns");
	playlist.attribute(L"xmlns") = L"http://xspf.org/ns/0/";
	if (title.size())
	{
		playlist.append_child(L"title");
		playlist.child(L"title").append_child(pugi::node_pcdata);
		playlist.child(L"title").text() = title.c_str();
	}


	xml_node trackList = playlist.append_child(L"trackList");

	for (unsigned int i = 0; i < Stations.size(); i++)
	{
		xml_node track=trackList.append_child(L"track");
		track.append_child(L"title");
		track.child(L"title").append_child(pugi::node_pcdata);
		track.child(L"title").text() = Stations.at(i).Name.c_str();

		for (unsigned int j = 0; j < Stations.at(i).Streams.size(); j++)
		{
			xml_node location=track.append_child(L"location");;
			location.append_child(pugi::node_pcdata);
			location.text() = Stations.at(i).Streams.at(j).Url.c_str();
		}
		if (Stations.at(i).Url.size())
		{
			track.append_child(L"info");
			track.child(L"info").append_child(pugi::node_pcdata);
			track.child(L"info").text() = Stations.at(i).Url.c_str();
		}
		if (Stations.at(i).Image.size())
		{
			track.append_child(L"image");
			track.child(L"image").append_child(pugi::node_pcdata);
			track.child(L"image").text() = Stations.at(i).Image.c_str();
		}
	}
	
	result=doc.save_file(path.c_str(), L"\t", 1U, pugi::encoding_utf8);


	return result;
}

