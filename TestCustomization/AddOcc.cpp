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
	tag_t tRev = NULLTAG;
	tag_t tAdd = NULLTAG;
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	tag_t* childs = NULLTAG;
	int n_occurrences = 3;
	int c = 0;
	int cc = 0;
	tag_t* bvr = NULLTAG;
	double qty = 5;
	tag_t* occ = NULLTAG;
	char* name = NULL;
	char* dd = NULL;
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

			ifail = ITEM_find_rev("000364", "A", &tRev);
			checkifail();
			checkNullTag(tRev);
			cout << "Parent found\n\n";
			ifail = ITEM_find_rev("000371", "A", &tAdd);
			checkifail();
			checkNullTag(tAdd);
			cout << "Found Occurrence to add\n\n";
			ifail = BOM_create_window(&tWindow);
			checkifail();
			checkNullTag(tWindow);
			cout << "Created window \n\n";
			ifail = BOM_set_window_top_line(tWindow, NULLTAG, tRev, NULLTAG, &tTopLine);
			checkifail();
			checkNullTag(tTopLine);
			cout << "Top line has been set\n\n";
			
			ifail = ITEM_rev_list_bom_view_revs(tRev, &c, &bvr);
			cout << c << endl;

			AOM_ask_value_string(bvr[0], "object_string", &name);
			cout << name << endl;

			ifail = PS_create_occurrences(bvr[0], tAdd, NULLTAG, n_occurrences, &occ);
			checkifail();

			for (int i = 0; i < n_occurrences; i++)
			{
				ifail = PS_set_occurrence_qty(bvr[0], occ[i], qty);
				checkifail();
			}
			BOM_save_window(tWindow);

			BOM_line_ask_all_child_lines(tTopLine, &cc, &childs);

			for (int i = 0; i < cc; i++)
			{
				AOM_ask_value_string(childs[i], "bl_indented_title", &dd);
					cout << dd << endl;
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