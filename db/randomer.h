
#ifndef _RANDOMER_H_
#define _RANDOMER_H_

#include <stdlib.h>
#include "db/coreobject.h"


class _Randomer : public _Virtual_Object{
public:
	static void setSeed(unsigned int seed){
		srand(seed);
	}
	static int getRandom(int begin, int end, unsigned int seed){
		int _result = rand()%(end - begin + 1) + begin;
		return _result;
	}
//has been inherited from _Virtual_Object, the code blow do not need anymore
/*
private:
	_Randomer();
	_Randomer(const _Randomer&);
*/
};

typedef _Randomer Simple_randomer;

#endif //!_RANDOMER_H_
