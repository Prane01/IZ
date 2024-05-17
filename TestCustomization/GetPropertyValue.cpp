#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	char* value = NULL;
	iFail = ITK_init_module("infodba", "infodba", "dba");
	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		iFail = ITEM_find_item("000199", &tItem);
		if (tItem != NULLTAG) {
			cout << "\n\n Item Found ...\n";
			iFail = AOM_ask_value_string(tItem, "object_desc", &value);
			if (iFail == ITK_ok) {
				cout << "\n\n Property: " << value;
			}
			else {
				EMH_ask_error_text(iFail, &cError);
				cout << "\n\n The error is : Cannot get property..", cError;
			}
		}
		else {
			cout << "\n\n The error is : Item not found";
		}
	}
	else {
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : %s\n", cError;
	}
	return iFail;

}