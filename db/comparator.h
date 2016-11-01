
#ifndef _COMPARATOR_H_
#define _COMPARATOR_H_

#include <string.h>
#include "db/coreobject.h"

template<typename Tp1, typename Tp2>
class _Comparator : public _Virtual_Object{
public:
	static int compare(const Tp1&, const Tp2&){
		return 0;
	}
/*
private:
	_Comparator();
	_Comparator(const _Comparator&);
 */
};

template<>
class _Comparator<const char*, const char*> : public _Virtual_Object{
public:
	static int compare(const char* left, const char* right){
		return strcmp(left, right); //or memcmp() here???
	}
/*
private:
	_Comparator();
	_Comparator(const _Comparator&);
 */
};

typedef _Comparator<const char*, const char*> Comparator_psz;

#endif //!_COMPARATOR_H_