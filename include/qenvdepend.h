
#ifndef _ENVDEPEND_H_
#define _ENVDEPEND_H_

namespace Qlevel{

//Only readable
class _SequentialReadFile{
public:
	~_SequentialReadFile(){}
	virtual size_t Read(char* result, size_t n) = 0;
	virtual int Skip(size_t n) = 0;
};

//Sequential writable
class _SequentialAppendFile{
public:
	~_SequentialAppendFile(){}
	virtual size_t Append(const char* content, size_t n) = 0;
	virtual int Close() = 0;
	virtual int Flush() = 0;
	virtual int Sync() = 0;
};

//Only readable
class _RandomReadFile{
public:
	_RandomReadFile(){}
	virtual size_t Read(char* result, size_t n, size_t offset) = 0;
    //no Write() now...
};

typedef _SequentialReadFile SequentialReadFile;
typedef _SequentialAppendFile SequentialAppendFile;
typedef _RandomReadFile RandomReadFile;

class _EnvDepend{
public:
	//All the users get env with EnvDepend_Default(), this function must be
	//implemented in specified environment, such as in Windows or Posix.
	static _EnvDepend* EnvDepend_Default();

	virtual int NewSequentialReadFile(const char*, SequentialReadFile**) = 0;
	virtual int NewSequentialAppendFile(const char*, SequentialAppendFile**) = 0;
	virtual int NewRandomReadFile(const char*, RandomReadFile**) = 0;
	virtual int GetFileSize(const char*, size_t*) = 0;
	//Just Schedule, can not guaranteen task will run immediately
	virtual void Schedule(void (*task)(void*), void* arg) = 0;
	static long long GetTime();//seconds? milli-seconds? micro-seconds?
};

typedef _EnvDepend EnvDepend;
}//!namespace Qlevel

#endif //!_ENVDEPEND_H_
