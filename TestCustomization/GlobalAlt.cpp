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
#include<me/meline.h>
//#define uname "izn"
//#define pass "izn"
//#define grp "dba"
using namespace std;

void display();
int checkNullTag(tag_t tag);
int checkifail();
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t tRevisionP = NULLTAG;
	tag_t tRevisionC1 = NULLTAG;
	tag_t tRevisionC2 = NULLTAG;
	tag_t tRevisionG1 = NULLTAG;
	tag_t tRevisionL1 = NULLTAG;
	tag_t BOMwindow = NULLTAG;
	tag_t tbv = NULLTAG;
	tag_t TopBomView = NULLTAG;
	int childCount = 0;
	int childCountLvl2 = 0;
	tag_t* children = NULLTAG;
	tag_t* childrenLvl2 = NULLTAG;
	char* bl_item_id = NULL;
	tag_t occLocal = NULLTAG;
	tag_t tItem = NULLTAG;
	int pCount = 0;
	tag_t* pbvrs = NULLTAG;
	tag_t newLine = NULLTAG;
	tag_t tGlobalAlt = NULLTAG;

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
			ifail = ITEM_find_rev("000343", "A", &tRevisionP);
			checkifail();
			checkNullTag(tRevisionP);
			cout << "Top Parent ItemRevision found\n\n";

			ifail = ITEM_find_rev("000345", "A", &tRevisionC2);
			checkifail();
			checkNullTag(tRevisionC2);
			cout << "Child ItemRevision found\n\n";

			ITEM_find_item("000337", &tItem);
			ifail = ITEM_find_rev("000337", "A", &tRevisionG1);
			checkifail();
			checkNullTag(tRevisionC2);
			cout << "Global alt ItemRevision found\n\n";

			ifail = BOM_create_window(&BOMwindow);
			checkifail();
			checkNullTag(BOMwindow);
			cout << "... BOMwindow ...\n\n";

			ifail = BOM_set_window_top_line(BOMwindow, NULLTAG, tRevisionP, NULLTAG, &TopBomView);
			checkifail();
			checkNullTag(TopBomView);

			ifail = BOM_line_ask_all_child_lines(TopBomView, &childCountLvl2, &childrenLvl2);
			checkifail();
			ifail = ITEM_rev_list_bom_view_revs(tRevisionP, &pCount, &pbvrs);
			checkifail();
			cout << pCount << endl;

			for (int i = 0; i < childCountLvl2; i++)
			{
				ifail = AOM_ask_value_string(childrenLvl2[i], "bl_item_item_id", &bl_item_id);
				checkifail();
				if (strcmp(bl_item_id, "000337") == 0)
				{
					cout << "Found object to add global alternate...\n\n";
					ifail =ITEM_add_related_global_alternates(tItem, 1, &tRevisionC2);
					checkifail();
					//AOM_save_without_extensions(tGlobalAlt);
					ifail = BOM_save_window(BOMwindow);
					checkifail();
					cout << "Yes..." << endl;
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