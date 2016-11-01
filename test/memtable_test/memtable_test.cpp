
#include <iostream>
#include "qdatabase.h"
#include "db/skiplist.h"

using namespace std;

int main(){
	Qlevel::SkipList sp;
	const char* psz;

	cout<<"sp::num:"<<sp.NodeNum()<<endl;
	sp.Set("Hello", "World");
	sp.Set("Good", "Bye");
	sp.Set("See you", "Later");
	cout<<"sp::num:"<<sp.NodeNum()<<endl;

	cout<<"sp.Get(\"Hello\"):"<<sp.Get("Hello", &psz)<<endl;
	cout<<"The psz:"<<psz<<endl;

	cout<<"sp.Get(\"Good\"):"<<sp.Get("Good", &psz)<<endl;
	cout<<"The psz:"<<psz<<endl;

	cout<<"sp.Get(\"See you\"):"<<sp.Get("See you", &psz)<<endl;
	cout<<"The psz:"<<psz<<endl;

	//sp.PrintStatis();

	Qlevel::SkipList::iterator iter;
	for(iter = sp.Begin(); iter != sp.End(); iter++)
		cout<<"iter->key:"<<iter->key<<"  iter->value:"<<iter->value<<endl;

    Qlevel::SkipList::const_iterator const_iter;
	const_iter = sp.Begin();
	while(const_iter != sp.End()){
		cout<<"(*const_iter).key: "<<(*const_iter).key<<" (*const_iter).value:"<<(*const_iter).value<<endl;
		const_iter++;
	}

	return 0;
}
