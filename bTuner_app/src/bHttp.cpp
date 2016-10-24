#include "bHttp.h"



bool bHttp::DownloadFile(const std::wstring & url, const std::wstring & path)
{
	::HINTERNET handle =::InternetOpen(0, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
	if (handle == 0)
	{
		bLog::AddLog(bLogEntry(L"InternetOpen Error", L"bHttp", eLogType::Error));
		return false;
	}
	BOOL op = TRUE;
	InternetSetOption(handle, INTERNET_OPTION_HTTP_DECODING, (LPVOID)&op, sizeof(BOOL));
	HINTERNET stream =::InternetOpenUrlW(handle, url.c_str(), L"Accept-Encoding: gzip", 21, INTERNET_FLAG_NO_COOKIES, 0);
	if (handle == 0)
	{
		bLog::AddLog(bLogEntry(L"InternetOpenURL Error", L"bHttp", eLogType::Error));
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
				bLog::AddLog(bLogEntry(L"InternetReadFile Error", L"bHttp", eLogType::Error));
				return false;
			}
			ostream.write((const char*)data, size);
		} while ((error == ERROR_SUCCESS) && (size > 0));

		ostream.close();
	}
	else 
	{
		bLog::AddLog(bLogEntry(L"Could not open file : "+path, L"bHttp", eLogType::Error));
		return false;
	}


	if (::InternetCloseHandle(stream) == FALSE)
		bLog::AddLog(bLogEntry(L"InternetCloseHandle 1 Error", L"bHttp", eLogType::Error));
	if (::InternetCloseHandle(handle) == FALSE)
		bLog::AddLog(bLogEntry(L"InternetCloseHandle 2 Error", L"bHttp", eLogType::Error));

	return true;
}

std::string bHttp::FetchString(const std::wstring & url)
{
	::HINTERNET handle = ::InternetOpen(0, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
	if (handle == 0)
	{
		bLog::AddLog(bLogEntry(L"InternetOpen FetchString Error", L"bHttp", eLogType::Error));
		return std::string();
	}
	BOOL op = TRUE;
	InternetSetOption(handle, INTERNET_OPTION_HTTP_DECODING, (LPVOID)&op, sizeof(BOOL));
	HINTERNET hstream = ::InternetOpenUrlW(handle, url.c_str(), L"Accept-Encoding: gzip", 21, INTERNET_FLAG_NO_COOKIES, 0);
	if (handle == 0)
	{
		bLog::AddLog(bLogEntry(L"InternetOpenUrl FetchString Error", L"bHttp", eLogType::Error));
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
				bLog::AddLog(bLogEntry(L"InternetReadFile FetchString Error", L"bHttp", eLogType::Error));
				return std::string();
			}
			stream.write((const char*)data,size);
		} while ((error == ERROR_SUCCESS) && (size > 0));



	if (::InternetCloseHandle(hstream) == FALSE)
		bLog::AddLog(bLogEntry(L"InternetCloseHandle 1 Fetch String Error", L"bHttp", eLogType::Error));
	if (::InternetCloseHandle(handle) == FALSE)
		bLog::AddLog(bLogEntry(L"InternetCloseHandle 2 Fetch String Error", L"bHttp", eLogType::Error));
	return stream.str();
}
