
#include <iostream>

#include "qlevel.h"
#include "qdatabase.h"

using namespace Qlevel;
using namespace std;

int main()
{
    QDataBase *db;
    int result = QDataBase::Open("test_db", &db);
    if(result == QLEVEL_SUCCESS) cout<<"DataBase open success"<<endl;
    else cout<<"DataBase open failed"<<endl;

    result = db->DataBaseSize();
    cout<<"Database size:"<<result<<endl;

    cout<<"Set 'Hello' - 'World' to the database"<<endl;
    db->Set("Hello", "World");
    result = db->DataBaseSize();
    cout<<"Database size: "<<result<<endl;

    cout<<"Set 'Good' - 'Bye' to the database"<<endl;
    db->Set("Good", "Bye");
    result = db->DataBaseSize();
    cout<<"Database size:"<<result<<endl;

	db->Close();

	return 0;
}
