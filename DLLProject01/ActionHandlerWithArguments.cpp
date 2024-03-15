#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<epm/epm.h>
#include<tccore/aom_prop.h>
#define DLLAPI _declspec(dllexport)
#define PLM_inform (EMH_USER_error_base +6)
using namespace std;


extern "C" {
	int ifail = 0;
	char* cError = NULL;
	int status = 0;

	// Declaration of function
	int checkifail();
	int checkNullTag(tag_t tag);
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int add_status_from_argument(EPM_action_message_t msg);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	//Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "* * * * Teamcenter Login Success * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * * * * * \n\n";
		EPM_register_action_handler("PLM-create-status", "PLM-create-status", (EPM_action_handler_t)add_status_from_argument);
		return status;
	}

	extern DLLAPI int add_status_from_argument(EPM_action_message_t msg) {
		tag_t root_task = NULLTAG;
		int attCount = 0;
		tag_t* attachments = NULLTAG;
		int noOfArguments = 0;
		char* argument_name = NULL;
		char* argument_value = NULL;
		char* argStatus = NULL;
		tag_t class_id = NULLTAG;
		char* className = NULL;
		tag_t release_status = NULLTAG;
		char* p1 = NULL;

		ifail = EPM_ask_root_task(msg.task, &root_task);
		checkifail();
		checkNullTag(root_task);
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		checkifail();
		if (attCount > 0)
		{
			noOfArguments = TC_number_of_arguments(msg.arguments);
			for (int i = 0; i < noOfArguments; i++)
			{
				ITK_ask_argument_named_value(TC_next_argument(msg.arguments), &argument_name, &argument_value);
				if (tc_strcmp(argument_name, "apply_status") == 0)
				{
					argStatus = (char*)MEM_alloc(10 * sizeof(char*));
					tc_strcpy(argStatus, "");
					tc_strcpy(argStatus, argument_value);
				}
				for (int j = 0; j < attCount; j++)
				{
					ifail = POM_class_of_instance(attachments[j], &class_id);
					checkifail();
					checkNullTag(class_id);
					ifail = POM_name_of_class(class_id, &className);
					checkifail();
					if (tc_strcmp(className, "ItemRevision") == 0)
					{
						ifail = RELSTAT_create_release_status(argStatus, &release_status);
						checkifail();
						checkNullTag(release_status);
						ifail = RELSTAT_add_release_status(release_status, 1, &attachments[j], true);
						checkifail();
					}
					if (ifail != ITK_ok)
					{
						AOM_ask_value_string(attachments[j], "object_string", &p1);
						EMH_store_error_s1(EMH_severity_information, PLM_inform, p1);
					}
				}
				if (tc_strcmp(p1, "") != 0)
				{
					return PLM_inform;
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
}