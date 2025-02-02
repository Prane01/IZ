#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	bool flag = false;
	//string desc = "ITK description set";
	char* char_array;
	//strcpy_s(char_array, 10, desc.c_str());
	iFail = ITK_init_module("izn", "izn", "dba");
	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		iFail = ITEM_find_item("152415", &tItem);
		if (tItem != NULLTAG)
		{
			cout << "\n\n Object Present in the DB..\n";
		}
		else {
			EMH_ask_error_text(iFail, &cError);
			cout << "\n\n The error is : Object not found in DB\n", cError;
		}
	}
	else {
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : %s\n", cError;
	}
	return iFail;
}