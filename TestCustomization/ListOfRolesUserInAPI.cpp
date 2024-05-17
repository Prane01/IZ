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
#include<sa/sa.h>
#include<sa/groupmember.h>
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	char* uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char* pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char* grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			tag_t tUser = NULLTAG;
			int num_mem = 0;
			tag_t* members = NULLTAG;
			char* role = NULL;
			SA_find_user2("infodba", &tUser);
			SA_find_all_groupmember_by_user(tUser, true, &num_mem, &members);
			for (int i = 0; i < num_mem; i++)
			{
				PROP_ask_composite_property(members[i], "role", &role);
				if (tc_strcmp(role, "") != 0)
				{
					cout << "Role : " << role << endl;
				}
			}
		}
	}
	return ifail;
}
void display() {
	cout << "\n -u\t ---> Enter username";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
}
int checkifail()
{
	if (ifail != ITK_ok)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "Error: " << cError;
	}
	else
	{
		return 0;
	}
}
int checkNullTag(tag_t tag) {
	if (tag == NULLTAG)
	{
		exit(0);
	}
	else
	{
		cout << "tag ok\n";
		return 0;
	}
}