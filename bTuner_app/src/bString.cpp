#include "bString.h"

#include <cctype>

#include <algorithm>
#include <string>
#include <vector>

using std::string;
using std::wstring;
using std::vector;

vector<wstring> bString::Explode(wstring str, wstring separator) {
	vector<wstring> results;
	size_t found = str.find_first_of(separator);
	while (found != string::npos) {
		if (found > 0)
			results.push_back(str.substr(0, found));
		str = str.substr(found + separator.length());
		found = str.find_first_of(separator);
	}
	if (str.length() > 0)
		results.push_back(str);
	return results;
}

wstring bString::TrimLeft(wstring str) {
	wstring::iterator it;

	for (it = str.begin(); it != str.end(); ++it)
		if (!iswspace(*it))
			break;

	str.erase(str.begin(), it);
	return str;
}

wstring bString::TrimRight(wstring str) {
	wstring::reverse_iterator it;

	for (it = str.rbegin(); it != str.rend(); ++it)
		if (!iswspace(*it))
			break;

	string::difference_type dt = str.rend() - it;

	str.erase(str.begin() + dt, str.end());
	return str;
}

wstring bString::Trim(wstring str) {
	return TrimRight(TrimLeft(str));
}

void bString::SearchAndReplace(wstring &str, const wstring &search, const wstring &replace) {

	wstring::size_type next;
	for (next = str.find(search); next != string::npos;
		next = str.find(search, next)) {

		str.replace(next, search.length(), replace);
		next += replace.length();
	}
}

wstring bString::PointerToString(const char *ptr) {
	std::string cString(ptr);
	return ConvertToW(cString);
}

char *bString::StringToPointer(wstring str) {

	return (char*)ConvertToS(str).c_str();
}

std::wstring bString::ConvertToW(const std::string input) {
	try {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(input);
	}
	catch (std::range_error& e) {
		UNREFERENCED_PARAMETER(e);
		size_t length = input.length();
		std::wstring result;
		result.reserve(length);
		for (size_t i = 0; i < length; i++)
			result.push_back(input[i] & 0xFF);

		return result;
	}
}

std::string bString::ConvertToS(const std::wstring input) {
	try {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(input);
	}
	catch (std::range_error& e) {
		UNREFERENCED_PARAMETER(e);
		size_t length = input.length();
		std::string result;
		result.reserve(length);
		for (size_t i = 0; i < length; i++)
		{
			result.push_back(input[i] & 0xFF);
		}
		return result;
	}
}