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
#include<pom/pom/pom.h>
#include<ps/ps.h>
#include<bom/bom.h>
//#define uname "izn"
//#define pass "izn"
//#define grp "dba"
using namespace std;

void display();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;
int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	tag_t tRevisionP = NULLTAG;
	tag_t tRevisionC1 = NULLTAG;
	tag_t tRevisionC2 = NULLTAG;
	tag_t tbv = NULLTAG;
	tag_t tbvr = NULLTAG;
	tag_t BomView = NULLTAG;
	tag_t BOMwindow = NULLTAG;
	tag_t TopBomView = NULLTAG;
	tag_t newLine = NULLTAG;
	tag_t tItem = NULLTAG;

	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			ifail = ITEM_find_rev("000330", "A", &tRevisionP);
			checkNullTag(tRevisionP);
			cout << "Parent ItemRevision found\n\n";
			ITEM_find_item("000331", &tItem);
			ifail = ITEM_find_rev("000331", "A", &tRevisionC1);
			checkNullTag(tRevisionC1);
			cout << "Child ItemRevision found\n\n";
			ifail = ITEM_find_rev("000332", "A", &tRevisionC2);
			checkNullTag(tRevisionC2);
			cout << "Child ItemRevision found\n\n";

			PS_create_bv_with_bvr(NULL, NULL, NULLTAG, NULL, NULL, false, tRevisionP, &tbv, &tbvr);
			checkNullTag(tbv); checkNullTag(tbvr);
			cout << "Created bv and bvr..\n\n";

			BOM_create_window(&BOMwindow);
			checkNullTag(BOMwindow);
			cout << "Created BOMwindow..\n\n";

			BOM_set_window_top_line(BOMwindow, NULLTAG, tRevisionP, tbv, &TopBomView);
			checkNullTag(TopBomView);
			cout << "Created BOMwindow top line..\n\n";

			BOM_line_add(TopBomView, NULLTAG, tRevisionC1, NULLTAG, &newLine);
			checkNullTag(newLine);
			cout << "Added ItemRevision(Child) to top line ItemRevision..\n\n";

			BOM_line_add(TopBomView, NULLTAG, tRevisionC2, NULLTAG, &newLine);
			checkNullTag(newLine);
			cout << "Added ItemRevision(Child) to top line ItemRevision..\n\n";

			AOM_save_without_extensions(tbv);
			AOM_save_without_extensions(tbvr);
			AOM_save_without_extensions(BOMwindow);
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
		cout << "The Error is: ", cError;
		exit(0);
	}
	else
	{
		return 0;
	}
	return ITK_ok;
}