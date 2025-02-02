#include <iostream>
#include<tc/tc_startup.h>
#include<tcinit\tcinit.h>
#include "MyException.h"
using namespace std;

int ITK_user_main(int argc, char* argv[])
{
	MyException obj;
	try {
		obj = ITK_init_module("infodba", "infodba", "dbjoa"); // Login to server
	}
	catch(MyException& obj){
		cout << "Error is: " << obj << endl;
	}
	return 0;
}
