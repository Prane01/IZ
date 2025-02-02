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
using namespace std;
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +20)

int status = 0;
int ifail = ITK_ok;
char* cError = NULL;
tag_t signOff_tag = NULLTAG;
extern "C" {
	// Declaration
	int checkifail();
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_add_reviewers(EPM_action_message_t msg);
	int assign_reviewers(tag_t useGrp, tag_t signOff_task);
	//int perform_operation(tag_t perform_task);
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
		tag_t root_task = NULLTAG;
		int c = 0;
		int no_st_tasks = 0;
		tag_t* attachments = NULL;
		char* task_name = NULL;
		tag_t rev = NULLTAG;
		tag_t* tasks = NULL;
		tag_t task = NULLTAG;
		EPM_ask_root_task(msg.task, &root_task);
		EPM_ask_attachments(root_task, EPM_target_attachment, &c, &attachments);
		rev = attachments[0];
		AOM_ask_value_string(msg.task, "object_type", &task_name);
		if (tc_strcmp(task_name, "EPMSelectSignoffTask") == 0 && signOff_tag == NULLTAG)
		{
			assign_reviewers(attachments[0], msg.task);
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

	int assign_reviewers(tag_t useGrp,tag_t signOff_task) {
		tag_t Group = NULLTAG;
		tag_t Role = NULLTAG;
		tag_t Job = NULLTAG;
		tag_t resourcepool = NULLTAG;
		tag_t* us_profiles = NULL;
		int unassigned_count = 0;
		AOM_ask_group(useGrp, &Group);
		SA_ask_group_default_role(Group, &Role);
		ifail = EPM_get_resource_pool(Group, Role, false, &resourcepool);
		checkifail();
		/*ifail = EPM_create_adhoc_signoff_with_requirement(signOff_task, resourcepool, NULL, &n_signOffs, &signOff_tag);
		checkifail();*/
		ifail = EPM_ask_job(signOff_task, &Job);
		checkifail();
		ifail = EPM_ask_unassigned_profiles(Job, signOff_task, &unassigned_count, &us_profiles);
		checkifail();
		ifail = EPM_create_profile_signoff(signOff_task, resourcepool, us_profiles[0], &signOff_tag);
		checkifail();
		ifail = EPM_set_adhoc_signoff_selection_done(signOff_task, true);
		checkifail();
		cout << "DONE>>\n";
		return ifail;
	}

	//int perform_operation(tag_t perform_task) {
	//	ifail = EPM_claim_task(perform_task, signOff_tag);
	//	checkifail();
	//	ifail = EPM_set_task_decision3(perform_task, signOff_tag, EPM_approve_decision, "qwertyuhgxdfghjkl");
	//	checkifail();
	//	EPM_trigger_action(perform_task, EPM_complete_action, "HIHIHIHI");
	//	//checkifail();
	//	cout << "DONE\n";
	//	return ifail;
	//}

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

