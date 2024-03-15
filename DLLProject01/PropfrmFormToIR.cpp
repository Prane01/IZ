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
		EPM_register_action_handler("PLM-create-form", "PLM-create-form", (EPM_action_handler_t)add_status_from_argument);
		return status;
	}

	extern DLLAPI int add_status_from_argument(EPM_action_message_t msg) {
		tag_t root_task = NULLTAG;
		int attCount = 0;
		tag_t* attachments = NULLTAG;
		int noOfArguments = 0;
		char* argument_name = NULL;
		char* argument_value = NULL;
		char* name = NULL;
		char* description = NULL;
		char* type = NULL;
		char* relation = NULL;
		tag_t class_id = NULLTAG;
		char* className = NULL;
		char* className1 = NULL;
		tag_t release_status = NULLTAG;
		char* p1 = NULL;
		tag_t datasetType = NULLTAG;
		tag_t newDataset = NULLTAG;
		tag_t relationType = NULLTAG;
		tag_t trelation = NULLTAG;
		int nSup = 0;
		tag_t* supList = NULLTAG;
		tag_t forItemRevision = NULLTAG;
		bool flag = true;

		ifail = EPM_ask_root_task(msg.task, &root_task);
		checkifail();
		checkNullTag(root_task);
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		checkifail();
		if (attCount > 0)
		{
			noOfArguments = TC_number_of_arguments(msg.arguments);
			cout << noOfArguments << endl;
			for (int i = 0; i < noOfArguments; i++)
			{
				ITK_ask_argument_named_value(TC_next_argument(msg.arguments), &argument_name, &argument_value);
				if (tc_strcmp(argument_name, "dataset_name") == 0)
				{
					name = (char*)MEM_alloc(100);
					tc_strcpy(name, "");
					tc_strcpy(name, argument_value);
				}
				if (tc_strcmp(argument_name, "dataset_desc") == 0)
				{
					description = (char*)MEM_alloc(100);
					tc_strcpy(description, "");
					tc_strcpy(description, argument_value);
				}
				if (tc_strcmp(argument_name, "dataset_type") == 0)
				{
					type = (char*)MEM_alloc(100);
					tc_strcpy(type, "");
					tc_strcpy(type, argument_value);
				}
				if (tc_strcmp(argument_name, "dataset_relation") == 0)
				{
					relation = (char*)MEM_alloc(100);
					tc_strcpy(relation, "");
					tc_strcpy(relation, argument_value);
				}
			}
			cout << name << endl;
			cout << description << endl;
			cout << type << endl;
			cout << relation << endl;
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