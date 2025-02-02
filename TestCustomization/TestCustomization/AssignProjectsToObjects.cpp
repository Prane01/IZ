#include<stdio.h>
#include<conio.h>
#include<iostream>
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
#include <tccore/project.h>
#include <sa/user.h>
#define qName "IR_DR_PR"
using namespace std;
int checkNullTag(tag_t tag);
int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	tag_t tQuery = NULLTAG;
	int entries = 0;
	char** actEntries = NULL;
	char** actValues = NULL;
	int foundObj = 0;
	tag_t* results = NULL;

	tag_t tProject = NULLTAG;
	char* object_string = NULL;
	char* object_type = NULL;
	ifail = ITK_init_module("izn", "izn", "dba"); // Login to server

	if (ifail == ITK_ok)
	{
		cout << "\nLogin successful...\n\n";
		QRY_find2(qName, &tQuery); // finding query 
		checkNullTag(tQuery);
		QRY_find_user_entries(tQuery, &entries, &actEntries, &actValues);
		QRY_execute(tQuery, entries, actEntries, actValues, &foundObj, &results);

		PROJ_find("T003", &tProject); // finding specific project with project_id and storing it in tag_t
		cout << "Found project\n";

		ITK_set_bypass(true); // setting privileges to allow making changes

		for (int i = 0; i < foundObj; i++)
		{
			AOM_ask_value_string(results[i], "object_type", &object_type);
			if (tc_strcmp("ItemRevision", object_type) == 0 || tc_strcmp("DocumentRevision", object_type) == 0 || tc_strcmp("Part_0_Revision_alt", object_type) == 0)
			{
				AOM_ask_value_string(results[i], "object_string", &object_string);
				cout << "Object String: " << object_string << endl;
				cout << "Object Type: " << object_type << endl;
				AOM_refresh(results[i], true);
				PROJ_assign_objects(1, &tProject, 1, &results[i]);
				AOM_save_without_extensions(results[i]);
				AOM_refresh(results[i], false);
			}
		}
		ITK_set_bypass(false);
	}
	else
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "\n\n The error is : " << cError;
	}
	return ifail;
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