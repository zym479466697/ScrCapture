#include "CommFunc.h"
#include <iptypes.h>
#include <comutil.h>
#include <io.h>
#include <direct.h>
#include <fstream>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <tlhelp32.h> 
#include <assert.h> 
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <httpext.h>
#include <windef.h>
#include <atlbase.h>
#include <atlconv.h>
#include <nb30.h>
#include <snmp.h>
#include <iphlpapi.h>
#include "WindowsVersion.h"
#include<objbase.h>
#include<cstdio>

#pragma comment(lib, "ole32")
#pragma comment(lib, "shell32")
#pragma comment(lib, "comsuppw.lib")
#pragma comment ( lib, "Iphlpapi.lib" )
#pragma comment(lib,"netapi32")
//获取程序路径
std::wstring GetExeDir()
{
	wchar_t exeFullPath[MAX_PATH]; // Full path   
	std::wstring strPath = L"";
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	strPath = (std::wstring)exeFullPath;    // Get full path of the file   
	int pos = strPath.find_last_of(L'\\', strPath.length());
	return strPath.substr(0, pos);  // Return the directory without the file name   
}

int StringSplit(std::vector<std::wstring>& dst, const std::wstring& src, const std::wstring& separator)
{
	if (src.empty() || separator.empty())
		return 0;

	int nCount = 0;
	std::wstring temp;
	size_t pos = 0, offset = 0;

	// 分割第1~n-1个
	while ((pos = src.find_first_of(separator, offset)) != std::wstring::npos)
	{
		temp = src.substr(offset, pos - offset);
		if (temp.length() > 0) {
			dst.push_back(temp);
			nCount++;
		}
		offset = pos + 1;
	}

	// 分割第n个
	temp = src.substr(offset, src.length() - offset);
	if (temp.length() > 0) {
		dst.push_back(temp);
		nCount++;
	}

	return nCount;
}

int StringSplit(std::vector<std::string>& dst, const std::string& src, const std::string& separator)
{
	if (src.empty() || separator.empty())
		return 0;

	int nCount = 0;
	std::string temp;
	size_t pos = 0, offset = 0;

	// 分割第1~n-1个
	while ((pos = src.find_first_of(separator, offset)) != std::string::npos)
	{
		temp = src.substr(offset, pos - offset);
		if (temp.length() > 0) {
			dst.push_back(temp);
			nCount++;
		}
		offset = pos + 1;
	}

	// 分割第n个
	temp = src.substr(offset, src.length() - offset);
	if (temp.length() > 0) {
		dst.push_back(temp);
		nCount++;
	}
	return nCount;
}

std::string ws2s(const std::wstring& ws)
{
	_bstr_t t = ws.c_str();
	char* pchar = (char*)t;
	std::string result = pchar;
	return result;
}

std::wstring s2ws(const std::string& s)
{
	_bstr_t t = s.c_str();
	wchar_t* pwchar = (wchar_t*)t;
	std::wstring result = pwchar;
	return result;
}

//UTF-8到GB2312的转换
std::string U2G(const std::string utf8Str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	std::string tempStr = str;
	if (str) delete[] str;
	return tempStr;
}

//GB2312到UTF-8的转换
std::string G2U(const std::string gStr)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gStr.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gStr.c_str(), -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	std::string tempStr = str;
	if (str) delete[] str;
	return tempStr;
}


int CmdOutputByPipe(wchar_t *cmdline, std::wstring &result, int timeout, pf callback, bool bRetU2G, bool* bExit)
{
	result.clear();

	HANDLE rpipe;
	HANDLE wpipe;
	STARTUPINFOW si;
	DWORD exit_code;
	SECURITY_ATTRIBUTES sa;
	PROCESS_INFORMATION pi;

	memset(&sa, 0, sizeof(sa));
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));

	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&rpipe, &wpipe, &sa, 0)) {
		wchar_t szErrorCode[1024] = { 0 };
		wsprintf(szErrorCode, L"create pipe error code=[%d]", GetLastError());
		result = szErrorCode;
		if (callback) callback(szErrorCode);
		return -1;
	}

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = si.hStdError = wpipe;
	si.hStdInput = stdin;
	if (!CreateProcessW(NULL, cmdline, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
		wchar_t szErrorCode[1024] = { 0 };
		wsprintf(szErrorCode, L"CreateProcess error code=[%d]", GetLastError());
		result = szErrorCode;
		CloseHandle(rpipe);
		CloseHandle(wpipe);
		if (callback) callback(szErrorCode);
		return -2;
	}
	DWORD start_time = GetTickCount();
	while (true) {

		if (bExit != NULL)
		{
			if (*bExit)
			{
				return -3;
			}
		}

		if (timeout != -1) {
			if (GetTickCount() - start_time > (unsigned)timeout) {
				TerminateThread(pi.hThread, -3);
				TerminateProcess(pi.hProcess, -3);
				break;
			}
		}

		DWORD total = 0;
		if (!PeekNamedPipe(rpipe, NULL, 0, NULL, &total, NULL))
			break;

		if (total == 0) {
			DWORD wait = WaitForSingleObject(pi.hProcess, 100);
			if (wait == WAIT_TIMEOUT) {
				continue;
			}
			else if (wait == WAIT_OBJECT_0) {
				if (!PeekNamedPipe(rpipe, NULL, 0, NULL, &total, NULL))
					break;
				if (total == 0)
					break;
				else
					continue;
			}
		}

		std::string str;
		str.resize(total);
		DWORD read = 0;
		if (!ReadFile(rpipe, (void *)str.data(), total, &read, NULL))
			break;
		if (callback)
		{
			callback(s2ws(str));
		}
		if (bRetU2G)
			result += s2ws(U2G(str));
		else
			result += s2ws(str);
	}

	GetExitCodeProcess(pi.hProcess, &exit_code);
	CloseHandle(wpipe);
	CloseHandle(rpipe);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return exit_code;
}

void StringReplace(std::wstring& str, const std::wstring& to_replaced, const std::wstring& newchars)
{
	for (std::wstring::size_type pos(0); pos != std::wstring::npos; pos += newchars.length())
	{
		pos = str.find(to_replaced, pos);
		if (pos != std::wstring::npos)
			str.replace(pos, to_replaced.length(), newchars);
		else
			break;
	}
}

void StringReplace(std::string& str, const std::string& to_replaced, const std::string& newchars)
{
	for (std::string::size_type pos(0); pos != std::string::npos; pos += newchars.length())
	{
		pos = str.find(to_replaced, pos);
		if (pos != std::string::npos)
			str.replace(pos, to_replaced.length(), newchars);
		else
			break;
	}
}

std::wstring TrimFileName(const std::wstring& str)
{
	std::wstring strResult = str;
	StringReplace(strResult, L"\\", L"");
	StringReplace(strResult, L"/", L"");
	StringReplace(strResult, L":", L"");
	StringReplace(strResult, L"*", L"");
	StringReplace(strResult, L"?", L"");
	StringReplace(strResult, L"\"", L"");
	StringReplace(strResult, L"<", L"");
	StringReplace(strResult, L">", L"");
	StringReplace(strResult, L"|", L"");
	return strResult;
}


// &lt; < 小于号
//	&gt; > 大于号
//	&amp; & 和
//	&apos; ' 单引号
//	&quot; " 双引号
//文件名去掉特殊符号
std::wstring XmlTrim(const std::wstring& str)
{
	std::wstring strResult = str;
	StringReplace(strResult, L"<", L"lt;");
	StringReplace(strResult, L">", L"&gt;");
	StringReplace(strResult, L"&", L"&amp;");
	StringReplace(strResult, L"'", L"&apos;");
	StringReplace(strResult, L"\"", L"&quot;");
	return strResult;
}


bool IsWin64()
{
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandleA("kernel32"), "IsWow64Process");
	if (NULL != fnIsWow64Process)
	{
		fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
	}
	return bIsWow64;
}

BOOL GetSystemVersion(int& nVersion)
{
	nVersion = GetWinOSVersion();
	return TRUE;
}

//获取版本号
BOOL GetSystemStrVersion(std::string& strVersion)
{
	int nVersion = 0;
	GetSystemVersion(nVersion);
	switch (nVersion)
	{
	case SYSTEM_XP:
	{
		strVersion = "xp系统";
	}
	break;
	case SYSTEM_XP_SP3:
	{
		strVersion = "xp_sp3系统";
	}
	break;
	case SYSTEM_XP_64:
	{
		strVersion = "xp 64位系统";
	}
	break;
	case SYSTEM_VISTA_32:
	{
		strVersion = "vista 32位系统";
	}
	break;
	case SYSTEM_VISTA_64:
	{
		strVersion = "vista 64系统";
	}
	break;
	case SYSTEM_WIN7_32:
	{
		strVersion = "win7 32位系统";
	}
	break;
	case SYSTEM_WIN7_64:
	{
		strVersion = "win7 64位系统";
	}
	break;
	case SYSTEM_WIN8_32:
	{
		strVersion = "win8 32位系统";
	}
	break;
	case SYSTEM_WIN8_64:
	{
		strVersion = "win8 64位系统";
	}
	break;
	case SYSTEM_WIN8_1_32:
	{
		strVersion = "win8.1 32位系统";
	}
	break;
	case SYSTEM_WIN8_1_64:
	{
		strVersion = "win8.1 64位系统";
	}
	break;
	case SYSTEM_WIN10_32:
	{
		strVersion = "win10 32位系统";
	}
	break;
	case SYSTEM_WIN10_64:
	{
		strVersion = "win10 64位系统";
	}
	break;
	case SYSTEM_UNKNOWN:
	default:
	{
		strVersion = "未知系统";
	}
	}
	return true;
}

BOOL GetSystemStrVersion(std::wstring& strVersion)
{
	std::string strVer;
	BOOL ret = GetSystemStrVersion(strVer);
	strVersion = s2ws(strVer);
	return ret;
}

bool CopyFileTo(const std::wstring &strSrcFileName, const std::wstring &strDesFileName, bool bCover/* = true*/)
{
	std::wstring srcFilePath = L"\\\\?\\" + strSrcFileName;
	std::wstring desFilePath = L"\\\\?\\" + strDesFileName;

	if (!bCover)
	{
		//已经存在
		if (AccessEx(desFilePath, 0) != -1)
		{
			return false;
		}
	}

	std::ifstream in(srcFilePath, std::ios::binary);
	if (!in) return false;

	std::ofstream out(desFilePath, std::ios::binary);
	if (!out) {
		in.close();
		return false;
	}
	char flush[4096] = { 0 };
	while (!in.eof()) {
		//read从in流中读取4096字节，放入buf数组中，同时文件指针向后移动4096字节
		//若不足4096字节遇到文件结尾，则以实际提取字节读取
		in.read(flush, 4096);
		out.write(flush, in.gcount());
	}
	in.close();
	out.close();
	return true;
}

bool CopyTo(const std::wstring &strSrcPath, const std::wstring &strDescPath)
{
	//std::wstring strSrc = strSrcDir;
	//std::wstring strDesc = strDescDir;
	//strSrc.push_back(L'\0');
	//strSrc.push_back(L'\0');
	//strDesc.push_back(L'\0');
	//strDesc.push_back(L'\0');

	//SHFILEOPSTRUCT fop;
	//ZeroMemory(&fop, sizeof fop);
	//fop.wFunc = FO_COPY;
	//fop.hwnd = NULL;
	//fop.fFlags = FOF_NOCONFIRMATION|FOF_NO_UI;
	//fop.pFrom = strSrc.c_str();
	//fop.pTo = strDesc.c_str();
	//int ret = SHFileOperation(&fop);

	long hFile = 0;
	struct _wfinddata_t fileInfo;
	std::wstring pathName = strSrcPath + L"\\*";
	std::vector<std::wstring> fileList;
	if ((hFile = _wfindfirst(pathName.c_str(), &fileInfo)) == -1)
	{
		return false;
	}
	bool bRet = true;
	do {
		if (fileInfo.attrib&_A_SUBDIR) {
			std::wstring fname = std::wstring(fileInfo.name);
			if (fname == L".." || fname == L".") continue;

			std::wstring newPath = strSrcPath + L"\\" + fname;
			std::wstring newdesPath = strDescPath + L"\\" + fname;

			if (AccessEx(newdesPath, 0) == -1) //判断组合好的目录是否已经存在，不存在则创建
			{
				if (MkdirEx(newdesPath) != 0)
				{
					bRet = false;
					break;
				}
			}
			//递归复制子文件夹
			if (!CopyTo(newPath, newdesPath))
			{
				bRet = false;
				break;
			}
		}
		else {
			std::wstring fname = std::wstring(fileInfo.name);
			std::wstring srcFilePath = strSrcPath + L"\\" + fname;
			std::wstring desFilePath = strDescPath + L"\\" + fname;

			if (!CopyFileTo(srcFilePath, desFilePath))
			{
				bRet = false;
				break;
			}
		}
	} while (_wfindnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return bRet;
}

LONG64 FileSize(const std::wstring& strFileName)
{
	std::fstream input(strFileName.c_str());
	LONG64 size = 0;
	if (input.is_open())
	{
		input.seekg(0, std::ios::end);
		size = input.tellg();
		input.seekg(0, std::ios::beg);
		input.close();
	}
	return size;
}


bool GetFileMd5(__in const std::wstring& strFile, __out std::wstring& strOutMd5)
{
	wchar_t szCmdLine[1024] = { 0 };
	swprintf_s(szCmdLine, _countof(szCmdLine), L"certutil -hashfile \"%s\" MD5", strFile.c_str());
	std::wstring strResult;
	int retCode = CmdOutputByPipe(szCmdLine, strResult);
	if (retCode != 0)
	{
		return false;
	}
	std::vector<std::wstring> strResultList;
	StringSplit(strResultList, strResult, L"\r\n");
	if (strResultList.size() == 3)
	{
		std::wstring strMd5 = strResultList[1];
		StringReplace(strMd5, L" ", L"");
		strOutMd5 = strMd5;
		return true;
	}
	return false;
}


//删除目录
bool DelRecursionDir(const std::wstring& strDirPath)
{
	std::wstring strPathTmp = strDirPath + L"\\*.*";
	std::wstring sAddPath;
	struct _wfinddata_t sDate;
	long lFile = _wfindfirst(strPathTmp.c_str(), &sDate);
	if (lFile == -1)
	{
		RemoveEx(strDirPath.c_str());
		return false;
	}
	while (_wfindnext(lFile, &sDate) == 0)
	{
		if (sDate.attrib == _A_SUBDIR)
		{
			sAddPath = strDirPath;
			sAddPath += L"\\";
			sAddPath += sDate.name;
			if (std::wstring(sDate.name) == L"." || std::wstring(sDate.name) == L"..")
			{
				continue;
			}
			DelRecursionDir(sAddPath);
		}
		else
		{
			// 删除文件
			std::wstring strFile = strDirPath + L"\\" + std::wstring(sDate.name);
			RemoveEx(strFile.c_str());
		}
	}
	_findclose(lFile);
	// 删除文件夹
	_wrmdir(strDirPath.c_str());

	return true;
}

bool DelRecursionDir(const std::string& strDirPath)
{
	return DelRecursionDir(s2ws(strDirPath));
}

void fixPath(std::wstring &path)
{
	if (path.empty()) { return; }
	for (std::wstring::iterator iter = path.begin(); iter != path.end(); ++iter)
	{
		if (*iter == '\\') { *iter = '/'; }
	}
	if (path.at(path.length() - 1) != '/') { path.append(L"/"); }
}


bool isDirectory(const std::wstring& path)
{
	return PathIsDirectoryW(path.c_str()) ? true : false;
}

bool isDirectory(const std::string& path)
{
	return PathIsDirectoryA(path.c_str()) ? true : false;
}

bool CreateRecursionDir(const std::wstring& strPath)
{
	std::wstring path = strPath;
	if (path.length() == 0) return false;

	fixPath(path);

	std::wstring::size_type pos = path.find('/');
	while (pos != std::wstring::npos)
	{
		std::wstring cur = path.substr(0, pos - 0);
		if (cur.length() > 0 && !isDirectory(cur))
		{
			std::wstring curEx = cur;
			StringReplace(curEx, L"/", L"\\");
			bool ret = (MkdirEx(curEx) == 0);
			if (!ret)
				return false;
		}
		pos = path.find('/', pos + 1);
	}
	return true;
}

bool CreateRecursionDir(const std::string& strPath)
{
	return CreateRecursionDir(s2ws(strPath));
}

std::string ByteToFitFloat(LONG64 size)
{
	char szFormat[512] = { 0 };
	if (size / 1024 / 1024 / 1024 > 0)
	{
		float fRate = size / 1024.f / 1024.f / 1024.f;
		sprintf_s(szFormat, _countof(szFormat), "%0.2fG", fRate);
	}
	else if (size / 1024 / 1024 > 0)
	{
		float fRate = size / 1024.f / 1024.f;
		sprintf_s(szFormat, _countof(szFormat), "%0.2fM", fRate);
	}
	else if (size / 1024 > 0)
	{
		float fRate = size / 1024.f;
		sprintf_s(szFormat, _countof(szFormat), "%0.2fKB", fRate);
	}
	else
	{
		sprintf_s(szFormat, _countof(szFormat), "%lldB", size);
	}
	return szFormat;
}

std::string ByteToFitUnit(LONG64 size)
{
	char szFormat[512] = { 0 };
	if (size / 1024 / 1024 / 1024 > 0)
	{
		float fRate = size / 1024.f / 1024.f / 1024.f;
		sprintf_s(szFormat, _countof(szFormat), "%0.2fG", fRate);
	}
	else if (size / 1024 / 1024 > 0)
	{
		sprintf_s(szFormat, _countof(szFormat), "%lldM", size / 1024 / 1024);
	}
	else if (size / 1024 > 0)
	{
		sprintf_s(szFormat, _countof(szFormat), "%lldKB", size / 1024);
	}
	else
	{
		sprintf_s(szFormat, _countof(szFormat), "%lldB", size);
	}
	return szFormat;
}


int RenameEx(const std::wstring& strOldFileName, const std::wstring& strNewFileName)
{
	std::wstring strSrc = L"\\\\?\\" + strOldFileName;
	std::wstring strDest = L"\\\\?\\" + strNewFileName;
	return _wrename(strSrc.c_str(), strDest.c_str());
}

int RenameEx(const std::string& strOldFileName, const std::string& strNewFileName)
{
	return RenameEx(s2ws(strOldFileName), s2ws(strNewFileName));
}

int AccessEx(const std::wstring& strFileName, int mode)
{
	std::wstring strFile = L"\\\\?\\" + strFileName;
	return _waccess(strFile.c_str(), mode);
}

int AccessEx(const std::string& strFileName, int mode)
{
	return AccessEx(s2ws(strFileName), mode);
}

int RemoveEx(const std::wstring& strFileName)
{
	std::wstring strFile = L"\\\\?\\" + strFileName;
	return _wremove(strFile.c_str());
}

int RemoveEx(const std::string& strFileName)
{
	return RemoveEx(s2ws(strFileName));
}

int MkdirEx(const std::wstring& strPath)
{
	std::wstring strFile = L"\\\\?\\" + strPath;
	return _wmkdir(strFile.c_str());
}

int MkdirEx(const std::string& strPath)
{
	return MkdirEx(s2ws(strPath));
}


void FOpenEx(FILE**file, const std::wstring& strFileName, const std::wstring& strMode)
{
	std::wstring strFile = L"\\\\?\\" + strFileName;
	_wfopen_s(file, strFile.c_str(), strMode.c_str());
}

void FOpenEx(FILE**file, const std::string& strFileName, const std::string& strMode)
{
	FOpenEx(file, s2ws(strFileName), s2ws(strMode));
}


//创建快捷方式
bool CreateShortcut(PSHORTCUTCFG pShortcut, int nCsidl)
{
	TCHAR szLocationPath[MAX_PATH] = { 0 };
	TCHAR szShortcutFile[MAX_PATH] = { 0 };
	TCHAR szShortcutUninstallFile[MAX_PATH] = { 0 };

	LPITEMIDLIST lpItemIdList;
	HRESULT hRet = SHGetSpecialFolderLocation(0, nCsidl, &lpItemIdList);
	hRet = SUCCEEDED(hRet) ? SHGetPathFromIDList(lpItemIdList, szLocationPath) : hRet;
	if (SUCCEEDED(hRet) && -1 != _waccess(szLocationPath, 0))
	{
		if (CSIDL_COMMON_STARTMENU == nCsidl)   //开始菜单栏
		{
			TCHAR szStartMenu[MAX_PATH] = { 0 };
			TCHAR szStartMenuDir[MAX_PATH] = { 0 };

			wsprintf(szStartMenuDir, _T("%s\\Programs\\"), szLocationPath);

			if (_waccess(szStartMenuDir, 0) == -1)//表示Programs不存在 xp系统只认 "程序"
			{
				wsprintf(szStartMenu, _T("%s\\程序\\%s"), szLocationPath, pShortcut->szShortcutName);
			}
			else
			{
				wsprintf(szStartMenu, _T("%s\\Programs\\%s"), szLocationPath, pShortcut->szShortcutName);
			}

			if (-1 == _waccess(szStartMenu, 0))
			{
				_wmkdir(szStartMenu);
			}

			wsprintf(szShortcutFile, _T("%s\\%s.lnk"), szStartMenu, pShortcut->szShortcutName);
			wsprintf(szShortcutUninstallFile, _T("%s\\卸载 %s.lnk"), szStartMenu, pShortcut->szShortcutName);
		}
		else if (CSIDL_APPDATA == nCsidl)    //快速启动栏
		{
			wsprintf(szShortcutFile, _T("%s\\Microsoft\\Internet Explorer\\Quick Launch\\%s.lnk"),
				szLocationPath, pShortcut->szShortcutName);
		}
		else if (CSIDL_DESKTOP == nCsidl)   //桌面快捷方式
		{
			wsprintf(szShortcutFile, _T("%s\\%s.lnk"),
				szLocationPath, pShortcut->szShortcutName);
		}
	}
	else
	{
		return false;
	}

	BOOL bRet = false;
	IShellLink *pShellLink = NULL;
	IPersistFile *ppf = NULL;

	hRet = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&pShellLink);
	hRet = SUCCEEDED(hRet) ? pShellLink->QueryInterface(IID_IPersistFile, (void **)&ppf) : hRet;
	if (SUCCEEDED(hRet))
	{
		// 目标文件
		if (pShortcut->szDestFile[0] != '\0')
		{
			hRet = pShellLink->SetPath(pShortcut->szDestFile) >= 0;
		}

		// 参数
		if (SUCCEEDED(hRet) && pShortcut->szArguments[0] != '\0')
		{
			hRet = pShellLink->SetArguments(pShortcut->szArguments) >= 0;
		}

		// 显示图标
		if (SUCCEEDED(hRet) && pShortcut->szIconFile[0] != '\0' && _waccess(pShortcut->szIconFile, 0) == 0)
		{
			hRet = pShellLink->SetIconLocation(pShortcut->szIconFile, pShortcut->nIconIndex);
		}

		// 起始位置
		if (SUCCEEDED(hRet) && pShortcut->szWorkingDir[0] != '\0')
		{
			hRet = pShellLink->SetWorkingDirectory(pShortcut->szWorkingDir);
		}

		// 备注
		if (SUCCEEDED(hRet) && pShortcut->szDescription[0] != '\0')
		{
			hRet = pShellLink->SetDescription(pShortcut->szDescription);
		}

		// 快捷键
		if (SUCCEEDED(hRet) && pShortcut->wHotKey != 0)
		{
			hRet = pShellLink->SetHotkey(pShortcut->wHotKey);
		}

		// 运行方式
		if (SUCCEEDED(hRet) && pShortcut->nShowCmd != 0)
		{
			hRet = pShellLink->SetShowCmd(pShortcut->nShowCmd);
		}

		if (SUCCEEDED(hRet))
		{
			//TCHAR szShortcutFileW[MAX_PATH] = L"";
			//CharToTChar(szShortcutFile, szShortcutFileW);
			hRet = ppf->Save(szShortcutFile, TRUE);

			if (CSIDL_COMMON_STARTMENU == nCsidl && pShortcut->szUninstallFile[0] != '\0')
			{

				hRet = pShellLink->SetPath(pShortcut->szUninstallFile);
				if (SUCCEEDED(hRet))
				{
					//WCHAR szUninstallFileW[MAX_PATH] = L"";
					//CharToTChar(szShortcutUninstallFile, szUninstallFileW);
					hRet = ppf->Save(szShortcutUninstallFile, TRUE);
				}
			}

			if (SUCCEEDED(hRet))
				bRet = true;
		}
	}

	if (NULL != ppf)
		ppf->Release();
	if (NULL != pShellLink)
		pShellLink->Release();

	return bRet;
}


//删除开始菜单和快速启动栏 调用前 先 CoInitialize(NULL) 调用后 CoUninitialize()
BOOL DeleteShortcut(const TCHAR* szShortcutName, int nCsidl)
{
	assert(NULL != szShortcutName);
	TCHAR szLocationPath[MAX_PATH] = { 0 };
	TCHAR szShortcutFile[MAX_PATH] = { 0 };

	LPITEMIDLIST lpItemIdList;
	HRESULT hRet = SHGetSpecialFolderLocation(0, nCsidl, &lpItemIdList);
	hRet = SUCCEEDED(hRet) ? SHGetPathFromIDList(lpItemIdList, szLocationPath) : hRet;
	if (SUCCEEDED(hRet) && -1 != _waccess(szLocationPath, 0))
	{
		if (CSIDL_COMMON_STARTMENU == nCsidl)
		{
			TCHAR szStartMenu[MAX_PATH] = { 0 };
			TCHAR szStartMenuDir[MAX_PATH] = { 0 };

			wsprintf(szStartMenuDir, _T("%s\\Programs\\"), szLocationPath);

			if (_waccess(szStartMenuDir, 0) == -1)//表示Programs不存在 xp系统只认 "程序"
			{
				wsprintf(szStartMenu, _T("%s\\程序\\%s"), szLocationPath, szShortcutName);
			}
			else
			{
				wsprintf(szStartMenu, _T("%s\\Programs\\%s"), szLocationPath, szShortcutName);
			}
			//sprintf(szStartMenu, "%s\\Programs\\%s", szLocationPath, szShortcutName);
			if (0 == _waccess(szStartMenu, 0))
			{
				DelRecursionDir(szStartMenu);
			}
		}
		else
		{
			if (CSIDL_APPDATA == nCsidl)//快速启动栏
			{
				wsprintf(szShortcutFile, _T("%s\\Microsoft\\Internet Explorer\\Quick Launch\\%s.lnk"),
					szLocationPath, szShortcutName);
			}
			else if (CSIDL_DESKTOP == nCsidl)//删除桌面快捷方式
			{
				wsprintf(szShortcutFile, _T("%s\\%s.lnk"),
					szLocationPath, szShortcutName);
			}

			if (0 == _waccess(szShortcutFile, 0))
				DeleteFile(szShortcutFile);
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL FindProcessPid(LPCTSTR ProcessName, DWORD& dwPid)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return(FALSE);
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return(FALSE);
	}

	BOOL bRet = FALSE;
	do
	{
		if (!_tcscmp(ProcessName, pe32.szExeFile))
		{
			dwPid = pe32.th32ProcessID;
			bRet = TRUE;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return bRet;
}


bool endWith(const std::wstring &str, const std::wstring &tail) {
	if (str.size() < tail.size()) return false;
	return str.compare(str.size() - tail.size(), tail.size(), tail) == 0;
}

bool endWith(const std::string &str, const std::string &tail) {
	if (str.size() < tail.size()) return false;
	return str.compare(str.size() - tail.size(), tail.size(), tail) == 0;
}


bool startWith(const std::string &str, const std::string &start) {
	if (str.size() < start.size()) return false;
	return str.compare(0, start.size(), start) == 0;
}

bool startWith(const std::wstring &str, const std::wstring &start) {
	if (str.size() < start.size()) return false;
	return str.compare(0, start.size(), start) == 0;
}

//启动进程
bool StartProcess(LPCTSTR pszExeName, LPTSTR pszCmdLine, bool bIsBackground /* = true */)
{
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = bIsBackground ? SW_HIDE : SW_SHOWNORMAL;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS;
	if (bIsBackground)
	{
		dwCreationFlag |= CREATE_NO_WINDOW;
	}

	BOOL bRet = CreateProcess(pszExeName, pszCmdLine, NULL, NULL, FALSE, dwCreationFlag, NULL, NULL, &si, &pi);
	if (FALSE == bRet)
	{
		TCHAR szErr[128] = _T("");
		_sntprintf_s(szErr, 128, _T("CreateProcess failed. Error: %d"), GetLastError());
		OutputDebugString(szErr);
	}
	return bRet;
}



//启动进程
bool StartProcessWait(LPCTSTR pszExeName, LPTSTR pszCmdLine, DWORD& dwExitCode, bool bIsBackground/* = true */)
{
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = bIsBackground ? SW_HIDE : SW_SHOWNORMAL;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS;
	if (bIsBackground)
	{
		dwCreationFlag |= CREATE_NO_WINDOW;
	}

	BOOL bRet = CreateProcess(pszExeName, pszCmdLine, NULL, NULL, FALSE, dwCreationFlag, NULL, NULL, &si, &pi);
	if (FALSE == bRet)
	{
		TCHAR szErr[128] = { 0 };
		_sntprintf_s(szErr, 128, _T("CreateProcess failed. Error: %d"), GetLastError());
		OutputDebugString(szErr);

		dwExitCode = -1;
	}
	else
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
	}

	return bRet;
}

void ToLowerCase(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void ToLowerCase(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void ToUpperCase(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void ToUpperCase(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

//获取文件列表
//type -1->only file 0->file and dir 1->onlydir
std::vector<std::wstring> GetFileList(const std::wstring& path, int type/*=0*/)
{
	long hFile = 0;
	struct _wfinddata_t fileInfo;
	std::wstring pathName, exdName;
	std::vector<std::wstring> fileList;
	if ((hFile = _wfindfirst(pathName.assign(path).append(L"\\*").c_str(), &fileInfo)) == -1)
	{
		return fileList;
	}
	do {
		if (fileInfo.attrib&_A_SUBDIR) {
			std::wstring fname = std::wstring(fileInfo.name);
			if (fname == L".." || fname == L".")
			{
				continue;
			}
			if (type == 0 || type == 1) fileList.push_back(fname);
		}
		else {
			if (type == 0 || type == -1) fileList.push_back(fileInfo.name);
		}
	} while (_wfindnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return fileList;
}


//自己生成guid
std::wstring SelfRandGUID()
{
	srand(time(NULL));
	TCHAR buf[64] = { 0 };
	_snwprintf_s(buf, sizeof(buf),
		_T("{%08X%04X%04X%04X%04X%04X%04X}"),
		rand() & 0xffffffff,
		rand() & 0xffff,
		rand() & 0xffff,
		rand() & 0xffff,
		rand() & 0xffff, rand() & 0xffff, rand() & 0xffff
	);
	std::wstring strGuid = buf;
	return strGuid;
}


//guid生成
std::wstring MakeStrGUID()
{
	CoInitialize(NULL);
	TCHAR buf[64] = { 0 };
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snwprintf_s(buf, sizeof(buf)
			, _T("{%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X}")
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
			, guid.Data4[6], guid.Data4[7]
		);
	}
	//--COM  
	CoUninitialize();
	std::wstring strGuid = buf;
	if (strGuid.empty())
	{
		strGuid = SelfRandGUID();
	}
	return strGuid;
}


//锁定到任务栏
struct CoInitializeGuard
{
	HRESULT const hr;
	CoInitializeGuard() : hr(CoInitialize(NULL)) { }
	~CoInitializeGuard()
	{
		if (SUCCEEDED(hr))
		{
			CoUninitialize();
		}
	}
	operator HRESULT() const { return hr; }
};

struct PIDLFromPath
{
	PIDLIST_ABSOLUTE const pidl;
	PIDLFromPath(PCWSTR path) : pidl(ILCreateFromPathW(path)) { }
	~PIDLFromPath()
	{
		if (pidl)
		{
			ILFree(pidl);
		}
	}
	operator PIDLIST_ABSOLUTE() const { return pidl; }
};

const GUID CLSID_TaskbandPin =
{
	0x90aa3a4e, 0x1cba, 0x4233,
	{ 0xb8, 0xbb, 0x53, 0x57, 0x73, 0xd4, 0x84, 0x49 }
};

const GUID IID_IPinnedList3 =
{
	0x0dd79ae2, 0xd156, 0x45d4,
	{ 0x9e, 0xeb, 0x3b, 0x54, 0x97, 0x69, 0xe9, 0x40 }
};

enum PLMC { PLMC_EXPLORER = 4 };

struct IPinnedList3Vtbl;
struct IPinnedList3 { IPinnedList3Vtbl *vtbl; };

typedef ULONG STDMETHODCALLTYPE ReleaseFuncPtr(IPinnedList3 *that);
typedef HRESULT STDMETHODCALLTYPE ModifyFuncPtr(IPinnedList3 *that,
	PCIDLIST_ABSOLUTE unpin, PCIDLIST_ABSOLUTE pin, PLMC caller);

struct IPinnedList3Vtbl
{
	void *QueryInterface;
	void *AddRef;
	ReleaseFuncPtr *Release;
	void *MethodSlot4; void *MethodSlot5; void *MethodSlot6;
	void *MethodSlot7; void *MethodSlot8; void *MethodSlot9;
	void *MethodSlot10; void *MethodSlot11; void *MethodSlot12;
	void *MethodSlot13; void *MethodSlot14; void *MethodSlot15;
	void *MethodSlot16;
	ModifyFuncPtr *Modify;
};

bool PinUnpinTaskbar(const std::wstring& strFilePath, bool pin)
{
	CoInitializeGuard guard;
	if (!SUCCEEDED(guard)) {
		return false;
	}
	PIDLFromPath pidl(strFilePath.c_str());
	if (!pidl) {
		return false;
	}
	IPinnedList3 *pinnedList;
	if (!SUCCEEDED(CoCreateInstance(
		CLSID_TaskbandPin, NULL, CLSCTX_ALL,
		IID_IPinnedList3, (LPVOID *)(&pinnedList))))
	{
		return false;
	}
	if (!pinnedList) {
		return false;
	}
	if (!pinnedList->vtbl) {
		return false;
	}
	HRESULT hr = pinnedList->vtbl->Modify(pinnedList,
		pin ? NULL : pidl,
		pin ? pidl : NULL,
		PLMC_EXPLORER);
	if (!SUCCEEDED(hr)) {
		return false;
	}
	hr = pinnedList->vtbl->Release(pinnedList);
	if (!SUCCEEDED(hr))
	{
		return false;
	}
	return true;
}