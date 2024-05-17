#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tc/folder.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	tag_t tRevision = NULLTAG;
	tag_t tUser = NULLTAG;
	tag_t tHomeFolder = NULLTAG;
	int hits = 0;
	tag_t* list = NULLTAG;
	tag_t tFolder = NULLTAG;
	tag_t tItem1 = NULLTAG;
	char* s = NULL;
	char* cc = NULL;
	char* s1 = NULL;

	iFail = ITK_init_module("izn", "izn", "dba");
	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		iFail = ITEM_create_item("152441", "ITK Test", "Item", "A", &tItem, &tRevision);
		if (iFail == ITK_ok) {
			iFail = ITEM_save_item(tItem);
			if (iFail == ITK_ok)
			{
				cout << "\n\n Item Created Successfully in DB..\n";
				iFail = WSOM_find2("ITK", &hits, &list);
				for (int i = 0; i < hits; i++) {
					AOM_ask_value_string(list[i], "object_name", &s);
					cout << "Object name: " << s << '\n';

					AOM_ask_value_string(list[i], "object_type", &s1);
					cout << "Type: " << s1 << '\n';
					
					if (strcmp(s1, "Folder") == 0)
					{
						iFail = FL_insert(*list, tItem, 0);
						AOM_save_without_extensions(*list);
						cout << "\n\n Item Saved Successfully..\n";
					}
					else {
						EMH_ask_error_text(iFail, &cError);
						cout << "\n\n The error is : " << cError;
					}
				}
			}
			else {
				EMH_ask_error_text(iFail, &cError);
				cout << "\n\n The error is : " << cError;
			}
		}
		else {
			EMH_ask_error_text(iFail, &cError);
			cout << "\n\n The error is : " << cError;
		}
	}
	else {
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : " << cError;
	}
	return iFail;
}