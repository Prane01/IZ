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
int checkNullTag(tag_t tag);
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
	tag_t ActTag = NULLTAG;
	int pCount = 0;
	tag_t* pbvrs = NULLTAG;
	tag_t newLine = NULLTAG;
	int n_sub = 0;
	tag_t* tSub = NULLTAG;
	tag_t value = NULLTAG;
	char* name = NULL;

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
			ifail = ITEM_find_rev("000343", "A", &tRevisionP);
			checkNullTag(tRevisionP);
			cout << "Top Parent ItemRevision found\n\n";
			ifail = ITEM_find_rev("000344", "A", &tRevisionC1);
			checkNullTag(tRevisionC1);
			cout << "Parent1 ItemRevision found\n\n";
			ifail = ITEM_find_rev("000338", "A", &tRevisionL1);
			checkNullTag(tRevisionL1);
			cout << "Local sub ItemRevision found\n\n";

			BOM_create_window(&BOMwindow);
			checkNullTag(BOMwindow);
			cout << "... BOMwindow ...\n\n";

			BOM_set_window_top_line(BOMwindow, NULLTAG, tRevisionP, NULLTAG, &TopBomView);
			checkNullTag(TopBomView);

			BOM_line_ask_all_child_lines(TopBomView, &childCountLvl2, &childrenLvl2);

			ITEM_rev_list_bom_view_revs(tRevisionP, &pCount, &pbvrs);
			cout << pCount << endl;

			for (int i = 0; i < childCountLvl2; i++)
			{
				AOM_ask_value_string(childrenLvl2[i], "bl_item_item_id", &bl_item_id);
				cout << bl_item_id << endl;
				if (strcmp(bl_item_id, "000344") == 0)
				{
					BOM_line_list_substitutes(childrenLvl2[i], &n_sub, &tSub);
					cout << n_sub << endl;
					AOM_ask_value_string(*tSub, "bl_indented_title", &name);
					cout << name << endl;
					if (n_sub != 0)
					{
						BOM_line_remove_related_substitutes(n_sub, tSub);
						AOM_save_without_extensions(childrenLvl2[i]);
						BOM_save_window(BOMwindow);
						cout << "Yes..." << endl;
					}
					else
					{
						cout << "Child object not found.....!!!!!!";
					}
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
	return ITK_ok;
}