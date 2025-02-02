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
#include <epm/epm_task_template_itk.h>
#include <tc/preferences.h>
using namespace std;
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +20)

int status = 0;
int ifail = ITK_ok;
char* cError = NULL;
tag_t* signOff_tag = NULLTAG;
extern "C" {
	// Declaration
	int checkifail();
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_add_reviewers(EPM_action_message_t msg);
	int assign_reviewers(tag_t signOff_task);
	int perform_operation(tag_t perform_task);
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
		status = EPM_register_action_handler("HON-PMT-CTO-REVIWER", "HON_PMT_CTO_REVIWER", (EPM_action_handler_t)PLM_add_reviewers);
		return status;
	}

	extern DLLAPI int PLM_add_reviewers(EPM_action_message_t msg)
	{
		tag_t Current_task = msg.task, tUser = NULLTAG, *tGrpMems = NULLTAG, *tSignOffs = NULLTAG;
		int iPref = 0, iGrpMems = 0, iSignOffs = 0;
		char* PrefValues = NULL; char *task_name = NULL;

		cout << "\n Entered into main function which assigns";
		ITKCALL(PREF_ask_char_value("HON_PMT_CTO_REVIWER", &iPref, &PrefValues));
		ITKCALL(AOM_ask_name(Current_task, &task_name));
		cout << "\n The task name is " << task_name;
		cout << "\nThe prefvalue is " << PrefValues[0];

		ITKCALL(SA_find_user2(PrefValues[0], &tUser));

		ITKCALL(SA_find_groupmember_by_user(tUser, &iGrpMems, &tGrpMems));
		ITKCALL(EPM_create_adhoc_signoff(Current_task, tGrpMems[0], &iSignOffs, &tSignOffs));
		ITKCALL(EPM_set_adhoc_signoff_selection_done(Current_task, TRUE));
		cout << "\n\nSignoffs success";

		return EPM_go;
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
		tag_t Group = NULLTAG;
		tag_t Role = NULLTAG;
		tag_t resourcepool = NULLTAG;
		tag_t* members = NULL;
		tag_t review_task = NULLTAG;
		int no_of_tasks = 0;
		tag_t* sub_tasks = NULLTAG;
		char* task_type = NULL;
		SA_find_group("dba", &Group);
		SA_find_role2("DBA", &Role);
		ifail = EPM_get_resource_pool(Group, Role, false, &resourcepool);
		checkifail();
		ifail = EPM_create_adhoc_signoff_with_requirement(signOff_task, resourcepool, NULL, &n_signOffs, &signOff_tag);
		checkifail();
		ifail = EPM_set_adhoc_signoff_selection_done(signOff_task, true);
		checkifail();
		return ifail;
	}

	int perform_operation(tag_t perform_task) {
		/*ifail = EPM_claim_task(perform_task, *signOff_tag);
		checkifail();*/
		ifail = EPM_set_task_decision3(perform_task, *signOff_tag, EPM_approve_decision, "qwertyuhgxdfghjkl");
		checkifail();
		EPM_trigger_action(perform_task, EPM_complete_action, "HIHIHIHI");
		//checkifail();
		cout << "DONE\n";
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

