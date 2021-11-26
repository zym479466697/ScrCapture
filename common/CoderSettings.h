#ifndef _CODER_SETTINGS_H_
#define _CODER_SETTINGS_H_


#pragma once

#include <Windows.h>
#include <string>


class CCoderSettings
{
public:
	static void Utf8ToUnicode(LPTSTR lpUnicode, DWORD dwMaxSize, LPCSTR lpUtf8);	
	static void UnicodeToUtf8(LPSTR lpUtf8, DWORD dwMaxSize, LPCTSTR lpUnicode);

	static std::wstring Utf8ToUnicode(const std::string& utfString);
	static std::string UnicodeToUtf8(const std::wstring& unicodeString);

	static void Gb2312ToUnicode(LPTSTR lpUnicode, DWORD dwMaxSize, LPCSTR lpGb2312);
	static void UnicodeToGb2312(LPSTR lpGb2312, DWORD dwMaxSize, LPCTSTR lpUnicode);

	static void Gb2312ToUtf8(LPSTR lpUtf8, DWORD dwMaxSize, LPCSTR lpGb2312);
	static void Utf8ToGb2312(LPSTR lpGb2312, DWORD dwMaxSize, LPCSTR lpUtf8);

	static std::string UrlEncode(const std::string& szUrl);
	static std::string UrlDecode(const std::string& szUrl);

private:
	static unsigned char FromHex(unsigned char x);
	static unsigned char ToHex(unsigned char x);
};























#endif