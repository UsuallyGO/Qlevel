
#ifndef _DATABASE_POSIX_H_
#define _DATABASE_POSIX_H_

#include "qdatabase.h"
#include "qenvdepend.h"
#include "db/memtable.h"
#include "db/supervisor.h"
#include "port/port_posix.h"

namespace Qlevel{

class DataBase_Posix : public DataBase{
public:
	DataBase_Posix(const std::string& name);
	~DataBase_Posix();
	virtual int Set(const char* key, const char* value);
	virtual int Get(const char* key, const char** value);
	virtual int Delete(const char* key);
	virtual size_t DataBaseSize() const;

	//Compact related
	void MaybeCompact();
	static void CompactWorkWrap(void *db);
	void CompactWork();
	void Compact();
private:
	bool CompactScheduled;
private:
	//mutex and condvar is transparent to DataBase
	Port::Mutex   compact_mutex;
	Port::CondVar compact_condVar;
	Port::Mutex   schedule_mutex;

	EnvDepend*   _envdp;
	MemoryTable* _memTable;
	Supervisor  _supervisor;
};

}//!namespace Qlevel
#endif//!_DATABASE_POSIX_H_
