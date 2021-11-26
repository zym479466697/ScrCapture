#include "CoderSettings.h"
#include <assert.h>



void CCoderSettings::Utf8ToUnicode(LPTSTR lpUnicode, DWORD dwMaxSize, LPCSTR lpUtf8)
{
	assert(NULL != lpUtf8 && NULL != lpUnicode);

	int nLength = MultiByteToWideChar(CP_UTF8, 0, lpUtf8, -1, NULL, 0);
	TCHAR* szTemp = (TCHAR*)calloc(nLength+1, sizeof(TCHAR));
	MultiByteToWideChar(CP_UTF8, 0, lpUtf8, -1, szTemp, nLength);

	wcsncpy(lpUnicode, szTemp, dwMaxSize > nLength ? nLength : dwMaxSize);
	free(szTemp);
	szTemp = NULL;
}

void CCoderSettings::UnicodeToUtf8(LPSTR lpUtf8, DWORD dwMaxSize, LPCTSTR lpUnicode)
{
	assert(NULL != lpUtf8 && NULL != lpUnicode);

	int nLength = WideCharToMultiByte(CP_UTF8, 0, lpUnicode, -1, NULL, 0, NULL, NULL);
	char* szTemp = (char*)calloc(nLength+1, sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, lpUnicode, -1, szTemp, nLength, NULL, NULL);
	strncpy(lpUtf8, szTemp, dwMaxSize > nLength ? nLength : dwMaxSize);
	free(szTemp);
	szTemp = NULL;
}

std::wstring CCoderSettings::Utf8ToUnicode(const std::string& utfString)
{
	int nLength = MultiByteToWideChar(CP_UTF8, 0, utfString.c_str(), -1, NULL, 0);
	TCHAR* szTemp = (TCHAR*)calloc(nLength + 1, sizeof(TCHAR));
	MultiByteToWideChar(CP_UTF8, 0, utfString.c_str(), -1, szTemp, nLength);

	std::wstring unicodeString = szTemp;
	free(szTemp);
	szTemp = NULL;
	return unicodeString;
}

std::string CCoderSettings::UnicodeToUtf8(const std::wstring& unicodeString)
{
	int nLength = WideCharToMultiByte(CP_UTF8, 0, unicodeString.c_str(), -1, NULL, 0, NULL, NULL);
	char* szTemp = (char*)calloc(nLength + 1, sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, unicodeString.c_str(), -1, szTemp, nLength, NULL, NULL);
	
	std::string utfString = szTemp;
	free(szTemp);
	szTemp = NULL;
	return utfString;
}

void CCoderSettings::Gb2312ToUnicode(LPTSTR lpUnicode, DWORD dwMaxSize, LPCSTR lpGb2312)
{
	assert(NULL != lpGb2312 && NULL != lpUnicode);

	int nLength = MultiByteToWideChar(CP_ACP, 0, lpGb2312, -1, NULL, 0);
	TCHAR* szTemp = (TCHAR*)calloc(nLength+1, sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, 0, lpGb2312, -1, szTemp, nLength);

	wcsncpy(lpUnicode, szTemp, dwMaxSize > nLength ? nLength : dwMaxSize);
	free(szTemp);
	szTemp = NULL;
}

void CCoderSettings::UnicodeToGb2312(LPSTR lpGb2312, DWORD dwMaxSize, LPCTSTR lpUnicode)
{
	assert(NULL != lpUnicode && NULL != lpGb2312);

	int nLength = WideCharToMultiByte(CP_ACP, 0, lpUnicode, -1, NULL, 0, NULL, NULL);
	char* szTemp = (char*)calloc(nLength+1, sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, lpUnicode, -1, szTemp, nLength, NULL, NULL);

	strncpy(lpGb2312, szTemp, dwMaxSize > nLength ? nLength : dwMaxSize);
	free(szTemp);
	szTemp = NULL;
}

void CCoderSettings::Gb2312ToUtf8(LPSTR lpUtf8, DWORD dwMaxSize, LPCSTR lpGb2312)
{
	assert(NULL != lpGb2312 && NULL != lpUtf8);

	int nLength1 = MultiByteToWideChar(CP_ACP, 0, lpGb2312, -1, NULL, 0);
	TCHAR* szTemp = (TCHAR*)calloc(nLength1+1, sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, 0, lpGb2312, -1, szTemp, nLength1);

	int nLength2 = WideCharToMultiByte(CP_UTF8, 0, szTemp, -1, NULL, 0, NULL, NULL);
	char* szTemp2 = (char*)calloc(nLength2+1, sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, szTemp, -1, szTemp2, nLength2, NULL, NULL);

	strncpy(lpUtf8, szTemp2, dwMaxSize > nLength2 ? nLength2 : dwMaxSize);
	free(szTemp);
	free(szTemp2);

	szTemp = NULL;
	szTemp2 = NULL;
}

void CCoderSettings::Utf8ToGb2312(LPSTR lpGb2312, DWORD dwMaxSize, LPCSTR lpUtf8)
{
	assert(NULL != lpGb2312 && NULL != lpUtf8);
	int nLength1 = MultiByteToWideChar(CP_UTF8, 0, lpUtf8, -1, NULL, 0);
	TCHAR* szTemp = (TCHAR*)calloc(nLength1+1, sizeof(TCHAR));
	MultiByteToWideChar(CP_UTF8, 0, lpUtf8, -1, szTemp, nLength1);

	int nLength2 = WideCharToMultiByte(CP_ACP, 0, szTemp, -1, NULL, 0, NULL, NULL);
	char* szTemp2 = (char*)calloc(nLength2+1, sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, szTemp, -1, szTemp2, nLength2, NULL, NULL);

	strncpy(lpGb2312, szTemp2, dwMaxSize > nLength2 ? nLength2 : dwMaxSize);
	free(szTemp);
	free(szTemp2);

	szTemp = NULL;
	szTemp2 = NULL;
}

std::string CCoderSettings::UrlEncode(const std::string& szUrl)
{
	std::string strTemp = "";  
	size_t length = szUrl.length();  
	for (size_t i = 0; i < length; i++)  
	{  
		if (isalnum((unsigned char)szUrl[i]) ||   
			(szUrl[i] == '-') ||  
			(szUrl[i] == '_') ||   
			(szUrl[i] == '.') ||   
			(szUrl[i] == '~'))  
			strTemp += szUrl[i];  
		else if (szUrl[i] == ' ')  
			strTemp += "+";  
		else  
		{  
			strTemp += '%';  
			strTemp += ToHex((unsigned char)szUrl[i] >> 4);  
			strTemp += ToHex((unsigned char)szUrl[i] % 16);  
		}  
	}  
	return strTemp; 
}

std::string CCoderSettings::UrlDecode(const std::string& szUrl)
{
	std::string strTemp = "";  
	size_t length = szUrl.length();  
	for (size_t i = 0; i < length; i++)  
	{  
		if (szUrl[i] == '+') strTemp += ' ';  
		else if (szUrl[i] == '%')  
		{  
			unsigned char high = FromHex((unsigned char)szUrl[++i]);  
			unsigned char low = FromHex((unsigned char)szUrl[++i]);  
			strTemp += high*16 + low;  
		}  
		else strTemp += szUrl[i];  
	}  
	return strTemp; 
}

unsigned char CCoderSettings::FromHex(unsigned char x)
{
	unsigned char y;  
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;  
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;  
	else if (x >= '0' && x <= '9') y = x - '0';  
	return y; 
}

unsigned char CCoderSettings::ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48; 
}

