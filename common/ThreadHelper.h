#ifndef THREADHELPER_H_
#define THREADHELPER_H_
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
//! LockHelper
//////////////////////////////////////////////////////////////////////////
class LockHelper
{
public:
	LockHelper();
	virtual ~LockHelper();

public:
	void lock();
	void unLock();
private:
#if defined (WIN32) || defined(_WIN64)
	CRITICAL_SECTION _crit;
#else
	pthread_mutex_t  _crit;
#endif
};

//////////////////////////////////////////////////////////////////////////
//! AutoLock
//////////////////////////////////////////////////////////////////////////
class AutoLock
{
public:
	explicit AutoLock(LockHelper & lk):_lock(lk){_lock.lock();}
	~AutoLock(){_lock.unLock();}
private:
	LockHelper & _lock;
};



//////////////////////////////////////////////////////////////////////////
//! ThreadHelper
//////////////////////////////////////////////////////////////////////////
#if defined (WIN32) || defined(_WIN64)
static unsigned int WINAPI  threadProc(LPVOID lpParam);
#else
static void * threadProc(void * pParam);
#endif

class ThreadHelper
{
public:
	ThreadHelper(){_hThreadID = 0;}
	virtual ~ThreadHelper(){}
public:
	bool start();
	bool wait();
	virtual void run() = 0;
private:
	unsigned long long _hThreadID;
#ifndef WIN32
	pthread_t _phtreadID;
#endif
};

#endif