#include<iostream>
#include<tcinit/tcinit.h>
#include<tc/emh.h>
#include<tccore/item.h>
#include<property/prop.h>
#include<sa/groupmember.h>
#include<tccore/aom.h>
#include<tccore/aom_prop.h>
#include<epm/epm.h>
#include<tccore/releasestatus.h>
#include<epm/epm_task_template_itk.h>
using namespace std;

int loginToTeamcenter();
int checkifail();
int checkNullTag(tag_t tag);

int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	loginToTeamcenter();
	tag_t process_template = NULLTAG;
	tag_t rev1 = NULLTAG;
	tag_t rev2 = NULLTAG;
	tag_t new_process = NULLTAG;
	char* value1 = NULL;
	tag_t* value11 = NULLTAG;
	int num = 1;
	int att_type = 1;
	ifail = ITEM_find_rev("001040", "A" ,&rev1);
	checkifail();
	int n_status = 0;
	tag_t* statuses = NULL;

	ifail = PROP_ask_composite_property(rev1, "release_status_list", &value1);
	cout << value1 << endl;

	/*ifail = AOM_ask_value_tags(rev1, "release_status_list", &num, &value11);
	checkifail();*/

	ifail = WSOM_ask_release_status_list(rev1, &n_status, &statuses);
	checkifail();
	cout << n_status << endl;
	ifail = ITEM_find_rev("001041", "A", &rev2);
	checkifail();

	RELSTAT_add_release_status(statuses[n_status-1], 1, &rev2, true);
	//AOM_save_without_extensions(rev2);
	cout << "SUCCESSSS"<< endl;
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