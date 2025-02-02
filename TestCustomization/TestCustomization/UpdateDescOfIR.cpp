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
	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char *item_id = ITK_ask_cli_argument("-id="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			tag_t tItem = NULLTAG;
			int cRev = 0;
			tag_t* revList = NULLTAG;
			char* release = NULL;
			char* id = NULL;
			bool flag = false;
			ifail = ITEM_find_item(item_id, &tItem);
			checkifail(); checkNullTag(tItem);
			ifail = ITEM_list_all_revs(tItem, &cRev, &revList);
			cout << cRev << endl;
			checkifail();
			if (cRev > 0)
			{
				for (int i = cRev - 1; i >= 0; i--)
				{
					ifail = PROP_ask_composite_property(revList[i], "release_status_list", &release);
					checkifail();
					AOM_ask_value_string(revList[i], "item_revision_id", &id);
					cout << id << endl;
					if (tc_strcmp(release, "") == 0)
					{
						tc_strcat(id, " not released...");
						for (int j = 1; j <= 2; j++)
						{
							flag = true;
							AOM_refresh(revList[i-j], true);
							AOM_set_value_string(revList[i-j], "object_desc", id);
							AOM_save_without_extensions(revList[i - j]);
							AOM_refresh(revList[i-j], false);
							cout << "Updated Description...\n\n";
						}
					}
					else
					{
						tc_strcat(id, " already released...");
						flag = true;
						ITK_set_bypass(true);
						AOM_refresh(revList[i], true);
						AOM_set_value_string(revList[i], "object_desc", id);
						AOM_save_without_extensions(revList[i]);
						AOM_refresh(revList[i], false);
						ITK_set_bypass(false);
						cout << "Updated Description...\n\n";
					}
					if (flag)
					{
						i = 0;
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