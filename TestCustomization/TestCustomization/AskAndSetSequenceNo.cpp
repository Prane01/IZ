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
using namespace std;

void display();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t tParent = NULLTAG;
	tag_t trevision = NULLTAG;
	tag_t tbv = NULLTAG;
	tag_t tbvr = NULLTAG;
	tag_t BOMwindow = NULLTAG;
	tag_t GTopBomLine = NULLTAG;
	tag_t TopBomLine = NULLTAG;
	tag_t* children = NULLTAG;
	int c = 0;
	char* bl_item_id = NULL;
	char* a = NULL;
	char* name = NULL;

	char* seq = NULL;

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
			ITEM_find_rev("000159", "A", &tParent);
			checkNullTag(tParent);
			cout << "found parent..\n\n";

			BOM_create_window(&BOMwindow);
			checkNullTag(BOMwindow);
			cout << "Created BOMwindow..\n\n";

			BOM_set_window_top_line(BOMwindow, NULLTAG, tParent, NULLTAG, &GTopBomLine);
			checkNullTag(GTopBomLine);
			cout << "Created BOMwindow top line..\n\n";
			BOM_ask_window_top_line(BOMwindow, &TopBomLine);
			checkNullTag(TopBomLine);
			AOM_ask_value_string(TopBomLine, "bl_indented_title", &a);
			cout << "Top line: "<< a << endl;

			BOM_line_ask_all_child_lines(TopBomLine, &c, &children);
			cout << c << endl;

			for (int i = 0; i < c; i++)
			{
				AOM_ask_value_string(children[i], "bl_item_item_id", &bl_item_id);
				if (strcmp(bl_item_id,"000182") == 0)
				{
					AOM_ask_value_string(children[i], "bl_indented_title", &name);
					cout << "Occurrence: " << name << endl;
					AOM_ask_value_string(children[i], "bl_sequence_no", &seq);
					cout << "Find number: " << seq << endl;
					AOM_set_value_string(children[i], "bl_sequence_no", "70");
					BOM_save_window(BOMwindow);
					AOM_ask_value_string(children[i], "bl_sequence_no", &seq);
					cout << "New Find number: " << seq << endl;
				}
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
		exit(0);
	}
	else
	{
		return 0;
	}
	return ITK_ok;
}