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
#include<cfm/cfm.h>
#include<ps/ps.h>
#include<fclasses/tc_date.h>
#include<tccore/releasestatus.h>
#define CFM_entry_status TCM Released
//#define uname "izn"
//#define pass "izn"
//#define grp "dba"
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
int o = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{

	tag_t rule = NULLTAG;
	tag_t tItem = NULLTAG;
	tag_t* status = NULLTAG;
	tag_t trev = NULLTAG;
	tag_t ss = NULLTAG;
	bool val = NULL;
	string d = "05-Aug-2023 00:00";
	date_t op;
	char* test = NULL;
	int type = 0;
	char* dd = new char[d.length()+1];

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

			ifail = ITEM_find_item("000364", &tItem);
			checkNullTag(tItem);
			ifail = ITEM_find_rev("000376","A", &trev);
			checkNullTag(trev);

			ifail = AOM_ask_value_tags(trev, "release_status_list", &o, &status);
			/*ifail = AOM_ask_value_string(status[0], "object_name", &test);
			cout << test << endl;*/
			cout << "Found Item...\n\n";
			ifail = CFM_rule_create("ITK2", "ITK test rule", &rule);
			ifail = AOM_save_without_extensions(rule);

			checkNullTag(rule);

			strcpy(dd, d.c_str());

			CFM_rule_add_entry(rule, 1, &ss);

			/*CFM_status_entry_set_status(ss, status[0]);
			AOM_save_without_extensions(ss);*/     //Not working for specific status to set at Has Status() 

			ifail = DATE_string_to_date_t(dd, &val, &op);
			ifail = CFM_rule_set_date(rule, op);
			//ifail = CFM_rule_set_unit(rule, 5);

			ifail = CFM_rule_set_end_item(rule, tItem);
			ifail = AOM_save_without_extensions(rule);
			cout << "Saved.....\n";
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