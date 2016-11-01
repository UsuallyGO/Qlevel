
#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <iostream>

#include "qlevel.h"
#include "db/allocator.h"
#include "db/randomer.h"
#include "db/comparator.h"

namespace Qlevel{

const int MAX_HEIGHT = 10;
struct _SkipNode{
	_SkipNode* levelNext[MAX_HEIGHT];
	int levelHeight;
	const char* key;
	const char* value;
	int state;
};

//Only can walk forward, no decrement(), iterator must be genericed with Tp
template<typename Tp>
struct _SkipList_forwardIterator_base{
	typedef Tp* link_type;
	link_type node;

    _SkipList_forwardIterator_base(link_type _node = NULL)
					: node(_node){}
	void Increment(){
		node = node->levelNext[0];
	}
};

template<typename Tp>
struct _SkipList_forwardIterator : public _SkipList_forwardIterator_base<Tp>{
    typedef Tp  value_type;
	typedef Tp& reference;
	typedef Tp* link_type;

	_SkipList_forwardIterator(link_type _node = NULL)
	  : _SkipList_forwardIterator_base<Tp>(_node){}

	_SkipList_forwardIterator& operator++(){
		this->Increment();
		return *this;
	}
	_SkipList_forwardIterator operator++(int){
		_SkipList_forwardIterator _tmp = *this;
		this->Increment();
		return _tmp;
	}

	link_type operator->(){
		return this->node;
	}
	reference operator*(){
		return *(this->node);
	}
	bool operator!=(const _SkipList_forwardIterator<Tp>& iter) const {
		return this->node != iter.node;
	}
	//no operator--
};

//const iterator, can't assign key or value with iterator->key/value
template<typename Tp>
struct _SkipList_const_forwardIterator : public _SkipList_forwardIterator_base<Tp>{
    typedef Tp  value_type;
	typedef Tp& reference;
	typedef  Tp& const_reference;
	typedef Tp* link_type;

    _SkipList_const_forwardIterator(link_type _node = NULL)
		: _SkipList_forwardIterator_base<Tp>(_node){}
	//We need 'const' here before parameter, because when we get const_iterator
	//from SkipList->Begin(), we assign the parameter to be a temporary variable,
	//for a temporary variable, we must use 'const' here.
	_SkipList_const_forwardIterator(const _SkipList_forwardIterator<Tp>& iter){
		this->node = iter.node;
	}
	/* Copy constructor of itertor base.
	_SkipList_const_forwardIterator(const _SkipList_forwardIterator_base<Tp>& iter){
		this->node = iter.node;
	}
	*/

	_SkipList_const_forwardIterator& operator++(){
		this->Increment();
		return *this;
	}
	_SkipList_const_forwardIterator operator++(int){
		_SkipList_const_forwardIterator _tmp = *this;
		this->Increment();
		return _tmp;
	}

	const link_type operator->() const{
		return this->node;
	}
	const_reference operator*() const {
		return *(this->node);
	}
	bool operator!=(_SkipList_const_forwardIterator<Tp>& iter)const{
		return this->node != iter.node;
	}
	bool operator!=(const _SkipList_forwardIterator<Tp>&  iter)const{
		return this->node != iter.node;
	}
};

//Key type and value type are const char*, don't need to be generalized
template<typename _Comparator, typename _Allocator, typename _Randomer>
class _SkipList{
public:
	_SkipList(unsigned int seed = 0xdeadbeef) : nodeCount(0), randomSeed(seed){
		memset(&dummyHead, 0, sizeof(dummyHead));
		dummyHead.levelHeight = LIST_HEIGHT - 1;
		memset(&dummyTail, 0, sizeof(dummyTail));
		dummyTail.levelHeight = 0;
		for(int _lev = 0; _lev <= LIST_HEIGHT-1; _lev++)
			dummyHead.levelNext[_lev] = &dummyTail;

		_Randomer::setSeed(randomSeed);
	}

	/*insert or update */
	int Set(const char* key, const char* value);
	int Get(const char* key, const char** value);
	int Delete(const char* key);
	void Clear();
	size_t NodeNum() const{
		return nodeCount;
	}
	void PrintStatis() const{
		_SkipNode* node = dummyHead.levelNext[0];
		while(node != 0){
			std::cout<<"Node key:"<<node->key<<"  value:"<<node->value;
			std::cout<<"  Levels:"<<node->levelHeight<<std::endl;
			node = node->levelNext[0];
		}
	}

	typedef _SkipList_forwardIterator<_SkipNode> iterator;
	typedef _SkipList_const_forwardIterator<_SkipNode> const_iterator;

	iterator Begin(){
		return dummyHead.levelNext[0];
	}
	const_iterator Begin() const{
		return dummyHead.levelNext[0];
	}
	iterator End(){
		return &dummyTail;
	}
	const_iterator End() const{
		return &dummyTail;
	}

private:
	enum {LIST_HEIGHT = MAX_HEIGHT, NODE_SIZE = sizeof(_SkipNode)};
	enum {NODE_DUMMY = 0, NODE_EMPTY = 1, NODE_NORMAL = 2, NODE_DELETE = 3 };

	_SkipNode* _searchNode(const char* key);//return node maybe changed, no 'const' here
	int _getRandom(int begin, int end) const{
		return _Randomer::getRandom(begin, end, randomSeed);
	}
	int _compare(const char* key, const char* value) const{
		_cmpCount++;
		return _Comparator::compare(key, value);
	}
	_SkipNode* _allocNode() const{
		_SkipNode* node = (_SkipNode*)_Allocator::allocate(NODE_SIZE);
		memset(node->levelNext, 0, sizeof(node->levelNext));
		node->levelHeight = 0;
		node->key = NULL, node->value = NULL;
		node->state = NODE_EMPTY;

		return node;
	}
	inline void _ClearCmpCount(){
		_cmpCount = 0;
	}
	inline void _PrintCmpCount(){
		std::cout<<"Compare Count:"<<_cmpCount<<std::endl;
	}
	_SkipNode dummyHead;
	_SkipNode dummyTail;
	size_t nodeCount;
	unsigned int randomSeed;
	mutable size_t _cmpCount;
};

template<typename _Comparator, typename _Allocator, typename _Randomer>
_SkipNode* _SkipList<_Comparator, _Allocator, _Randomer>::_searchNode(const char* key){
    int level;
    _SkipNode *cur, *prev;

    //Retrieving from the heighest level
	prev = &dummyHead;
	for(level = LIST_HEIGHT-1; level >= 0; level--){
		cur = prev->levelNext[level];
		while(cur != &dummyTail)//not cur != NULL now...
		{
			int _cmp = _compare(cur->key, key);
			if(_cmp == 0) return cur;
			else if(_cmp < 0) prev = cur, cur = cur->levelNext[level];
			else break;//retrieving from the lower level
		}
	}

	return NULL;
}

template<typename _Comparator, typename _Allocator, typename _Randomer>
int _SkipList<_Comparator, _Allocator, _Randomer>::Set(const char* key, const char* value){
    _SkipNode *cur, *prev;
	int level;

	prev = &dummyHead;
	for(level = LIST_HEIGHT-1; level >= 0; level--)
	{
		cur = prev->levelNext[level];
		while(cur != &dummyTail)
		{
			int _cmp = _compare(cur->key, key);
			if(_cmp == 0){
				cur->value = value;
				int result = QLEVEL_DATA_UPDATE;
				if(cur->state != NODE_NORMAL)
					cur->state = NODE_NORMAL, result = QLEVEL_DATA_INSERT;
				return result;//nodeCount is not changed
			}
			else if(_cmp < 0)
				prev = cur, cur = cur->levelNext[level];
			else break;//try to low down the retrieve level
		}
	}
	//come here, 'key' is not in the skiplist, insert a new node
	_SkipNode* node = _allocNode();//safe allocator, do not need to check cur
	node->key = key, node->value = value;
	node->state = NODE_NORMAL;
	node->levelHeight = _getRandom(0, LIST_HEIGHT-1);

	int delt_height = node->levelHeight - prev->levelHeight;
	if(delt_height < 0){//rand_height is lower than prev->levelHeight
		for(int _lev = 0; _lev <= node->levelHeight; _lev++)
		{
			node->levelNext[_lev] = prev->levelNext[_lev];
			prev->levelNext[_lev] = node;
		}
	}
	else{
		int _lev = 0;
		for(; _lev <= prev->levelHeight; _lev++)
		{
			node->levelNext[_lev] = prev->levelNext[_lev];
			prev->levelNext[_lev] = node;
		}

		prev = &dummyHead;
		for(; _lev <= node->levelHeight; _lev++)
		{
			cur = prev->levelNext[_lev];
			while(cur != &dummyTail)
			{
				int _cmp = _compare(cur->key, key);
				if(_cmp < 0)//cur->key < key, forward
					prev = cur, cur = cur->levelNext[_lev];
				else if(_cmp > 0)// prev->key < key < cur->key, just insert here
					break;
				else //can't be _cmp == 0, error in the skiplist
					;
			}

			node->levelNext[_lev] = prev->levelNext[_lev];
			prev->levelNext[_lev] = node;
		}
	}
	nodeCount++;
	return QLEVEL_DATA_INSERT;
}

template<typename _Comparator, typename _Allocator, typename _Randomer>
int _SkipList<_Comparator, _Allocator, _Randomer>::Get(const char* key, const char** value) {
	_ClearCmpCount();
	_SkipNode *node = _searchNode(key);
	int _result;
	if(node == NULL || node->state != NODE_NORMAL)
		*value = NULL, _result = QLEVEL_NOT_FOUND;
    else
		*value = node->value, _result = QLEVEL_SUCCESS;
	_PrintCmpCount();
	return _result;
}

template<typename _Comparator, typename _Allocator, typename _Randomer>
int _SkipList<_Comparator, _Allocator, _Randomer>::Delete(const char* key){
	_SkipNode *cur, *prev, *node;
	int level;

	node = _searchNode(key);
	if(node == NULL) return QLEVEL_NOT_FOUND;

	for(level = LIST_HEIGHT-1; level >= 0; level--)
	{
		prev = &dummyHead;
		if(node->levelNext[level] == &dummyTail) continue;
		cur = prev->levelNext[level];
		while(cur != &dummyTail && _compare(cur->key, key) < 0)
			prev = cur, cur = cur->levelNext[level];
		if(cur == node)
			prev->levelNext[level] = node->levelNext[level];
	}
	//deallocate(node)here...
	nodeCount--;
	return QLEVEL_DATA_DELETE;
}

template<typename _Comparator, typename _Allocator, typename _Randomer>
void _SkipList<_Comparator, _Allocator, _Randomer>::Clear(){
	_SkipNode* prev, *cur;

	prev = &dummyHead;
	while(1)
	{
		cur = prev->levelNext[0];
		if(cur == &dummyTail) break;
		prev->levelNext[0] = cur->levelNext[0];
		_Allocator::deallocate(cur);
		nodeCount--;
	}

	//if(nodeCount) THROW{};//unknow error, should be zero here
	memset(&dummyHead, 0, sizeof(dummyHead));
	for(int _lev = 0; _lev < MAX_HEIGHT; _lev++)
		dummyHead.levelNext[_lev] = &dummyTail;
	dummyHead.levelHeight = LIST_HEIGHT - 1;
	_ClearCmpCount();
}

typedef _SkipList<Comparator_psz, Simple_allocator, Simple_randomer> SkipList;

}//!namespace Qlevel

#endif //!_SKIPLIST_H_
