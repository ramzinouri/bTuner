#ifndef BSTRING_H
#define BSTRING_H

#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include <Windows.h>

	class bString {
	public:
		static std::vector<std::wstring> Explode(std::wstring str,std::wstring separator);
		static std::wstring TrimLeft(std::wstring str);
		static std::wstring TrimRight(std::wstring str);
		static std::wstring Trim(std::wstring str);

		static void SearchAndReplace(std::wstring &str,const std::wstring &search, const std::wstring &replace);

		static std::wstring PointerToString(const char *ptr);
		static char *StringToPointer(std::wstring str);
		static std::wstring bString::ConvertToW(const std::string input);
		static std::string bString::ConvertToS(const std::wstring input);
	};


#endif  // BSTRING_H
