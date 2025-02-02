#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <res/res_itk.h>
#include <tccore/aom.h>
#include <lov/lov.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	tag_t tLOV = NULLTAG;
	char** val = NULLTAG;
	int numvalues = 0;
	
	iFail = ITK_init_module("izn", "izn", "dba"); //Attempts an automatic login first by assuming sufficient data is available on the command line.
	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		iFail = ITEM_find_item("000199", &tItem); //Searches the database for all Items with IDs that match the given string.
		if (tItem != NULLTAG) {
			AOM_ask_lov(tItem, "a2Fruit", &tLOV);
			LOV_ask_values_string(tLOV, &numvalues, &val);
			for (int i = 0; i < numvalues; i++)
			{
				if (strcmp(val[i], "apple") == 0 || strcmp(val[i], "") == 0)
				{
					AOM_refresh(tItem, true);
					cout << "\n\n Object CheckedOut Successfully..\n";
					AOM_set_value_string(tItem, "a2Fruit", "banana");
					AOM_save_without_extensions(tItem);
					cout << "..Done..";
					AOM_refresh(tItem, false);
					cout << "\n\n Object CheckedIn Successfully..\n";
				}
				else 
				{
					continue;
				}
			}
		}
		else 
		{
			EMH_ask_error_text(iFail, &cError);
			cout << "\n\n The error is : " << cError;
		}
	}
	else 
	{
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : " << cError;
	}
	return iFail;
}