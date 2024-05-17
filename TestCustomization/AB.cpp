#include<iostream>
#include<tcinit/tcinit.h>
#include<tc/emh.h>
#include<sa/groupmember.h>
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
	tag_t primaryObj = NULLTAG;
	ITK__convert_uid_to_tag(primaryObject.c_str(), &primaryObj);
	int count = 0;
	tag_t relation_type = NULLTAG;
	tag_t* secondary_objects = NULL;
	tag_t out_relation_type = NULLTAG;
	tag_t relation = NULLTAG;
	GRM_find_relation_type("IMAN_reference", &relation_type);
	GRM_find_relation_type("IMAN_specification", &out_relation_type);
	GRM_list_secondary_objects_only(primaryObj, relation_type, &count, &secondary_objects);
	for (int i = 0; i < count; i++) {
		GRM_create_relation(primaryObj, secondary_objects[i], out_relation_type, NULLTAG, &relation);
		GRM_save_relation(relation);
	}
	GRM_delete_relation(relation_type);
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
		tag_t grpMem = NULLTAG;
		char* name = NULL;
		SA_ask_current_groupmember(&grpMem);
		AOM_ask_value_string(grpMem, "user_name", &name);
		cout << "Current Login User: " << name << endl;
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