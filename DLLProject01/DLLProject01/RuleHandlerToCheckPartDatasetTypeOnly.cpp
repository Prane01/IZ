#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<epm/epm.h>
#include<tccore/grm.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +9)
using namespace std;

extern "C" {

	int status = 0;
	int ifail = 0;
	char* cError = NULL;

	// Declaration of function
	int checkifail();
	int checkNullTag(tag_t tag);
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int check_specific_dataset(EPM_rule_message_t msg);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	//Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * \n\n";
		cout << " * *  * Teamcenter Login Success * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * \n\n";
		EPM_register_rule_handler("custom-check-dataset", "custom-check-dataset", (EPM_rule_handler_t)check_specific_dataset);
		return status;
	}

	extern DLLAPI int check_specific_dataset(EPM_rule_message_t msg) {
		tag_t root_task = NULLTAG;
		int attCount = 0;
		tag_t* attachments = NULL;
		int secCount = 0;
		tag_t* secDatasets = NULL;
		int noOfArguments = 0;
		char* argument_name = NULL;
		char* argument_value = NULL;
		char* value = NULL;
		int decision = 0;
		char* secType = NULL;
		bool flag = false;
		tag_t class_id = NULLTAG;
		char* className = NULL;

		ifail = EPM_ask_root_task(msg.task, &root_task);
		checkifail();
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		checkifail();
		noOfArguments = TC_number_of_arguments(msg.arguments);
		ITK_ask_argument_named_value(TC_next_argument(msg.arguments), &argument_name, &argument_value);
		if (attCount > 0)
		{
			if (tc_strcmp(argument_name, "type") == 0)
			{
				for (int i = 0; i < attCount; i++) {
					ifail = GRM_list_secondary_objects_only(attachments[i], NULLTAG, &secCount, &secDatasets);
					checkifail();
					for (int j = 0; j < secCount; j++)
					{
						AOM_ask_value_string(secDatasets[j], "object_type", &secType);
						if (tc_strcmp(argument_value, secType) == 0)
						{
							decision = EPM_go;
						}
						else
						{
							EMH_store_error_s2(EMH_severity_error, PLM_error, secType, argument_value);
							decision = EPM_nogo;
							return PLM_error;
						}
					}
				}
			}
			else
			{
				cout << "No argument of " << argument_name << " exist for this handler\n\n";
			}
		}
		return decision;
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