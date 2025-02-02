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
#define uname "izn"
#define pass "izn"
#define grp "dba"
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;
char* search_id = NULL;
char* rev_id = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t tRevision = NULLTAG;
	tag_t tEndRev = NULLTAG;
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	const date_t* s = 01 / 9 / 2023;
	const date_t* e = 15 / 9 / 2023;

	const date_t* aa = { ;
	WSOM_open_ended_status_t status = EFFECTIVITY_open_ended;
	tag_t tEffObj = NULLTAG;

	//char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	//char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	//char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	//search_id = ITK_ask_cli_argument("-sid="); // API takes user input as group

	//rev_id = ITK_ask_cli_argument("-rid="); // API takes user input as group

	//if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

	//	display();
	//	return ifail;
	//}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			//ITEM_find_rev(search_id, rev_id, &tRevision);
			ITEM_find_rev("000365", "A", &tRevision);
			checkNullTag(tRevision);
			ITEM_find_rev("000364", "A", &tEndRev);
			checkNullTag(tEndRev);
			BOM_create_window(&tWindow);
			checkNullTag(tWindow);
			cout << "window created...\n\n";
			BOM_set_window_top_line(tWindow, NULLTAG, tRevision, NULLTAG, &tTopLine);
			checkNullTag(tTopLine);
			cout << "Top line created...\n\n";

			ifail = BOM_create_occurrence_effectivity(tTopLine, NULL, NULLTAG, tEndRev, NULL, 2, start, status, false, &tEffObj);
			checkifail();
			checkNullTag(tEffObj);

			cout << "eff created...\n\n";
			BOM_apply_occurrence_effectivity(tRevision, tEffObj);
			BOM_save_window(tWindow);

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