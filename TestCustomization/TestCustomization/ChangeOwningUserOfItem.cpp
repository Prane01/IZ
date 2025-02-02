#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <sa/user.h>
#include <tccore/aom_prop.h>
#include <tccore/aom.h>
#include <res/res_itk.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	tag_t tUser = NULLTAG;
	tag_t tOwning_group = NULLTAG;
	char* s = NULL;
	iFail = ITK_init_module("izn", "izn", "dba");

	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		if (iFail == ITK_ok)
		{
			/*AOM_ask_value_string(tUser, "object_string", &s);
			cout << s << '\n';*/
			iFail = ITEM_find_item("152416", &tItem);
			if (tItem != NULLTAG)
			{
				cout << "\n\n Object Present in the DB..\n";

				iFail = SA_find_user2("izn", &tUser); // getting user into tUser
				iFail = SA_find_group("dba", &tOwning_group); // getting group into tOwning_group
				iFail = ITK_set_bypass(true); // bypass to get privileges..

				if (tUser != NULLTAG && tOwning_group != NULLTAG) // validate for not null
				{
					iFail = AOM_set_ownership(tItem, tUser, tOwning_group); // setting owning_user and owning_group to tItem
					if (iFail == ITK_ok)
					{
						cout << "Ownership changed..";
						AOM_save(tItem);
					}
					else {
						cout << "\n\n The error is : Ownership Not changed ", cError;
					}
				}
				else {
					EMH_ask_error_text(iFail, &cError);
					cout << "\n\n The error is : Cannot Fetch Object ", cError;
				}
			}
				
			else {
				EMH_ask_error_text(iFail, &cError);
				cout << "\n\n The error is : Cannot Fetch Object ", cError;
			}
		}
		else {
			EMH_ask_error_text(iFail, &cError);
			cout << "\n\n The error is : %s\n", cError;
		}
	}
	else {
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : %s\n", cError;
	}	
	return iFail;
}