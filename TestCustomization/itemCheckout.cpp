#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <res/res_itk.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	iFail = ITK_init_module("izn", "izn", "dba");
	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		iFail = ITEM_find_item("152415", &tItem);
		if (tItem != NULLTAG) {
			cout << "\n\n Object Fetched Successfully..\n";
			iFail = RES_checkout2(tItem,"update through ITK",NULLTAG,NULLTAG,2);
			if (iFail == ITK_ok) {
				cout << "\n\n Object CheckedOut Successfully..\n";
			}
			else {
				cout << "\n\n The error is : Cannot check-out Object";
			}
		}
		else {
			cout << "\n\n The error is : Cannot Fetch Object";
		}
	}
	else {
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : %s\n", cError;
	}
	return iFail;
}