#pragma once

class CEvent  
{  
	void* mpRealEvent;
public:

	/* 
	��̬��ʽ��ʼ��������,��ʼ��״̬����m_cond 
	`bAutoReset  true   �˹����� 
	false  �Զ����� 
	*/  
	CEvent(bool manualReset);         

	/* 
	ע��������,ע��״̬����m_cond 
	*/  
	~CEvent();  

	/* 
	����ǰ�¼���������Ϊ���ź�״̬ 
	���Զ����ã���ȴ����¼�����������߳�ֻ��һ���ɱ����� 
	���˹����ã���ȴ����¼�����������̱߳�Ϊ�ɱ����� 
	*/  
	void SetEvent();  
	
	/* 
	����ǰ�¼���������Ϊ���ź�״̬ 
	*/  
	void ResetEvent();  

	/* 
	�Ե�ǰ�¼����������̣߳�������Զ���� 
	ֱ���¼���������Ϊ���ź�״̬ 
	*/  
	bool WaitEvent();  

	/* 
	�Ե�ǰ�¼����������̣߳��������ָ��ʱ���� 
	֮���߳��Զ��ָ��ɵ��� 
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
