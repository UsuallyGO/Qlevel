
#include <assert.h>
#include <stdio.h>
#include <string>

#include "qdatabase.h"
#include "qenvdepend.h"
#include "dbimpl/db_posix.h"
#include "db/memtable.h"

namespace Qlevel{

DataBase_Posix::DataBase_Posix(const std::string& name)
  : compact_condVar(&compact_mutex){
	//all mutexs will be init in their constructor
	_envdp = EnvDepend::EnvDepend_Default();
	_memTable = new MemoryTable();
	CompactScheduled = false;
}

/*Do not have to flush memory data to disk file, because most of the data has
 *been synchronized to disk while compact is working. The newly data in memory
 *table can be dropped directly because we can restore them from 'log' file
 *when this database being opened again. So in deconstructor, we can just refuse
 *any more compactions. Notice, we donot refuse schedule here, but newly schedule
 *will never lead to real compact(the compact_mutex is occupied in deconstructor).
 */
DataBase_Posix::~DataBase_Posix(){
    /*Pay attention: Lock on schedule_mutex can lead to deadlock*/
	compact_mutex.Lock();//Occupy the mutex, so there will be no more compact
	printf("###Starting to deconstruct the database...\n");
	printf("   time now:%lld\n", _envdp->GetTime());
	while(CompactScheduled){//Wait until compact is done if there is any
		compact_condVar.Wait();//Wake up when recently compact is done
	}
	//do something here...
	compact_mutex.UnLock();
}

int DataBase_Posix::Set(const char* key, const char* value){
	assert(_memTable);//Can't be null, instanced when database is opened
	int _result = _memTable->Set(key, value);//lock table here...
	MaybeCompact();
	return _result;
}

int DataBase_Posix::Get(const char* key, const char** value){
	assert(_memTable);
	return _memTable->Get(key, value);
}

int DataBase_Posix::Delete(const char* key){
	assert(_memTable);
	return _memTable->Delete(key);//lock table here
}

size_t DataBase_Posix::DataBaseSize() const{
	assert(_memTable);
	return _memTable->TableSize();//Think about twice here...
}

/*We need two mutexs which are 'schedule_mutex' and 'compact_mutex', the prev 
 *one is used to protect variable 'CompactScheduled' which used to check out
 *do we need to accept new schedule requirement now. The later one is used to
 *protect the database, for example, the database can't be closed until current
 *compacting is done.
 *Why don't we use just compact_mutex? Imagine about this: the database is
 *compacting now, a new schedule requirement is raised, if there is only one 
 *mutex, the new requirement will be wait until current compacting is done, then
 *schedule again. But actually, what we want to do is just refuse the new
 *requirement because we are compacting now. This means not every schedule
 *requirement can lead to real schedule.
 *Imagine about another situation: Several schedule requirements raised at the
 *same time, if there is no 'schedule_mutex', all of these requirements can
 *lead to real schedule sequencely. But actually, we don't have to schedule so 
 *many times at the same time. With 'shcedule_mutex', we can just schedule once
 *and refuse the others.
 */
void DataBase_Posix::MaybeCompact(){
	schedule_mutex.Lock();
    printf("###Schedule locked: MaybeCompact() ThreadID:%lu\n", pthread_self());
	printf("   time now:%lld\n", _envdp->GetTime());
	if(CompactScheduled){//already compacting now
		schedule_mutex.UnLock();
		printf("###MaybeCompact() No schedule ThreadID:%lu\n", pthread_self());
		printf("   time now:%lld\n", _envdp->GetTime());
	}
	else if(DataBaseSize() >= (size_t)1/*for debug*//*COMPACT_TABLE_SIZE*/){//Notice: which table?
		CompactScheduled = true;
		schedule_mutex.UnLock();//See the comments above
		printf("###MaybeCompact() end Schedule ThreadID:%lu\n", pthread_self());
		printf("   time now:%lld\n", _envdp->GetTime());
		_envdp->Schedule(DataBase_Posix::CompactWorkWrap, this);
	}
	else{
		schedule_mutex.UnLock();//No scheduling but also do need to schedule now
		printf("###MaybeCompact() quit this time ThreadID:%lu\n", pthread_self());
		printf("   time now:%lld\n", _envdp->GetTime());
	}
}

//Wrap for CompactWork(), make it more safety with reinterpret_cast for argument
void DataBase_Posix::CompactWorkWrap(void *db){
	reinterpret_cast<DataBase_Posix*>(db)->CompactWork();
}

void DataBase_Posix::CompactWork(){
	compact_mutex.Lock();
	assert(CompactScheduled);
	Compact();
    /*Do not use 'schedule_mutex' here because assign 'false' to
     *'CompactScheduled' do not effect MaybeCompact().*/
	CompactScheduled = false;
	MaybeCompact();
	compact_condVar.SignalAll();//CompactWork is done, database can be deleted right now
	compact_mutex.UnLock();
}

void DataBase_Posix::Compact(){
	std::cout<<"Compact()"<<std::endl;
	_memTable->Clear();
}

int DataBase::Open2(){
	std::cout<<"Opend2"<<std::endl;
}

int DataBase::Open(const std::string& name, DataBase** dbptr){
	*dbptr = NULL;

	DataBase *db = new DataBase_Posix(name);
	if(db == NULL) return QLEVEL_NO_MEMORY;
	*dbptr = db;
	return QLEVEL_SUCCESS;
}

int DataBase::Close(){
	//In nowadays, we use most simple way to do...
	delete this;
	return QLEVEL_SUCCESS;
}

};//!namespace Qlevel

