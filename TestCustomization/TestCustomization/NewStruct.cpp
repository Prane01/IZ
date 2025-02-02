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
#include<tc/folder.h>
#define uname "izn"
#define pass "izn"
#define grp "dba"
using namespace std;

void display();
int checkNullTag(tag_t tag);
char* findChilds(tag_t* tchildren, int c, int c2);
int ifail = 0;
char* cError = NULL;
tag_t* tchildrenInner = NULLTAG;
char* aa = NULL;
int again = 0;
tag_t tRevisionP = NULLTAG;
tag_t BOMwindow = NULLTAG;
tag_t TopBomLine = NULLTAG;
tag_t GTopBomLine = NULLTAG;
tag_t* tchildren = NULLTAG;
char* item_id = NULL;
char* revision_id = NULL;
tag_t ViewCheck = NULLTAG;

int ITK_user_main(int argc, char* argv[])
{
	
	int c = 0;
	int c2 = 0;
	tag_t tarrangement = NULLTAG;


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
			ifail = ITEM_find_rev("000159", "A", &tRevisionP);
			checkNullTag(tRevisionP);

			ifail = BOM_create_window(&BOMwindow);
			checkNullTag(BOMwindow);
			cout << "Created BOMwindow..\n\n";

			ifail = BOM_set_window_top_line(BOMwindow, NULLTAG, tRevisionP, NULLTAG, &GTopBomLine);
			checkNullTag(GTopBomLine);
			cout << "Created BOMwindow top line..\n\n";

			ifail = BOM_ask_window_top_line(BOMwindow, &TopBomLine);
			checkNullTag(TopBomLine);
			cout << "Found top line BOM..\n\n";

			//ifail = BOM_line_ask_used_arrangement(TopBomLine, &tarrangement);

			BOM_line_ask_all_child_lines(TopBomLine, &c, &tchildren);
			cout << c << endl << endl;

			AOM_ask_value_string(tRevisionP, "object_string", &aa);
			cout << aa << endl;

			findChilds(tchildren, c - 1, c2);
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
		exit(0);
	}
	else
	{
		return 0;
	}
	return ITK_ok;
}

char* findChilds(tag_t* tchildren, int c, int c2) {
	if (c < 0)
	{
		exit(0);
	}
	else
	{
		PS_ask_bom_view_of_bvr(tchildren[c], &ViewCheck);
		cout << ViewCheck << endl;
		cout << "checking.." << endl;
		if (ViewCheck != NULLTAG)
		{
			c--;
			cout << "Ifff.." << endl;
			AOM_ask_value_string(tchildren[c], "item_id", &item_id);
			AOM_ask_value_string(tchildren[c], "revision_id", &revision_id);
			ifail = ITEM_find_rev(item_id, revision_id, &tRevisionP);
			checkNullTag(tRevisionP);

			AOM_ask_value_string(tRevisionP, "object_string", &aa);
			cout << aa << endl;

			ifail = BOM_create_window(&BOMwindow);
			checkNullTag(BOMwindow);

			ifail = BOM_set_window_top_line(BOMwindow, NULLTAG, tRevisionP, NULLTAG, &GTopBomLine);
			checkNullTag(GTopBomLine);

			ifail = BOM_ask_window_top_line(BOMwindow, &TopBomLine);
			checkNullTag(TopBomLine);

			BOM_line_ask_all_child_lines(TopBomLine, &c, &tchildren);
			return findChilds(tchildren, c, c2);
		}
		else
		{
			c--;
			cout << "In else" << endl;
			while (c >= 0)
			{
				AOM_ask_value_string(tchildren[c], "object_string", &aa);
				cout << aa << endl;
				break;
			}
		}
		return findChilds(tchildren, c - 1, 0);
	}
}