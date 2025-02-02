#include<iostream>
#include<tcinit/tcinit.h>
#include<tc/emh.h>
#include<property/prop.h>
#include<tccore/item.h>
#include<tccore/aom.h>
#include<tccore/aom_prop.h>
#include<fclasses/tc_date.h>
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
	if (ifail = TC_auto_login() != ITK_ok)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "Error: " << cError;
	}
	else
	{
		cout << "Login successful..";
		tag_t tItem = NULLTAG;
		date_t date = NULLDATE;
		string sDate = "15-Jan-2024 20:45:00";
		char* c_date = new char[sDate.length() + 1];
		strcpy(c_date, sDate.c_str());
		date_t setDate = NULLDATE;
		bool isValid = NULL;
		ifail = ITEM_find_item("001124", &tItem);
		checkifail(); checkNullTag(tItem);
		ifail = POM_set_env_info(POM_bypass_attr_update, FALSE, 0, 0, NULLTAG, NULL);
		checkifail();
		ifail = DATE_string_to_date_t(c_date, &isValid, &setDate);
		checkifail();
		AOM_lock(tItem);
		ifail = POM_set_creation_date(tItem, setDate);
		checkifail();
		ifail = POM_save_instances(1, &tItem, true);
		checkifail();
		AOM_unlock(tItem);
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