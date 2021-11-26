#include "Log.h"
#include "CommFunc.h"
#include <memory>
#include <io.h>
#include <stdio.h>
#include <Shlwapi.h>
#include <algorithm>

void LogInfo(int level, const wchar_t * file, int line, wchar_t *func, wchar_t* format, ...)
{
	if (level < CLog::getInstance()->GetLogLevel()) return;

	va_list pArgList;
	va_start(pArgList, format);
	wchar_t szMessage[4000] = { 0 };
	_vsnwprintf_s(szMessage, _countof(szMessage), format, pArgList);
	va_end(pArgList);

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	wchar_t szTime[64] = { 0 };
	wsprintf(szTime, L"%d-%02d-%02d %02d:%02d:%02d.%03d",
		sys.wYear,
		sys.wMonth,
		sys.wDay,
		sys.wHour,
		sys.wMinute,
		sys.wSecond,
		sys.wMilliseconds);

	wchar_t logbufw[4096] = { 0 };
	switch (level)
	{
		case LOG_LEVEL_DEBUG:
		default:
			{
				_snwprintf_s(logbufw, _countof(logbufw), _TRUNCATE, L"[%d][%s][DEBUG]%s", GetCurrentThreadId(), szTime, szMessage);
			}break;
		case LOG_LEVEL_INFO:
			{
				_snwprintf_s(logbufw, _countof(logbufw), _TRUNCATE, L"[%d][%s][INFO]%s", GetCurrentThreadId(), szTime, szMessage);
			}break;
		case LOG_LEVEL_WARN:
			{
				_snwprintf_s(logbufw, _countof(logbufw), _TRUNCATE, L"[%d][%s][WARN]%s", GetCurrentThreadId(), szTime, szMessage);
			}break;
		case LOG_LEVEL_ERROR:
			{
				_snwprintf_s(logbufw, _countof(logbufw), _TRUNCATE, L"[%d][%s][ERROR]%s", GetCurrentThreadId(), szTime, szMessage);
			}break;
	}
	//写入文件
	CLog::getInstance()->PushInfo(ws2s(logbufw));
}

void LogInfo(int level, const char * file, int line, const char *func, const char* format, ...)
{
	if (level < CLog::getInstance()->GetLogLevel()) return;

	va_list pArgList;
	va_start(pArgList, format);
	char szMessage[4000] = { 0 };
	_vsnprintf_s(szMessage, _countof(szMessage), format, pArgList);
	va_end(pArgList);

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	char szTime[64] = { 0 };
	sprintf_s(szTime, 64, "%d-%02d-%02d %02d:%02d:%02d.%03d",
		sys.wYear,
		sys.wMonth,
		sys.wDay,
		sys.wHour,
		sys.wMinute,
		sys.wSecond,
		sys.wMilliseconds);

	char logbufw[4096] = { 0 };
	switch (level)
	{
	case LOG_LEVEL_DEBUG:
	default:
	{
		_snprintf_s(logbufw, _countof(logbufw), _TRUNCATE, "[%d][%s][DEBUG]%s", GetCurrentThreadId(), szTime, szMessage);
	}break;
	case LOG_LEVEL_INFO:
	{
		_snprintf_s(logbufw, _countof(logbufw), _TRUNCATE, "[%d][%s][INFO]%s", GetCurrentThreadId(), szTime, szMessage);
	}break;
	case LOG_LEVEL_WARN:
	{
		_snprintf_s(logbufw, _countof(logbufw), _TRUNCATE, "[%d][%s][WARN]%s", GetCurrentThreadId(), szTime, szMessage);
	}break;
	case LOG_LEVEL_ERROR:
	{
		_snprintf_s(logbufw, _countof(logbufw), _TRUNCATE, "[%d][%s][ERROR]%s", GetCurrentThreadId(), szTime, szMessage);
	}break;
	}
	//写入文件
	CLog::getInstance()->PushInfo(std::string(logbufw));
}

void Log2Dbg(const char * fmt, ...)
{
//#ifdef _DEBUG
	va_list pArgList;
	va_start(pArgList, fmt);
	char szMessage[4000] = { 0 };
	_vsnprintf_s(szMessage, _countof(szMessage), fmt, pArgList);
	va_end(pArgList);
	OutputDebugStringA(szMessage);
//#endif
}

LockHelper CLog::s_logLock;
CLog::CLog(void)
{
	m_bRuning = false;
	_outputType = TYPE_FILE;
	_log_level = LOG_LEVEL_INFO;
}


CLog::~CLog(void)
{
	Stop();
}

//设置日志输出方式 默认是文件输出
void CLog::SetOutputType(OUTPUT_TYPE type)
{
	_outputType = type;
}

void CLog::SetLogFile(const char* szPath, const char* szFileName/* = NULL*/)
{
	CreateRecursionDir(szPath);
	AutoLock l(s_logLock);
	std::string strLogFileName;
	_str_path = szPath;
	if (szFileName != NULL)
	{
		strLogFileName = szFileName;
	}
	else
	{
		strLogFileName = DayFileName();
		//只保留7天内的日志
		LimitDayLogCnt();
	}
	
	_mainLogFile = _str_path + "\\" + strLogFileName;
	m_ptrLogFile = std::make_shared<LocalFileHandler>();
	bool ret = m_ptrLogFile->open(_mainLogFile.c_str(), "a+");
	if (!ret)
	{
		printf("open file=%s error=%d", _mainLogFile.c_str(), ::GetLastError());
	}
	//启动
	start();
}

void CLog::SetLogLevel(int level/* = LOG_LEVEL_INFO*/)
{
	_log_level = level;
}

std::string CLog::DayFileName()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	char szFile[64] = { 0 };
	sprintf_s(szFile, 64, "%d-%02d-%02d.log",sys.wYear, sys.wMonth, sys.wDay);
	return szFile;
}


void GetLogFileList(std::string path, std::vector<std::string>& fileList)
{
	long hFile = 0;
	struct _finddata_t fileInfo;
	std::string pathName, exdName;

	if ((hFile = _findfirst(pathName.assign(path).
		append("\\*").c_str(), &fileInfo)) == -1) {
		return;
	}
	do {
		if (fileInfo.attrib&_A_SUBDIR) {
			std::string fname = std::string(fileInfo.name);
			if (fname != ".." && fname != ".")
			{
				GetLogFileList(path + "\\" + fname, fileList);
			}
		}
		else {
			std::string strFileName = fileInfo.name;
			int posInf = strFileName.find(".log");
			if (posInf > -1)
			{
				fileList.push_back(strFileName);
			}
		}
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
}

bool compDay(const std::string &str1, const std::string &str2)
{
	std::string strLogName1 = str1;
	std::string strLogName2 = str2;
	int a1 = 0, b1 = 0, c1 = 0, d1 = 0;
	sscanf_s(strLogName1.c_str(), "%d-%d-%d-%d", &a1, &b1, &c1, &d1);
	int a2 = 0, b2 = 0, c2 = 0, d2 = 0;
	sscanf_s(strLogName2.c_str(), "%d-%d-%d-%d", &a2, &b2, &c2, &d2);
	if (a1 > a2) return true;
	if (a1 < a2) return false;

	if (b1 > b2) return true;
	if (b1 < b2) return false;

	if (c1 > c2) return true;
	if (c1 < c2) return false;

	if (d1 > d2) return true;
	if (d1 < d2) return false;

	return false;
}

void CLog::LimitDayLogCnt(int cnt/* = 7*/)
{
	if (_access(_str_path.c_str(), 0) == -1) return;

	std::vector<std::string> logFileList;
	GetLogFileList(_str_path, logFileList);

	//删除文件
	if (logFileList.size() <= cnt) return;

	//sort log file by day
	sort(logFileList.begin(), logFileList.end(), compDay);

	//remove file from file list
	logFileList.erase(logFileList.begin(), logFileList.begin() + cnt);

	//remove the old files
	for (auto iter = logFileList.begin(); iter != logFileList.end(); iter++)
	{
		std::string strLogName = *iter;
		std::string strFile = _str_path + "\\"+ strLogName;
		if (_access(strFile.c_str(), 0) != -1)
		{
			//remove file
			remove(strFile.c_str());
		}
	}
}

void CLog::PushInfo(const std::string& strContext)
{
	AutoLock l(s_logLock);
	m_contextList.push_back(strContext);
}

bool CLog::Stop()
{
	if (m_bRuning == true)
	{
		m_bRuning = false;
		wait();
		m_ptrLogFile = nullptr;
		return true;
	}
	return false;
}

void CLog::run()
{
	m_bRuning = true;
	while (m_bRuning)
	{
		bool bEmpty = false;
		do
		{
			AutoLock l(s_logLock);
			if (m_contextList.empty())
			{
				bEmpty = true;
				break;
			}
			std::string strContext = m_contextList.front();
			strContext = strContext + "\n";
			m_contextList.pop_front();
			if (_outputType == TYPE_FILE)
			{
				if (m_ptrLogFile)
				{
					if (!m_ptrLogFile->isOpen())
					{
						m_ptrLogFile->open(_mainLogFile.c_str(), "a+");
					}
					m_ptrLogFile->write(strContext.c_str(), strContext.size());
					m_ptrLogFile->close();
				}
			}
			else
			{
				OutputDebugStringA(strContext.c_str());
			}
			
		} while (false);

		if (bEmpty && m_bRuning)
		{
			Sleep(100);
		}
	}
}