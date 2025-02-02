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

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			tag_t tWindow = NULLTAG;
			tag_t tTopLine = NULLTAG;
			tag_t tItem = NULLTAG;
			tag_t baseline = NULLTAG;
			int cRev = 0;
			tag_t* revList = NULLTAG;

			ifail = ITEM_find_item(item_id, &tItem);
			checkifail(); checkNullTag(tItem);
			ifail = ITEM_list_all_revs(tItem, &cRev, &revList);
			cout << "good\n\n";
			ifail = BOM_create_window(&tWindow);
			checkifail();
			cout << "good\n\n";
			ifail = BOM_set_window_top_line(tWindow, tItem, NULLTAG, NULLTAG, &tTopLine);
			checkifail();
			cout << "good\n\n";
			ifail = BOM_create_baseline(tWindow, NULL, "Baseline created from ITK", "BaselineTest01", NULL, NULL, &baseline);
			checkifail(); checkNullTag(baseline);
			cout << "good\n\n";
			AOM_save_without_extensions(baseline);
			BOM_save_window(tWindow);
			cout << "Done\n\n";
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