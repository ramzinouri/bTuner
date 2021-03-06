#ifndef BHTTP_H
#define BHTTP_H

#include "bLog.h"


#include <Windows.h>
#include <Wininet.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>



class bHttp
{
public:
	static bool DownloadFile(const std::wstring &url, const std::wstring &path, bool UseCache = true);
	static std::string FetchString(const std::wstring &url);
};



#endif //BHTTP_H
