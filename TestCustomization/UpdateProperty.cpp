#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <res/res_itk.h>
#include <tccore/aom.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	//string desc = "ITK description set";
	//strcpy_s(char_array, 10, desc.c_str());
	iFail = ITK_init_module("infodba", "infodba", "dba"); //Attempts an automatic login first by assuming sufficient data is available on the command line.
	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		iFail = ITEM_find_item("152415", &tItem); //Searches the database for all Items with IDs that match the given string.
		if (tItem != NULLTAG) {
			cout << "\n\n Object Fetched Successfully..\n";
			iFail = RES_checkout2(tItem, "update through ITK", NULLTAG, NULLTAG, 2); /*Checks out @p object for @p user. If @p user is #NULLTAG it will checkout @p object for the logged in user.
			The other arguments set attributes in the ImanReservation object that is created as a result of a check - out.*/
			if (iFail == ITK_ok) {
				cout << "\n\n Object CheckedOut Successfully..\n";
				iFail = AOM_set_value_string(tItem, "object_desc", "ITK description set"); //Sets one or more values on a property.
				//iFail = ITEM_set_description(tItem, "ITK description set");
				if (iFail == ITK_ok) {
					iFail = AOM_save(tItem); /*Saves the object and executes the PreCondition, PreAction and PostAction extensions if TC_save_msg is implemented on the type of the object;
					For example, this generic ITK will execute TC_save_msg on Item in the same way as ITEM_save_item does.*/
					if (iFail == ITK_ok) {
						cout << "\n\n Description updated Successfully..\n";
						iFail = RES_checkin(tItem); //check-in's the particular object which is checked out
						if (iFail == ITK_ok) {
							cout << "\n\n Object CheckedIn Successfully..\n";
						}
						else {
							cout << "\n\n The error is : Cannot check-in Object";
						}
					}
					else {

						cout << "\n\n The error is : Cannot update Description";
					}
				}
			}
				else {
					EMH_ask_error_text(iFail, &cError);
					cout << "\n\n The error is : Cannot checkout Object ", cError;
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
	return iFail;
}