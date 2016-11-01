
#ifndef _PORT_H_
#define _PORT_H_

namespace Qlevel{

#include <pthread.h>

namespace Port{

class CondVar;

class Mutex{
public:
	Mutex();
	~Mutex();
	void Lock();
	void UnLock();
private:
	friend class CondVar;
	pthread_mutex_t mutex;
};

class CondVar{
public:
	//refuse type convertion
	explicit CondVar(Mutex* _mu);
	~CondVar();
	void Wait();
	void Signal();
	void SignalAll();
private:
	Mutex* mu;
	pthread_cond_t cv;
};

}//!namespace Port

}//!namespace Qlevel

#endif //!_PORT_H_
