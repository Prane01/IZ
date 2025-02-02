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
#include<pom/pom/pom.h>
#include<ps/ps.h>
#include<bom/bom.h>
//#define uname "izn"
//#define pass "izn"
//#define grp "dba"
using namespace std;

void display();
int checkNullTag(tag_t tag);
void TopLine();
int ifail = 0;
char* cError = NULL;
tag_t tRevisionP = NULLTAG;
tag_t tRevisionL1C1 = NULLTAG;
tag_t tRevisionL1C2 = NULLTAG;
tag_t tRevisionL1C3 = NULLTAG;
tag_t tRevisionL2C1 = NULLTAG;
tag_t tRevisionL2C2 = NULLTAG;
tag_t tbv = NULLTAG;
tag_t tbvr = NULLTAG;
tag_t BomView = NULLTAG;
tag_t BOMwindow = NULLTAG;
tag_t TopBomView = NULLTAG;
tag_t newLine = NULLTAG;
tag_t tItem = NULLTAG;
int c = 2;
tag_t* parents = new tag_t[2];
tag_t* collection1 = new tag_t[3];
tag_t* collection2 = new tag_t[2];
int counter = 0;

int ITK_user_main(int argc, char* argv[])
{
	

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
			ifail = ITEM_find_rev("000374","A", &tRevisionP);
			checkNullTag(tRevisionP);
			parents[0] = tRevisionP;
			cout << "Top Parent ItemRevision found\n\n";
			ifail = ITEM_find_rev("000375", "A", &tRevisionL1C1);
			checkNullTag(tRevisionL1C1);
			collection1[0] = tRevisionL1C1;
			parents[1] = tRevisionL1C1;
			cout << "Parent1 ItemRevision found\n\n";
			ifail = ITEM_find_rev("000376", "A", &tRevisionL1C2);
			checkNullTag(tRevisionL1C2);
			collection1[1] = tRevisionL1C2;
			cout << "Child ItemRevision found\n\n";
			ifail = ITEM_find_rev("000377", "A", &tRevisionL1C3);
			checkNullTag(tRevisionL1C3);
			collection1[2] = tRevisionL1C3;
			cout << "Child ItemRevision found\n\n";
			ifail = ITEM_find_rev("000378", "A", &tRevisionL2C1);
			checkNullTag(tRevisionL2C1);
			collection2[0] = tRevisionL2C1;
			cout << "Child ItemRevision found\n\n";
			ifail = ITEM_find_rev("000379", "A", &tRevisionL2C2);
			checkNullTag(tRevisionL2C2);
			collection2[1] = tRevisionL2C2;
			cout << "Child ItemRevision found\n\n";

			TopLine();
			for (int i = 0; i <= sizeof(collection1)/sizeof(tag_t); i++)
			{
				BOM_line_add(TopBomView, NULLTAG, collection1[i], NULLTAG, &newLine);
				checkNullTag(newLine);
				AOM_save_without_extensions(newLine);
				cout << "Added ItemRevision(Child) to top line ItemRevision..\n\n";
				newLine = NULLTAG;
			}
			AOM_save_without_extensions(tbv);
			AOM_save_without_extensions(tbvr);
			AOM_save_without_extensions(BOMwindow);
			TopLine();
			for (int i = 0; i <= sizeof(collection2) / sizeof(tag_t); i++)
			{
				BOM_line_add(TopBomView, NULLTAG, collection2[i], NULLTAG, &newLine);
				checkNullTag(newLine);
				AOM_save_without_extensions(newLine);
				cout << "Added ItemRevision(Child) to top line ItemRevision..\n\n";
				newLine = NULLTAG;
			}
			AOM_save_without_extensions(tbv);
			AOM_save_without_extensions(tbvr);
			AOM_save_without_extensions(BOMwindow);

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

void TopLine()
{
	PS_create_bv_with_bvr(NULL, NULL, NULLTAG, NULL, NULL, false, parents[counter], &tbv, &tbvr);
	checkNullTag(tbv); checkNullTag(tbvr);
	cout << "Created bv and bvr..\n\n";

	BOM_create_window(&BOMwindow);
	checkNullTag(BOMwindow);
	cout << "Created BOMwindow..\n\n";

	BOM_set_window_top_line(BOMwindow, NULLTAG, parents[counter], tbv, &TopBomView);
	checkNullTag(TopBomView);
	cout << "Created BOMwindow top line..\n\n";
	counter++;
}
