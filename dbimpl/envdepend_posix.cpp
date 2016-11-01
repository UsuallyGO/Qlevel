
#include <deque>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>

#include "qlevel.h"
#include "qenvdepend.h"

namespace Qlevel{

/*All these classes are defined in CPP file, because they can only accessed by
 *_EnvDepend instance.*/
class Posix_SequentialReadFile : public SequentialReadFile{
public:
	Posix_SequentialReadFile(const char* fname, FILE* f)
		: filename(fname), file(f){}
	~Posix_SequentialReadFile(){
		if(file != NULL) fclose(file);
	}
	virtual size_t Read(char* result, size_t n){
		size_t rn = fread(result, 1, n, file);
		return rn;
	}
	virtual int Skip(size_t n){
		if(fseek(file, n, SEEK_CUR)) return QLEVEL_SUCCESS;
		else return QLEVEL_SYS_ERROR;
	}

private:
	const char* filename;
	FILE* file;//just use ANSI-C
};

class Posix_SequentialAppendFile : public SequentialAppendFile{
public:
	Posix_SequentialAppendFile(const char* fname, FILE* f)
		: filename(fname), file(f){}
	~Posix_SequentialAppendFile(){
		if(file != NULL) fclose(file);
	}
	virtual size_t Append(const char* content, size_t n){
		size_t wn = fwrite(content, 1, n, file);
		return wn;
	}
	virtual int Close(){
		if(fclose(file) != 0) return QLEVEL_SYS_ERROR;
		file = NULL;
		return QLEVEL_SUCCESS;
	}
	virtual int Flush(){
		if(fflush(file) != 0) return QLEVEL_SYS_ERROR;
		else return QLEVEL_SUCCESS;
	}
	virtual int Sync(){
		if(fflush(file) != 0 || fsync(fileno(file)) != 0)
			return QLEVEL_SYS_ERROR;
		else return QLEVEL_SUCCESS;
	}

private:
	const char* filename;
	FILE* file;
};

//Someday, we will use it, let think twice...
class Posix_RandomAccessFile : public RandomReadFile{
public:
	Posix_RandomAccessFile(const char* fname, int _fd)
		: filename(fname), fd(_fd){}
	virtual ~Posix_RandomAccessFile(){ close(fd); }
	virtual size_t Read(char* result, size_t n, size_t offset)const{
		size_t rn = pread(fd, result, n, offset);
		return rn;
	}
private:
	const char* filename;
	int fd;//not FILE* here, because system call pread() need to pass fd
};

//Use mmap() to random read
class PosixMMap_RandomReadFile : public _RandomReadFile{
public:
	PosixMMap_RandomReadFile(const char* fname, void* base, size_t len)
				: filename(fname), mmap_region(base), length(len){}
	~PosixMMap_RandomReadFile(){
		munmap(mmap_region, length);
	}
	virtual size_t Read(char* result, size_t n, size_t offset){
		if(offset + n > length)
			n = length - offset;
		memcpy(result, mmap_region, n);
		return n;
	}

private:
	const char* filename;
	void* mmap_region;
	size_t length;
};

/*This class is defined in CPP file but not head file, because there is
 *only one instanced to all the databases. And the user get the instance
 *by EnvDepend_Default().
 */
class _EnvDepend_Posix : public _EnvDepend{
public:
	_EnvDepend_Posix();
	virtual int NewSequentialReadFile(const char*, SequentialReadFile**);
	virtual int NewSequentialAppendFile(const char*, SequentialAppendFile**);
	virtual int NewRandomReadFile(const char*, RandomReadFile**);
	virtual int GetFileSize(const char*, size_t*);
private:
	//background thread
	void BGThread();
	static void* BGThreadWrapper(void* arg){
		reinterpret_cast<_EnvDepend_Posix*>(arg)->BGThread();
		return NULL;
	}
	virtual void Schedule(void (*task)(void*), void*);
	bool BGIsRunning;

	struct TaskItem{
		void (*task)(void*);
		void *arg;
	};
	typedef std::deque<TaskItem> TaskQue;
	TaskQue _taskQue;

    /*Don't need to use port::Mutex and port::Cond, because this file is
     *just implemented for Posix environment.
     */
	pthread_mutex_t env_mutex;
	pthread_cond_t   env_cond;
	pthread_t bgthread;
};

_EnvDepend_Posix::_EnvDepend_Posix(){
	BGIsRunning = false;
	pthread_mutex_init(&env_mutex, NULL);
	pthread_cond_init(&env_cond, NULL);
}

int _EnvDepend_Posix::NewSequentialReadFile(const char * fname,
							SequentialReadFile** file){
	FILE* fp = fopen(fname, "r");
	if(fp == NULL){
        *file = NULL;
        return QLEVEL_SYS_ERROR;
    }
	else{
		*file = new Posix_SequentialReadFile(fname, fp);
		return QLEVEL_SUCCESS;
	}
}

int _EnvDepend_Posix::NewSequentialAppendFile(const char* fname,
						SequentialAppendFile** file){
	FILE* fp = fopen(fname, "a");
	if(fp == NULL){
        *file = NULL;
        return QLEVEL_SYS_ERROR;
    }
	else{
		*file = new Posix_SequentialAppendFile(fname, fp);
		return QLEVEL_SUCCESS;
	}
}

int _EnvDepend_Posix::NewRandomReadFile(const char* fname,
						RandomReadFile** file){
	int fd = open(fname, O_RDONLY);
	*file = NULL;
	if(fd < 0) return QLEVEL_SYS_ERROR;
	else{
        size_t size;
		int result = GetFileSize(fname, &size);
		if(result != QLEVEL_SUCCESS) return QLEVEL_SYS_ERROR;
		void* base = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
		if(base == MAP_FAILED) return QLEVEL_SYS_ERROR;
		else *file = new PosixMMap_RandomReadFile(fname, base, size);
		close(fd);
		return QLEVEL_SUCCESS;
	}
}

int _EnvDepend_Posix::GetFileSize(const char* fname, size_t* size){
	struct stat sbuf;
	if(stat(fname, &sbuf) != 0){//'0' means sucess
        *size = 0;
        return QLEVEL_ARG_INVAL;
    }
	else{
        *size = sbuf.st_size;
        return QLEVEL_SUCCESS;
    }
}

void _EnvDepend_Posix::Schedule(void (*task)(void*), void* arg){
	TaskItem item;
	pthread_mutex_lock(&env_mutex);

    if(!BGIsRunning){ //Started the background thread when schedule firstly
		BGIsRunning = true;
		//Always working once started
		pthread_create(&bgthread, NULL, &_EnvDepend_Posix::BGThreadWrapper, this);
    }
	item.task = task;
	item.arg = arg;
	_taskQue.push_back(item);
	if(_taskQue.size() == 1)//taskque is empty before push_back
		pthread_cond_signal(&env_cond);

	pthread_mutex_unlock(&env_mutex);
}

void _EnvDepend_Posix::BGThread(){
	while(1){//will always running once started
		pthread_mutex_lock(&env_mutex);
		while(_taskQue.empty())
			pthread_cond_wait(&env_cond, &env_mutex);

        printf("BGThread() ThreadID:%lu\n", bgthread);
		void (*task)(void*) = _taskQue.front().task;
		void *arg = _taskQue.front().arg;
		_taskQue.pop_front();
		pthread_mutex_unlock(&env_mutex);
		(*task)(arg);
	}
}

static pthread_once_t once = PTHREAD_ONCE_INIT;
static EnvDepend* default_env;
static void InitDefault_PosixEnv(){
	default_env = new _EnvDepend_Posix();
}

//use pthread_once to avoid init multi-times. pthread_once can work well
//under multi-threads environment. And it is more effcient than use mutex.
_EnvDepend* _EnvDepend::EnvDepend_Default(){
	pthread_once(&once, InitDefault_PosixEnv);
	return default_env;
}
long long _EnvDepend::GetTime(){
	return 0;//not implement now
}

}//!namespace Qlevel
