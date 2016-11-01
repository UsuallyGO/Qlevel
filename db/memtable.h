
#ifndef _MEMTABLE_H_
#define _MEMTABLE_H_

#include "db/skiplist.h"

namespace Qlevel{

class _MemoryTable{
public:
	typedef SkipList::iterator iterator;
	typedef SkipList::const_iterator const_iterator;

	_MemoryTable(unsigned int seed = 0xdeadbeef) : _table(seed){
	}

	int Set(const char* key, const char* value);
	int Get(const char* key, const char** value);
	int Delete(const char* key);
	void Clear();
	size_t TableSize() const;

private:
	SkipList _table;
};

typedef _MemoryTable MemoryTable;

}//!namespace Qlevel

#endif //!_MEMTABLE_H_
