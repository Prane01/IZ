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
#include<pom/pom/pom.h>
using namespace std;

void display();
int checkNullTag(tag_t tag);
void deleteUser(tag_t tUser);
int ifail = 0;
char* cError = NULL;
tag_t tUserOW = NULLTAG;
tag_t tGroupOW = NULLTAG;

int ITK_user_main(int argc, char* argv[])
{
	int no_grps = 0;
	tag_t* tGroup = NULLTAG;
	char** grp_names = NULL;
	int no_roles = 0;
	tag_t* roles = NULLTAG;
	int no_users = 0;
	tag_t* members = NULLTAG;
	int j = 0;
	int count = 0;
	char* value = NULL;
	char* value1 = NULL;
	int status = 1;

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
			SA_list_groups(&no_grps, &tGroup, &grp_names);
			for (int i = 0; i < no_grps; i++)
			{
				POM_list_group_members(tGroup[i], &no_users, &members);
				AOM_ask_value_string(tGroup[i], "object_string", &value);
				cout << "Grp : " << value << endl;
				if (tc_strcmp(value, "Q") == 0) {
					for (int j = 0; j < no_users; j++)
					{
						POM_delete_member(members[j]);
					}
					cout << "users deleted\n\n";
					SA_ask_roles_from_group(tGroup[i], &no_roles, &roles);
					for (int z = 0; z < no_roles; z++)
					{
						AOM_ask_value_string(roles[z], "object_name", &value1);
						AOM_delete(roles[z]);
						cout << "Deleted : " << value1 << endl;
						AOM_save_without_extensions(tGroup[i]);
					}
					AOM_delete(tGroup[i]);
					cout << value << " deleted\n\n";
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

//void deleteUser(tag_t tUser)
//{
//	SA_find_user2("izn", &tUserOW);
//	SA_find_group("dba", &tGroupOW);
//	SA_delete_user(tUser, tUserOW, tGroupOW, true);
//	cout << "... User deleted successfully ...\n\n";
//}