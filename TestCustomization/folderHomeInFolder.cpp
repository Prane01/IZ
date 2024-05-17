#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/folder.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tccore/grm.h>
#include <sa/user.h>
#include <tccore/aom_prop.h>
#include <tccore/workspaceobject.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tHomeFolder = NULLTAG;
	tag_t tFolder = NULLTAG;
	tag_t tRelationType = NULLTAG;
	tag_t tRelation = NULLTAG;
	tag_t tUser = NULLTAG;
	//char* s = NULL;
	iFail = ITK_init_module("izn", "izn", "dba");
	//iFail = TC_auto_login();
	if (iFail == ITK_ok) {
		cout << "\n\n Login successful...";
		iFail = FL_create2("ITK Folder5", "Objects related to ITK code", &tFolder);
		if (tFolder != NULLTAG) {
			cout << "\n\n Folder created successfully...";
			iFail = AOM_save(tFolder);
		}
		else {
			EMH_ask_error_text(iFail, &cError);
			cout << "\n\n The error is: " << cError;
		}
	}
	else {
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is: " << cError;
	}
	if (iFail == ITK_ok)
	{
		iFail = SA_find_user2("izn", &tUser);
		if (tUser != NULLTAG)
		{
			iFail = SA_ask_user_home_folder(tUser, &tHomeFolder);
			//AOM_ask_value_string(tHomeFolder, "object_name", &s);
			//cout << s;
			if (iFail == ITK_ok)
			{
				iFail = FL_insert(tHomeFolder, tFolder, NULL);
				AOM_save(tHomeFolder);
				cout << "\n\n Folder saved in Home...";
			}
			else {
				cout << "\n\n Folder not saved in Home...";
			}
		}
		else {
			cout << "\n\n Cannot create Folder...";
		}
	}
	else {
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is: " << cError;
	}
	return iFail;
}