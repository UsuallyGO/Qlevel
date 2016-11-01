
#include "db/memtable.h"

namespace Qlevel{

int MemoryTable::Set(const char* key, const char* value){
	return _table.Set(key, value);
}

int MemoryTable::Get(const char* key, const char** value){
	return _table.Get(key, value);
}

int MemoryTable::Delete(const char* key){
	return _table.Delete(key);
}

void MemoryTable::Clear(){
	_table.Clear();
}

size_t MemoryTable::TableSize() const {
	return _table.NodeNum();
}

}//!namespace Qlevel
