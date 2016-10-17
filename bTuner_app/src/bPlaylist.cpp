#include "bPlaylist.h"

bPlaylist::bPlaylist()
{

}

bPlaylist::~bPlaylist()
{

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
			int num = std::stoi(p1);
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
			int num = std::stoi(p1);
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

bool bPlaylist::SaveFile(std::wstring path)
{
	bool result = false;


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
	int length = file.tellg();
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

	

	return result;
}
