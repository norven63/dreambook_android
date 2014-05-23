#pragma once

class CEvent  
{  
	void* mpRealEvent;
public:

	/* 
	动态方式初始化互斥锁,初始化状态变量m_cond 
	`bAutoReset  true   人工重置 
	false  自动重置 
	*/  
	CEvent(bool manualReset);         

	/* 
	注销互斥锁,注销状态变量m_cond 
	*/  
	~CEvent();  

	/* 
	将当前事件对象设置为有信号状态 
	若自动重置，则等待该事件对象的所有线程只有一个可被调度 
	若人工重置，则等待该事件对象的所有线程变为可被调度 
	*/  
	void SetEvent();  
	
	/* 
	将当前事件对象设置为无信号状态 
	*/  
	void ResetEvent();  

	/* 
	以当前事件对象，阻塞线程，将其永远挂起 
	直到事件对象被设置为有信号状态 
	*/  
	bool WaitEvent();  

	/* 
	以当前事件对象，阻塞线程，将其挂起指定时间间隔 
	之后线程自动恢复可调度 
	*/  
	bool WaitEvent(long milliseconds);  
};  

class CMutex
{
private:
	void* mpRealCMutex;
	
	CMutex(const CMutex&);
	CMutex& operator=(const CMutex&);
public:
	CMutex();
	~CMutex();

	void Lock();
	void Unlock();
};

/**
* @brief Semaphore class.
* The semaphore is actually an unsigned integer value which can be incremented
* (by Semaphore::Signal()) or decremented (by Semaphore::Wait()). If the value
* is 0 then any try to decrement it will result in execution blocking of the current thread
* until the value is incremented so the thread will be able to
* decrement it. If there are several threads waiting for semaphore decrement and
* some other thread increments it then only one of the hanging threads will be
* resumed, other threads will remain waiting for next increment.
*/
class CSemaphore
{
private:
	void* mpRealSeamphore;

	CSemaphore(const CSemaphore& );
	CSemaphore& operator=(const CSemaphore& );
public:
	CSemaphore(unsigned initialValue = 0);
	~CSemaphore();

	void Wait();
	bool Wait(unsigned int timeoutMillis);
	void Signal();
};



class CAutoLock
{
public:
	CAutoLock(CMutex& cs) : m_cs(cs)	{m_cs.Lock(); }
	~CAutoLock() { m_cs.Unlock(); }
private:
	CMutex& m_cs ;
};


/**
* @brief a base class for threads.
* This class should be used as a base class for thread objects, one should override the
* Thread::Run() method.
*/
class CThread
{
	friend void _threadrun(CThread* pThread);

	CMutex mutex1;

	unsigned int th;

	bool _bRunning;

	//forbid copying
	CThread(const CThread& );
	CThread& operator=(const CThread& );

public:

	CThread();

	virtual ~CThread();

	bool IsRunning() ;

	
	/**
	* @brief This should be overridden, this is what to be run in new thread.
	* Pure virtual method, it is called in new thread when thread runs.
	*/
	virtual void Run() = 0;

	/**
	* @brief Start thread execution.
	* Starts execution of the thread. Thread's Thread::Run() method will
	* be run as separate thread of execution.
	* @param stackSize - size of the stack in bytes which should be allocated for this thread.
	*                    If stackSize is 0 then system default stack size is used
	*                    (stack size depends on underlying OS).
	*/
	void Start(unsigned stackSize = 0);



	/**
	* @brief Wait for thread to finish its execution.
	* This function waits for the thread finishes its execution,
	* i.e. until the thread returns from its Thread::Run() method.
	* Note: it is safe to call Join() on not started threads,
	*       in that case it will return immediately.
	*/
	void Wait();


	/**
	* @brief get current thread ID.
	* Returns unique identifier of the currently executing thread. This ID can further be used
	* to make assertions to make sure that some code is executed in a specific thread. E.g.
	* assert that methods of some object are executed in the same thread where this object was
	* created.
	* @return unique thread identifier.
	*/
	static unsigned int GetCurrentThreadID();
};
