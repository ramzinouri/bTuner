#include "bHttp.h"

bool bHttp::DownloadFile(const std::wstring & url, const std::wstring & path)
{
	::HINTERNET handle =::InternetOpen(0, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
	if (handle == 0)
	{
		const ::DWORD error = ::GetLastError();
		std::cerr
			<< "InternetOpen(): " << error << "."
			<< std::endl;
		return false;
	}
	HINTERNET stream =::InternetOpenUrlW(handle, url.c_str(), 0, 0, 0, 0);
	if (handle == 0)
	{
		const ::DWORD error = ::GetLastError();
		std::cerr
			<< "InternetOpenUrl(): " << error << "."
			<< std::endl;
		return false;
	}
	std::ofstream ostream(path, std::ios::binary);
	if (ostream.is_open())
	{
		static const ::DWORD SIZE = 1024;
		::DWORD error = ERROR_SUCCESS;
		::BYTE data[SIZE];
		::DWORD size = 0;
		do {
			::BOOL result = ::InternetReadFile(stream, data, SIZE, &size);
			if (result == FALSE)
			{
				error = ::GetLastError();
				std::cerr
					<< "InternetReadFile(): " << error << "."
					<< std::endl;
				return false;
			}
			ostream.write((const char*)data, size);
		} while ((error == ERROR_SUCCESS) && (size > 0));
	}
	else 
	{
		std::cerr << "Could not open file." << std::endl;
		return false;
	}


	if (::InternetCloseHandle(handle) == FALSE)
	{
		const ::DWORD error = ::GetLastError();
		std::cerr
			<< "InternetClose(): " << error << "."
			<< std::endl;
	}
	if (::InternetCloseHandle(stream) == FALSE)
	{
		const ::DWORD error = ::GetLastError();
		std::cerr
			<< "InternetClose(): " << error << "."
			<< std::endl;
	}
	
	return true;
}

std::string bHttp::FetchString(const std::wstring & url)
{
	::HINTERNET handle = ::InternetOpen(0, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
	if (handle == 0)
	{
		const ::DWORD error = ::GetLastError();
		std::cerr
			<< "InternetOpen(): " << error << "."
			<< std::endl;
		return std::string();
	}
	HINTERNET hstream = ::InternetOpenUrlW(handle, url.c_str(), 0, 0, 0, 0);
	if (handle == 0)
	{
		const ::DWORD error = ::GetLastError();
		std::cerr
			<< "InternetOpenUrl(): " << error << "."
			<< std::endl;
		return std::string();
	}
	std::stringstream stream;

		static const ::DWORD SIZE = 1024;
		::DWORD error = ERROR_SUCCESS;
		::BYTE data[SIZE];
		::DWORD size = 0;
		do {
			::BOOL result = ::InternetReadFile(hstream, data, SIZE, &size);
			if (result == FALSE)
			{
				error = ::GetLastError();
				std::cerr
					<< "InternetReadFile(): " << error << "."
					<< std::endl;
				return std::string();
			}
			stream.write((const char*)data,size);
		} while ((error == ERROR_SUCCESS) && (size > 0));



	if (::InternetCloseHandle(handle) == FALSE)
	{
		const ::DWORD error = ::GetLastError();
		std::cerr
			<< "InternetClose(): " << error << "."
			<< std::endl;
	}
	if (::InternetCloseHandle(hstream) == FALSE)
	{
		const ::DWORD error = ::GetLastError();
		std::cerr
			<< "InternetClose(): " << error << "."
			<< std::endl;
	}
	return stream.str();
}
