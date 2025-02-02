#include<stdio.h>
#include<conio.h>
#include<ostream>
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
#include<datetimeapi.h>
#include<pom/pom/pom.h>
#include<ctime>
#define POM_bypass_attr_update              3011
using namespace std;

void display();

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	
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
			ifail = ITEM_find_item("000023", &tItem);
			cout << "found" << endl;
			POM_set_env_info(POM_bypass_attr_update, false, 0, 0, NULLTAG, NULL);
			AOM_refresh(tItem, true);
			ITEM_set_description(tItem, "CFHCHGACH");
			AOM_save_without_extensions(tItem);
			AOM_refresh(tItem, true);
			cout << "\ncheck last_mod_user.\n";
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