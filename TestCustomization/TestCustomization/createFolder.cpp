#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/folder.h>
#include <tccore/aom.h>
#include <tc/emh.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tFolder = NULLTAG;
	//iFail = ITK_init_module("izn", "izn", "dba");
	iFail = TC_auto_login();
	if(iFail == ITK_ok) {
		cout << "\n\n Login successful...";
		iFail = FL_create2("ITK Folder2","Objects related to ITK code", &tFolder);
		if (tFolder != NULLTAG) {
			cout << "\n\n Folder created successfully...";
			iFail = AOM_save(tFolder);
			if (iFail == ITK_ok) {
				cout << "\n\n Folder saved in DB...";
			}
			else {
				cout << "\n\n Folder not saved in DB...";
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