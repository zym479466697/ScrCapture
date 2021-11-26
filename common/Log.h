#ifndef LOG_H_
#define LOG_H_
#include "ThreadHelper.h"
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <string>


#define LOG_LEVEL_DEBUG	0
#define LOG_LEVEL_INFO		1
#define LOG_LEVEL_WARN		2
#define LOG_LEVEL_ERROR	3

void LogInfo(int level, const wchar_t * file, int line, wchar_t *func, wchar_t* format, ...);
void LogInfo(int level, const char * file, int line, const char *func, const char* format, ...);
void Log2Dbg(const char *fmt, ...);

#define LOG_DEBUG(format, ...)  do{  LogInfo(LOG_LEVEL_DEBUG, __FILEW__, __LINE__, __FUNCTIONW__, format, ##__VA_ARGS__);} while (0)
#define LOG_INFO(format, ...)  do{  LogInfo(LOG_LEVEL_INFO, __FILEW__, __LINE__, __FUNCTIONW__, format, ##__VA_ARGS__);} while (0)
#define LOG_WARN(format, ...)  do{  LogInfo(LOG_LEVEL_WARN, __FILEW__, __LINE__, __FUNCTIONW__, format, ##__VA_ARGS__);} while (0)
#define LOG_ERROR(format, ...)  do{  LogInfo(LOG_LEVEL_ERROR, __FILEW__, __LINE__, __FUNCTIONW__, format, ##__VA_ARGS__);} while (0)
#define LOGA_DEBUG(format, ...)  do{  LogInfo(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__);} while (0)
#define LOGA_INFO(format, ...)  do{  LogInfo(LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__);} while (0)
#define LOGA_WARN(format, ...)  do{  LogInfo(LOG_LEVEL_WARN, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__);} while (0)
#define LOGA_ERROR(format, ...)  do{  LogInfo(LOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__);} while (0)

class LocalFileHandler
{
public:
	LocalFileHandler(){ _file = NULL; }
	~LocalFileHandler(){ close(); }
	inline bool isOpen(){ return _file != NULL; }
	inline bool open(const char *path, const char * mod)
	{
		if (_file != NULL){fclose(_file);_file = NULL;}
		fopen_s(&_file, path, mod);
		//_file = fopen(path, mod);
		return _file != NULL;
	}
	inline void close()
	{
		if (_file != NULL){fclose(_file);_file = NULL;}
	}
	inline void write(const char * data, size_t len)
	{
		if (_file && len > 0)
		{
			if (fwrite(data, 1, len, _file) != len)
			{
				close();
			}
		}
	}
	inline void flush(){ if (_file) fflush(_file); }

	inline std::string readLine()
	{
		char buf[500] = { 0 };
		if (_file && fgets(buf, 500, _file) != NULL)
		{
			return std::string(buf);
		}
		return std::string();
	}
	inline const std::string readContent();
public:
	FILE *_file;
};


class CLog : public ThreadHelper
{
public:
	CLog(void);
	~CLog(void);
	enum  OUTPUT_TYPE
	{
		TYPE_FILE,
		TYPE_OUTPUTDEBUG
	};
	//设置日志输出方式 默认是文件输出
	void SetOutputType(OUTPUT_TYPE type);
	//如果不填写filename 默认按日期写日志
	void SetLogFile(const char* szPath, const char* szFileName = NULL);
	void SetLogLevel(int level = LOG_LEVEL_INFO);
	int GetLogLevel() { return _log_level; }
	void PushInfo(const std::string& strContext);
	bool Stop();

	static CLog* getInstance(){
		static CLog m_pInstance; // 注意，声明在该函数内
		return &m_pInstance;
	}

	static LockHelper s_logLock;
protected:
	void LimitDayLogCnt(int cnt = 7);
	std::string DayFileName();
	virtual void run();
private:
	std::shared_ptr<LocalFileHandler> m_ptrLogFile;
	std::list<std::string> m_contextList;
	bool m_bRuning;
	std::string _str_path;
	std::string _mainLogFile;
	int _log_level;
	OUTPUT_TYPE _outputType;
};

#endif /*LOCALSTATISTICS_H_*/