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
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	tag_t trevision = NULLTAG;
	tag_t* childs = NULLTAG;
	int c = 0;
	int c2 = 0;
	char* name = NULL;
	char* qq = NULL;
	tag_t* packed = NULLTAG;

	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char *search_id = ITK_ask_cli_argument("-s="); // API takes user input as group

	char *rev_id = ITK_ask_cli_argument("-i="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			ITEM_find_rev(search_id, rev_id, &trevision);
			BOM_create_window(&tWindow);
			checkNullTag(tWindow);
			cout << "BOM Window created...\n\n";
			BOM_set_window_top_line(tWindow, NULLTAG, trevision, NULLTAG, &tTopLine);
			checkNullTag(tTopLine);
			cout << "Top BOM Line created...\n\n";

			BOM_set_window_pack_all(tWindow, true);

			BOM_line_ask_all_child_lines(tTopLine, &c, &childs);
			cout << "c: " << c << endl;

			//for (int i = 0; i < c; i++)
			//{
			//	/*AOM_ask_value_string(childs[i], "bl_indented_title", &name);
			//	cout << name << endl;*/
			//	BOM_line_unpack(childs[i]);
			//	//BOM_line_pack(childs[i]);
			//	//BOM_line_split_occurrence("2", childs[i]);
			//}
			BOM_save_window(tWindow);
			cout << "BOM Lines unpacked...\n\n";
			BOM_line_ask_all_child_lines(tTopLine, &c, &childs);
			for (int j = 0; j < c; j++)
			{
				AOM_ask_value_string(childs[j], "bl_indented_title", &name);
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
		cout << "The Error is: ", cError;
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