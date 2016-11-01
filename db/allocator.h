
#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <stdlib.h>
#include "db/coreobject.h"

class _Allocator : public _Virtual_Object{
public:
	static void* allocate(size_t n){
		void* _result = malloc(n);
		return _result;//save more safe protection, supplement later
	}
	static void* reallocate(void* ptr, size_t n){
		void* _result = realloc(ptr, n);
		return _result;
	}
	static void deallocate(void* ptr){
		free(ptr);
	}
/*
private:
	_Allocator();
	_Allocator(const _Allocator&);
*/
};

typedef _Allocator Simple_allocator;

#endif //!_ALLOCATOR_H_
