
#include <pthread.h>
#include "port/port_posix.h"

namespace Qlevel{

namespace Port{

Mutex::Mutex(){ pthread_mutex_init(&mutex, NULL); }
Mutex::~Mutex(){ pthread_mutex_destroy(&mutex); }
void Mutex::Lock(){ pthread_mutex_lock(&mutex); }
void Mutex::UnLock(){ pthread_mutex_unlock(&mutex); }

CondVar::CondVar(Mutex* _mu) : mu(_mu)
{ pthread_cond_init(&cv, NULL); }
CondVar::~CondVar(){ pthread_cond_destroy(&cv); }
void CondVar::Wait(){ pthread_cond_wait(&cv, &mu->mutex); }
void CondVar::Signal(){ pthread_cond_signal(&cv); }
void CondVar::SignalAll(){ pthread_cond_broadcast(&cv); }

}//!namespace Port

}//!namespace Qlevel
