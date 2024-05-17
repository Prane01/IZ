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
#include <epm/epm.h>
#include<sa/sa.h>
#include <epm/epm_task_template_itk.h>
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;
tag_t group_member(const char* role, const char* group, const char* user_id);
int ITK_user_main(int argc, char* argv[])
{


	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			tag_t process_template = NULLTAG;
			int no_od_sub_tasks = 0;
			int resource_list_count = 2;
			int resource_count = 1;
			tag_t* sub_tasks = NULL;
			char* task_type = NULL;
			tag_t new_PAList = NULLTAG;
			ITK_set_bypass(true);
			ifail = EPM_find_process_template("ProcessAssignmentList", &process_template);
			checkifail();
			ifail = EPM_ask_subtask_templates(process_template, &no_od_sub_tasks, &sub_tasks);
			checkifail();
			tag_t GroupMember1 = group_member("DBA","dba","izn");
			tag_t GroupMember2 = group_member("DBA", "dba", "infodba");
			EPM_resource_list_t* resourcesToAdd = NULL;
			resourcesToAdd = (EPM_resource_list_t *)MEM_alloc(resource_list_count * sizeof(EPM_resource_list_t));
			resourcesToAdd[0].count = resource_count;
			resourcesToAdd[0].profiles = (tag_t*)MEM_alloc(resource_count * sizeof(tag_t));
			resourcesToAdd[0].Resources = (tag_t*)MEM_alloc(resource_count * sizeof(tag_t));
			resourcesToAdd[0].actions = (int*)MEM_alloc(resource_count * sizeof(int));
			resourcesToAdd[0].profiles[0] = NULLTAG;
			resourcesToAdd[0].Resources[0] = GroupMember1;
			resourcesToAdd[0].actions[0] = EPM_Perform;
			resourcesToAdd[1].count = resource_count;
			resourcesToAdd[1].profiles = (tag_t*)MEM_alloc(resource_count * sizeof(tag_t));
			resourcesToAdd[1].Resources = (tag_t*)MEM_alloc(resource_count * sizeof(tag_t));
			resourcesToAdd[1].actions = (int*)MEM_alloc(resource_count * sizeof(int));
			resourcesToAdd[1].profiles[0] = NULLTAG;
			resourcesToAdd[1].Resources[0] = GroupMember2;
			resourcesToAdd[1].actions[0] = EPM_Perform;

			ifail = EPM_assignment_list_create("ProcessAssignmentList 01", 0, NULL, process_template, false, resource_list_count, sub_tasks, resourcesToAdd, &new_PAList);
			checkifail();
			AOM_save_without_extensions(new_PAList);
			cout << "DONE..\n";
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

tag_t group_member(const char* role, const char* group, const char* user_id) {
	int n_found = 0;
	tag_t* list = NULL;
	SA_find_groupmember_by_rolename(role, group, user_id, &n_found, &list);
	return list[0];
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
		cout << "The Error is: " << cError;
		exit(0);
	}
	else
	{
		return 0;
	}
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