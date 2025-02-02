#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include "MyException.h"
using namespace std;

int ITK_user_main(int arg, char* args[]) {
	tag_t tItem = NULLTAG;
	tag_t tRevision = NULLTAG;
	MyException ex;
	try {
		ex = ITK_init_module("izn", "izn", "dba");
		cout << "\n\n Login Successful..\n";
		ex = ITEM_create_item("234723", "Normal", "Item", "A", &tItem, &tRevision);
		cout << "\n\n Item Created Successfully..\n";
		ex = ITEM_save_item(tItem);
		cout << "\n\n Item Saved Successfully..\n";
	}
	catch(MyException err){
		err.error();
	}
	return 0;
}