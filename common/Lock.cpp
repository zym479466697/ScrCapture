#include "Lock.h"

namespace MyLock
{

//创建一个匿名互斥对象
CMutex::CMutex()
{
	InitializeCriticalSection(&m_crs);
}

//销毁互斥对象，释放资源
CMutex::~CMutex()
{
	DeleteCriticalSection(&m_crs);
}

//确保拥有互斥对象的线程对被保护资源的独自访问
void CMutex::Lock()
{
	EnterCriticalSection(&m_crs);
}

//释放当前线程拥有的互斥对象，以使其它线程可以拥有互斥对象，对被保护资源进行访问
void CMutex::Unlock()
{
	LeaveCriticalSection(&m_crs);
}

//利用C++特性，进行自动加锁
CMutexLocker::CMutexLocker(CMutex* pMutex) : m_pMutex(pMutex)
{
	m_pMutex->Lock();
}

//利用C++特性，进行自动解锁
CMutexLocker::~CMutexLocker()
{
	m_pMutex->Unlock();
}

}