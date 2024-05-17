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

int ITK_user_main(int argc, char* argv[])
{
	tag_t tTop = NULLTAG;
	tag_t tRevision = NULLTAG;
	tag_t tLevel = NULLTAG;
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	int cc = 0;
	tag_t* children = NULLTAG;
	char* item_id = NULL;
	bool flag = true;

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
			ITEM_find_rev("000364", "A", &tTop);
			checkNullTag(tTop);
			cout << "Found Top line...\n\n";
			ITEM_find_rev("000370", "A", &tLevel);
			checkNullTag(tTop);
			cout << "Found Insert line...\n\n";
			BOM_create_window(&tWindow);
			checkNullTag(tTop);
			cout << "Created Window...\n\n";
			BOM_set_window_top_line(tWindow, NULLTAG, tTop, NULLTAG, &tTopLine);
			checkNullTag(tTop);
			cout << "Created Top line on window...\n\n";
			BOM_line_ask_all_child_lines(tTopLine, &cc, &children);

			if (cc > 0)
			{
				cout << "Childs found: " << cc << endl;;
				for (int i = 0; i < cc; i++)
				{
					AOM_ask_value_string(children[i], "bl_item_item_id", &item_id);
					if (strcmp(item_id,"000366") == 0)
					{
						flag = false;
						BOM_line_insert_level(1, &children[i], tLevel, NULLTAG);
						cout << "Successfully inserted bom level...\n\n";
						BOM_save_window(tWindow);
					}
				}
			}
			if (flag)
			{
				cout << "Cannot find bom line required...\n\n";
			}
			else
			{
				cout << "No childs found...\n\n";
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