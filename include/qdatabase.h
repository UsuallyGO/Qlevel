
#ifndef QDATABASE_H
#define QDATABASE_H

#include <string>

namespace Qlevel{

const int COMPACT_TABLE_SIZE = 10000;

class DataBase{
public:
	static int Open(const std::string& name, DataBase** ptr);
	static int Open2();
	virtual ~DataBase(){};//DataBase has been inherited inside
	virtual int Set(const char* key, const char* value) = 0;
	virtual int Get(const char* key, const char** value) = 0;
	virtual int Delete(const char* key) = 0;
	virtual size_t DataBaseSize() const = 0;
	virtual int Close();
};

typedef DataBase QDataBase;

}//!namespace Qlevel

#endif//!QDATABASE_H
