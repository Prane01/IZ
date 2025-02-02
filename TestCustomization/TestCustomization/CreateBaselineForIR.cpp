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
#include<bom/bom.h>
#include<ps/ps.h>
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

	char *item_id = ITK_ask_cli_argument("-id="); // API takes user input as item_id

	char *rev_id = ITK_ask_cli_argument("-rid="); // API takes user input as rev_id

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			tag_t tRevision = NULLTAG;
			tag_t baseline = NULLTAG;
			tag_t newRev = NULLTAG;
			int deep_n = 0;
			tag_t* deep_objects = NULLTAG;
			char* dObj = NULL;
			char* release = NULL;
			ifail = ITEM_find_rev(item_id, rev_id, &tRevision);
			checkifail(); checkNullTag(tRevision);
			cout << "good\n\n";
			PROP_ask_composite_property(tRevision, "release_status_list", &release);
			if (tc_strcmp(release, "") == 0)
			{
				ifail = ITEM_baseline_rev(tRevision, NULL, "Baseline created from ITK", "BaselineTest01", NULL, NULL, &newRev, &deep_n, &deep_objects);
				checkifail();
				cout << deep_n << endl;
				for (int i = 0; i < deep_n; i++)
				{
					AOM_ask_value_string(deep_objects[i], "object_string", &dObj);
					cout << dObj << endl;
				}
				AOM_save_without_extensions(newRev);
				cout << "Done\n\n";
			}
			else {
				AOM_ask_value_string(tRevision, "object_string", &release);
				cout << "Can't create baseline as "<< release << " already released...\n\n";
			}
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