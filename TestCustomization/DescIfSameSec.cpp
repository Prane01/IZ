#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/workspaceobject.h>
#include<tccore/aom.h>
#include<tccore/aom_prop.h>
using namespace std;

int checkifail();
int checkNullTag(tag_t tag);

int ifail = 0;
char* cError = NULL;

int ITK_user_main(int arg, char* args[]) {

	
	tag_t trev = NULLTAG;
	char* value = NULL;
	tag_t tRelationType = NULLTAG;
	int count = 0;
	char* value1 = NULL;
	char* test = NULL;
	char* act = NULL;
	tag_t * secondary_list = NULL;
	tag_t* stored = NULLTAG;
	bool flag = true;

	ifail = ITK_init_module("infodba", "infodba", "dba");
	if (ifail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		ifail = ITEM_find_rev("000316", "A", &trev);
		if (trev != NULLTAG)
		{
			cout << "\n\n Item Revision Found..\n";
			cout << "\n\n Fetching secondary objects..\n";
			ifail = GRM_list_secondary_objects_only(trev, NULLTAG, &count, &secondary_list);
			if (secondary_list != NULLTAG)
			{
				cout << "No.of secondaries are : " << count << '\n';
				char* SecondaryTypes = NULL;
				for (int i = 0; i < count; i++)
				{
					if (i != 0)
					{
						AOM_ask_value_string(secondary_list[i], "object_type", &test);
						cout << i << ". " << test << '\n';
						if (i == 1)
						{
							act = test;
						}
						else if (strcmp(act, test) != 0)
						{
							flag = false;
							cout << "Secondaries are not same, cannot update object description...\n\n";
							break;
						}
						else
						{
							flag = true;
						}
					}	
				}
				if (flag)
				{
					AOM_refresh(trev, true);
					AOM_set_value_string(trev, "object_desc", "Updated Now");
					AOM_save_without_extensions(trev);
					AOM_refresh(trev, false);
					cout << "object description updated...\n";
				}
			}
			else {
				cout << "\n\n The error is : Cannot list secondaries";
			}
		}
		else {
			cout << "\n\n The error is : Unable to fetch secondaries";
		}
	}
	else {
		EMH_ask_error_text(ifail, &cError);
		cout << "\n\n The error is : %s\n", cError;
	}
	return ifail;

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
		exit(0);
	}
	return ITK_ok;
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