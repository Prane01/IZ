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
//#define uname "izn"
//#define pass "izn"
//#define grp "dba"
using namespace std;

void display();
int checkNullTag(tag_t);
void findChilds(tag_t*, int);
int ifail = 0;
char* cError = NULL;
tag_t* tchildrenInner = NULLTAG;
char* aa = NULL;
char* bb = NULL;
int again = 0;
int c2 = 0;
int ITK_user_main(int argc, char* argv[])
{
	tag_t tRevisionP = NULLTAG;
	tag_t BOMwindow = NULLTAG;
	tag_t TopBomLine = NULLTAG;
	tag_t GTopBomLine = NULLTAG;
	int c = 0;
	tag_t* tchildren = NULLTAG;
	tag_t tarrangement = NULLTAG;


	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}
	cout << "HIIII";
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
			
			AOM_ask_value_string(tRevisionP, "object_string", &aa);
			cout << aa << endl;

			BOM_line_ask_all_child_lines(TopBomLine, &c, &tchildren);

			findChilds(tchildren, c);
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

void findChilds(tag_t* tchildren, int c) {
	tag_t p = NULLTAG;
	if (c == 0)
	{
		exit(0);
	}
	else
	{
		for(int i = 0; i < c; i++)
		{
			AOM_ask_value_string(tchildren[i], "bl_indented_title", &aa);
			cout << aa << endl;
			AOM_ask_value_tag(tchildren[i], "bl_parent", &p);
			AOM_ask_value_string(p, "bl_indented_title", &aa);
			cout << aa << endl;
			BOM_line_ask_all_child_lines(tchildren[i], &c2, &tchildrenInner);
			cout << c2 << endl;
			if (c2 > 0)
			{
				findChilds(tchildrenInner, c2);
			}
		}
	}
}