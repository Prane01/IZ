#include<iostream>
#include<tcinit/tcinit.h>
#include<tc/emh.h>
#include<sa/groupmember.h>
#include<sa/user.h>
#include<tccore/aom_prop.h>
using namespace std;

int loginToTeamcenter();
int checkifail();
int checkNullTag(tag_t tag);

int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	loginToTeamcenter();
	return ITK_ok;
}

int checkNullTag(tag_t tag)
{
	if (tag == NULLTAG)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "The Error is: " << cError;
		return ifail;
	}
	else
	{
		return ITK_ok;
	}
}

int loginToTeamcenter()
{
	if (ifail = ITK_auto_login() != ITK_ok)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "Error: " << cError;
	}
	else
	{
		cout << "Login successful..";
		/*tag_t grpMem = NULLTAG;
		char* name = NULL;
		//SA_find_user2("13165", &grpMem);
		SA_find_person2("Praneeth", &grpMem);
		AOM_ask_value_string(grpMem, "user_name", &name);
		cout << "os_username: " << name << endl;*/
	}
	return ITK_ok;
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