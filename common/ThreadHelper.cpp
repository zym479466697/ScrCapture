#include "ThreadHelper.h"
#include <iostream>
#include <process.h>
//////////////////////////////////////////////////////////////////////////
// LockHelper
//////////////////////////////////////////////////////////////////////////
LockHelper::LockHelper()
{
#if defined (WIN32) || defined(_WIN64)
	InitializeCriticalSection(&_crit);
#else
	//_crit = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&_crit, &attr);
	pthread_mutexattr_destroy(&attr);
#endif
}
LockHelper::~LockHelper()
{
#if defined (WIN32) || defined(_WIN64)
	DeleteCriticalSection(&_crit);
#else
	pthread_mutex_destroy(&_crit);
#endif
}

void LockHelper::lock()
{
#if defined (WIN32) || defined(_WIN64)
	EnterCriticalSection(&_crit);
#else
	pthread_mutex_lock(&_crit);
#endif
}
void LockHelper::unLock()
{
#if defined (WIN32) || defined(_WIN64)
	LeaveCriticalSection(&_crit);
#else
	pthread_mutex_unlock(&_crit);
#endif
}


#if defined (WIN32) || defined(_WIN64)
unsigned int WINAPI  threadProc(LPVOID lpParam)
{
	ThreadHelper * p = (ThreadHelper *) lpParam;
	p->run();
	return 0;
}
#else
void * threadProc(void * pParam)
{
	ThreadHelper * p = (ThreadHelper *) pParam;
	p->run();
	return NULL;
}
#endif


//////////////////////////////////////////////////////////////////////////
//! ThreadHelper
//////////////////////////////////////////////////////////////////////////
bool ThreadHelper::start()
{
#if defined (WIN32) || defined(_WIN64)
	unsigned long long ret = _beginthreadex(NULL, 0, threadProc, (void *) this, 0, NULL);

	if (ret == -1 || ret == 0)
	{
		std::cout << "create thread error! \r\n" <<std::endl;
		return false;
	}
	_hThreadID = ret;
#else
	int ret = pthread_create(&_phtreadID, NULL, threadProc, (void*)this);
	if (ret != 0)
	{
		std::cout <<"create thread error! \r\n" << std::endl;
		return false;
	}
#endif
	return true;
}

bool ThreadHelper::wait()
{
#if defined (WIN32) || defined(_WIN64)
	if (WaitForSingleObject((HANDLE)_hThreadID, INFINITE) != WAIT_OBJECT_0)
	{
		return false;
	}
#else
	if (pthread_join(_phtreadID, NULL) != 0)
	{
		return false;
	}
#endif
	return true;
}