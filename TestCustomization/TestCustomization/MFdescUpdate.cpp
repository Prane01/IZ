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
#include<tccore/grm.h>
using namespace std;

void display();
int checkNullTag(tag_t tag);

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	tag_t tRelationType = NULLTAG;
	int count = 0;
	tag_t* tItemMasterForm = NULLTAG;

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
			ITEM_find_item("000279", &tItem);
			checkNullTag(tItem);
			cout << "Item found...!";
			GRM_find_relation_type("IMAN_master_form", &tRelationType);
			checkNullTag(tRelationType);
			GRM_list_secondary_objects_only(tItem, tRelationType, &count, &tItemMasterForm);
			checkNullTag(tItemMasterForm[0]);
			cout << "Item Master Form found found...!";
			AOM_refresh(tItemMasterForm[0], true);
			AOM_set_value_string(tItemMasterForm[0], "object_desc", "Hello");
			AOM_save_without_extensions(tItemMasterForm[0]);
			cout << "Item Master Form description updated...!";
			AOM_refresh(tItemMasterForm[0], false);
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
		cout << "tag ok\n";
		return 0;
	}
	return ITK_ok;
}