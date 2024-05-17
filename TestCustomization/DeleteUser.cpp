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
#include<sa/user.h>
using namespace std;

void display();
void makeInactive(char* ActUser, int ActStatus);
void deleteUser(char* ActUser);
int ifail = 0;
char* cError = NULL;
tag_t tUser = NULLTAG;
tag_t tUserOW = NULLTAG;
tag_t tGroup = NULLTAG;
tag_t tPerson = NULLTAG;

int ITK_user_main(int argc, char* argv[])
{
	
	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username
	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password
	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group
	char *user = ITK_ask_cli_argument("-user="); // API takes user input as group
	char *status = ITK_ask_cli_argument("-status="); // API takes user input as group
	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			ITK_set_bypass(true);
			char* ActUser = user;
			int ActStatus = *status - '0';
			//makeInactive(ActUser, ActStatus);
			deleteUser(ActUser);
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
	cout << "\n -user\t ---> Enter User";
	cout << "\n -status\t ---> Enter status";
}

void makeInactive(char* ActUser, int ActStatus)
{
	SA_find_user2(ActUser, &tUser);
	AOM_lock(tUser);
	SA_set_user_status(tUser, ActStatus);
	if (ActStatus == 1)
	{
		cout << "... User status set to Inactive ...\n\n";
	}
	else if (ActStatus == 0) 
	{
		cout << "... User status set to active ...\n\n";
	}
	else
	{
		cout << "... Give proper input ...\n\n";
	}
	AOM_save_without_extensions(tUser);
	AOM_unlock(tUser);
}

void deleteUser(char* ActUser)
{
	makeInactive(ActUser, 1);
	SA_find_user2("izn", &tUserOW);
	SA_find_group("dba", &tGroup);
	SA_delete_user(tUser, tUserOW, tGroup, true);
	cout << "... User deleted successfully ...\n\n";
}