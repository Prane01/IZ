#include<stdio.h>
#include<conio.h>
#include<iostream>
#include<fstream>
#include<tccore/tctype.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<tc/tc_startup.h>
#include<tcinit\tcinit.h>
#include<tccore/item.h>
#include<tc\emh.h>
#include<stdlib.h>
#include<string.h>
#include<pie/pie.h>
#include<pom/pom/pom.h>
//#define uname "izn"
//#define pass "izn"
//#define grp "dba"
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{


	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char *item = ITK_ask_cli_argument("-t="); // API takes user input as group

	char *item2 = ITK_ask_cli_argument("-t2="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			tag_t tItem = NULLTAG;
			tag_t tItem2 = NULLTAG;
			tag_t session = NULLTAG;
			tag_t transferMode = NULLTAG;
			void* plmxmlObj;
			int n_obj = 2;
			int fail = 0;
			tag_t* objects = new tag_t[2];

			ifail = ITEM_find_item(item, &tItem);
			checkifail();
			ifail = ITEM_find_item(item2, &tItem2);
			checkifail();
			objects[0] = tItem;
			objects[1] = tItem2;

			ifail = PIE_create_session(&session);
			checkifail();

			ifail = PIE_session_set_transfer_mode(session, transferMode);
			checkifail();

			ifail = PIE_session_set_file(session, "C:\\Users\\T50372\\Desktop\\ITKExp\\exprtITK.xml");
			checkifail();

			ifail = PIE_session_export_objects(session, n_obj, objects);
			checkifail();
			cout << "Exported file: C:\\Users\\T50372\\Desktop\\ITKExp\\exprtITK.xml\n\n";
		}
		else
		{
			EMH_ask_error_text(ifail, &cError);
			cout << "\n\n The error is : " << cError;
		}
	}
	else
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "\n\n The error is : " << cError;
	}
	return ifail;
}
void display() {
	cout << "\n -u\t ---> Enter username";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
}

int checkNullTag(tag_t tag)
{
	if (tag == NULLTAG)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "The Error is: " << cError;
		exit(0);
	}
	else
	{
		return 0;
	}
}

int checkifail()
{
	if (ifail != ITK_ok)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "Error: " << cError;
		exit(0);
	}
	else
	{
		return 0;
	}
}