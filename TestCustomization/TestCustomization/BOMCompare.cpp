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
#define BOM_compare_multilevel                  2

#define BOM_compare_output_report   4

using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t tWindow = NULLTAG;
	tag_t tWindow2 = NULLTAG;
	tag_t tCompare = NULLTAG;
	tag_t tTop1 = NULLTAG;
	tag_t tTop2 = NULLTAG;
	tag_t tRev1 = NULLTAG;
	tag_t tRev2 = NULLTAG;
	tag_t* tChilds1 = NULLTAG;
	tag_t* tChilds2 = NULLTAG;
	int c1 = 0;
	int c2 = 0;
	int outputs_to = 0;
	int reportLength = 0;
	char** reportLines = NULL;
	tag_t* reportItems = NULLTAG;
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

			ITEM_find_rev("000355", "A", &tRev1);
			checkNullTag(tRev1);
			cout << "found rev1\n\n";
			ITEM_find_rev("000356", "A", &tRev2);
			checkNullTag(tRev2);
			cout << "found rev2\n\n";
			BOM_create_window(&tWindow);
			checkNullTag(tWindow);
			cout << "found tWindow\n\n";
			BOM_create_window(&tWindow2);
			checkNullTag(tWindow2);
			cout << "found tWindow\n\n";
			BOM_set_window_top_line(tWindow, NULLTAG, tRev1, NULLTAG, &tTop1);
			checkNullTag(tTop1);
			/*AOM_ask_value_string(tTop1, "bl_indented_title", &name);
			cout << name << endl;*/
			cout << "found tTop1\n\n";
			BOM_set_window_top_line(tWindow2, NULLTAG, tRev2, NULLTAG, &tTop2);
			checkNullTag(tTop2);
			cout << "found tTop2\n\n";
			BOM_line_ask_all_child_lines(tTop1, &c1, &tChilds1);
			cout << c1 << endl;
			BOM_line_ask_all_child_lines(tTop2, &c2, &tChilds2);
			cout << c2 << endl;
			BOM_compare_create(&tCompare);
			checkNullTag(tCompare);
			cout << "found tCompare\n\n";
			BOM_compare_execute(tCompare, tTop1, tTop2, BOM_std_compare_single_level_name, BOM_compare_output_report);
			cout << "found outputs_to\n\n";
			BOM_compare_report(tCompare, &reportLength, &reportLines, &reportItems);
			cout << reportLength << endl;
			cout << "found reportItems\n\n";
			for (int i = 0; i < reportLength; i++)
			{
				cout << reportLines[i] << endl;
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