#include<iostream>
#include<stdio.h>
#include<conio.h>
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
#include<epm/epm.h>
#include<epm/epm_task_template_itk.h>
#include<sa/sa.h>
#include<epm/signoff.h>
#define EPM_target_attachment               1
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
void executeTask(tag_t task, const char* item_id, const char* rev_id, tag_t* userss);
int ifail = 0;
char* cError = NULL;
tag_t* signOffs = NULL;

int ITK_user_main(int argc, char* argv[])
{


	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char *item_id = ITK_ask_cli_argument("-id="); // API takes user input as group

	char *rev_id = ITK_ask_cli_argument("-rid="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			tag_t tGroup = NULLTAG;
			tag_t tUser = NULLTAG;
			tag_t* userss = NULL;
			tag_t tRevision = NULLTAG;
			int n_tasks = 0;
			tag_t* tasks = NULLTAG;
			tag_t process_template = NULLTAG;
			tag_t new_process = NULLTAG;
			int att_types[] = { 1};
			tag_t task = NULLTAG;
			int mem = 0;
			tag_t* multiple = new tag_t[1];
			SA_find_group("dba", &tGroup);
			SA_find_user2("izn", &tUser);
			SA_find_groupmembers(tUser, tGroup, &mem, &userss);
			/*ifail = ITEM_find_rev(item_id, rev_id, &multiple[0]);
			checkifail();*/
			ifail = ITEM_find_rev("000692", "A", &multiple[0]);
			checkifail();
			ifail = EPM_find_process_template("Run throug ITK Test", &process_template);
			checkifail();
			ifail = EPM_create_process("Run throug ITK Test", NULL, process_template, 1, multiple, att_types, &new_process);
			checkifail();
			do {
				ifail = EPM_ask_tasks(new_process, EPM_started, &n_tasks, &tasks);
				checkifail();
				if (n_tasks != 0)
				{
					task = tasks[n_tasks-1];
					executeTask(task, item_id, rev_id, userss);
				}
			} while (n_tasks != 0);
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

void executeTask(tag_t task, const char* item_id, const char* rev_id, tag_t* userss) {
	tag_t root_task = NULLTAG;
	tag_t signOff = NULLTAG;
	tag_t* attachments = NULLTAG;
	int c = 0;
	const char* desc = "@08 SET FROM ITK WRKFLW";
	char* name = NULL;
	EPM_action_t action = EPM_complete_action;
	EPM_signoff_decision_t decision = EPM_approve_decision;
	ifail = EPM_ask_root_task(task, &root_task);
	checkifail();
	ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &c, &attachments);
	checkifail();
	cout << "targets : " << c << endl;
	AOM_ask_value_string(task, "object_type", &name);
	cout << name << endl;
	if (tc_strcmp(name, "EPMDoTask") == 0)
	{
		ITK_set_bypass(true);
		for (int i = 0; i < c; i++)
		{
			AOM_refresh(attachments[i], true);
			AOM_set_value_string(attachments[i], "object_desc", desc);
			AOM_save_without_extensions(attachments[i]);
			AOM_refresh(attachments[i], false);
		}
		ITK_set_bypass(false);
		ifail = EPM_set_task_result(task, EPM_RESULT_Completed);
		checkifail();
		ifail = EPM_trigger_action(task, action, "HIHIHI");
		checkifail();
		cout << "Done\n";
	}
	if (tc_strcmp(name, "EPMAddStatusTask") == 0)
	{
		ifail = EPM_set_task_result(task, EPM_RESULT_Completed);
		checkifail();
		ifail = EPM_trigger_action(task, action, "HIHIHI");
		checkifail();
	}
	if (tc_strcmp(name, "EPMConditionTask") == 0)
	{
		char* result = NULL;
		ifail = EPM_get_task_result(task, &result);
		checkifail();
		ifail = EPM_trigger_action(task, EPM_start_action, "HIHIHI");
		checkifail();
	}
	if (tc_strcmp(name, "EPMSelectSignoffTask") == 0)
	{
		int n_signoffs = 0;
		/*ifail = EPM_add_reviewers_on_task_with_signoff(task, false, 1, &userss[0], "Optional");
		checkifail();*/
		ifail = EPM_create_adhoc_signoff_with_requirement(task, userss[0], NULL, &n_signoffs, &signOffs);
		checkifail();
		ifail = EPM_set_adhoc_signoff_selection_done(task, true);
		checkifail();
		ifail = EPM_trigger_action(task, EPM_signoff_attachment, "HIHIHI");
		checkifail();
		//signOff = task;
		cout << "Done\n";
	}
	if (tc_strcmp(name, "EPMPerformSignoffTask") == 0)
	{
		//EPM_set_task_decision3(task, signOff, decision, "Approved");
		ifail = EPM_set_task_decision3(task, *signOffs, decision, "Approved");
		checkifail();
		ifail = EPM_trigger_action(task, EPM_perform_action, "HIHIHI");
		checkifail();
		cout << "Done\n";
	}
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