#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<epm/epm.h>
#include<tccore/aom_prop.h>
#define DLLAPI _declspec(dllexport)
using namespace std;

int status = 0;
char* cError = NULL;

int checkNullTag(tag_t tag);
extern "C" {
	
	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int add_custom_status(EPM_action_message_t msg);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	//Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "* * * Teamcenter Login Success * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		EPM_register_action_handler("custom-create-status", "Creating custom status", (EPM_action_handler_t)add_custom_status);
		return status;
	}

	extern DLLAPI int add_custom_status(EPM_action_message_t msg) {
		tag_t root_task = NULLTAG;
		int attCount = 0;
		tag_t* attachments = NULLTAG;
		tag_t class_id = NULLTAG;
		char* class_name = NULL;
		tag_t rel_status = NULLTAG;

		status = EPM_ask_root_task(msg.task, &root_task);
		checkNullTag(root_task);
		status = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		if (attCount > 0)
		{
			for (int i = 0; i < attCount; i++)
			{
				status = POM_class_of_instance(attachments[i], &class_id);
				status = POM_name_of_class(class_id, &class_name);
				if (tc_strcmp(class_name,"ItemRevision") == 0)
				{
					status = RELSTAT_create_release_status("CustomStatusDLL", &rel_status);
					checkNullTag(rel_status);
					status = RELSTAT_add_release_status(rel_status, 1, &attachments[i], true);
					cout << "... Released status has been set ...\n\n";
				}
			}
		}
		return status;
	}

	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "***** Teamcenter Logout Success *****\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		return status;
	}
}

int checkNullTag(tag_t tag)
{
	if (tag == NULLTAG)
	{
		EMH_ask_error_text(status, &cError);
		cout << "The Error is: " << cError;
		exit(0);
	}
	else
	{
		return 0;
	}
}