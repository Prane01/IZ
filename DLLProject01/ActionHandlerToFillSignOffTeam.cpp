#include <iostream>
#include <tc/emh.h>
#include <tccore/method.h>
#include <tccore/custom.h>
#include <epm/epm.h>
#include <epm/signoff.h>
#include <tccore/item.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <sa/sa.h>
using namespace std;
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +20)

int status = 0;
int ifail = ITK_ok;
char* cError = NULL;
extern "C" {
	// Declaration
	int checkifail();
	int assign_reviewers(tag_t signOff_task);
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_add_reviewers(EPM_action_message_t msg);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);
	// Definition
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "****************************************\n\n";
		cout << "* * * * Teamcenter Login Success * * * *\n\n";
		cout << "****************************************\n\n";
		status = EPM_register_action_handler("custom-add-reviewers", "custom-add-reviewers", (EPM_action_handler_t)PLM_add_reviewers);
		return status;
	}

	extern DLLAPI int PLM_add_reviewers(EPM_action_message_t msg)
	{
		tag_t task = msg.task;
		char* task_name = NULL;
		ifail = AOM_ask_value_string(task, "object_type", &task_name);
		checkifail();
		if (tc_strcmp(task_name,"EPMSelectSignoffTask") == 0)
		{
			assign_reviewers(task);
		}
		return ifail;
	}

	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "***** Teamcenter Logout Success *****\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		return status;
	}

	int assign_reviewers(tag_t signOff_task) {
		int n_signOffs = 0;
		int mem = 0;
		tag_t users = NULLTAG;
		tag_t* members = NULL;
		tag_t* signOff_tag = NULLTAG;
		SA_find_user2("izn", &users);
		SA_find_groupmember_by_user(users, &mem, &members);
		ifail = EPM_create_adhoc_signoff_with_requirement(signOff_task, members[0], NULL, &n_signOffs, &signOff_tag);
		checkifail();
		ifail = EPM_set_adhoc_signoff_selection_done(signOff_task, true);
		checkifail();
		return ifail;
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
}

