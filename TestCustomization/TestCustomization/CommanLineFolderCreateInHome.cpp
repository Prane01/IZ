#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/folder.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <sa/user.h>
using namespace std;

void display();
int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tFolder = NULLTAG;
	tag_t tHomeFolder = NULLTAG;
	tag_t tUser = NULLTAG;

	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char *foldername = ITK_ask_cli_argument("-f1=");

	char *fdesc = ITK_ask_cli_argument("-d1=");

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return iFail;
	}

	if (iFail == ITK_ok) {
		cout << "\n\n Login successful...";
		if (uname != NULL && pass != NULL && grp != NULL && foldername != NULL && fdesc != NULL) {  // Validates all values are filled
			if (ITK_init_module(uname, pass, grp) == ITK_ok && FL_create2(foldername, fdesc, &tFolder) == ITK_ok) {
				iFail = FL_create2(foldername, fdesc, &tFolder);
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
				iFail = FL_insert(tHomeFolder, tFolder, 999);
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

void display() {
	cout << "\n -u\t ---> Enter user name";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
	cout << "\n -f1\t ---> Enter folder name";
	cout << "\n -d1\t ---> Enter description";
}