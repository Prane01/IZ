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
#include<cfm/cfm.h>
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

	tag_t tRule = NULLTAG;
	tag_t tWindow = NULLTAG;
	tag_t configRule = NULLTAG;
	tag_t tItem = NULLTAG;
	tag_t tTopBOM = NULLTAG;
	int c = 0;
	tag_t* childs = NULLTAG;
	char* name = NULL;

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
			ifail = ITEM_find_rev("000364","A", &tItem);
			checkNullTag(tItem);
			cout << "Item found...\n\n";
			ifail = BOM_create_window(&tWindow);
			checkNullTag(tWindow);
			cout << "Created Window...\n\n";
			ifail = CFM_find("Test001", &tRule);
			checkNullTag(tRule);
			cout << "Rule found...\n\n";

			ifail = BOM_set_window_config_rule(tWindow, tRule);
			checkNullTag(tRule);
			cout << "Rule configured...\n\n";

			ifail = BOM_set_window_pack_all(tWindow, true);
			cout << "Packed all bomlines...\n\n";

			ifail = BOM_set_window_top_line(tWindow, NULLTAG, tItem, NULLTAG, &tTopBOM);
			checkNullTag(tTopBOM);

			cout << "Top bom line set...\n\n";

			ifail = BOM_save_window(tWindow);
			BOM_line_ask_all_child_lines(tTopBOM, &c, &childs);
			cout << c << endl;
			
			for (int i = 0; i < c; i++)
			{
				AOM_ask_value_string(childs[i], "bl_indented_title", &name);
				cout << name << endl;
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