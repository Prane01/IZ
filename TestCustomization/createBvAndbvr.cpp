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
#include<ps/ps.h>
#include<bom/bom.h>
#define uname "izn"
#define pass "izn"
#define grp "dba"
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t tItem = NULLTAG;
	tag_t tRev = NULLTAG;
	tag_t bv = NULLTAG;
	tag_t bvr = NULLTAG;
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	int c = 0;
	tag_t* BOMViews = NULLTAG;

	//char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	//char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	//char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	//if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

	//	display();
	//	return ifail;
	//}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";

			ITEM_find_item("000373", &tItem);
			ITEM_find_rev("000373", "A", &tRev);

			ifail = BOM_create_window(&tWindow);
			checkNullTag(tWindow);
			cout << "Created Window...\n\n";
			ifail = BOM_set_window_top_line(tWindow, NULLTAG, tRev, NULLTAG, &tTopLine);
			checkNullTag(tTopLine);
			cout << "Created Top line on window...\n\n";

			ifail = PS_create_bom_view(NULLTAG, "Praneeth", "Praneethhhh", tItem, &bv);
			checkNullTag(bv);
			ifail = AOM_save_without_extensions(bv);
			ifail = AOM_save_without_extensions(tItem);
			ifail = AOM_save_without_extensions(tWindow);

			ITEM_list_bom_views(tItem, &c, &BOMViews);

			ifail = PS_create_bvr(BOMViews[0], "ranjithh", "ranjithITK", false, tRev, &bvr);
			checkNullTag(bvr);
			ifail = AOM_save_without_extensions(bvr);
			ifail = AOM_save_without_extensions(tRev);
			ifail = AOM_save_without_extensions(tWindow);
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