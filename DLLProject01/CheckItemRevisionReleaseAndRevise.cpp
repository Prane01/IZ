#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tccore/aom_prop.h>
#include<fclasses/tc_string.h>

#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +2)
using namespace std;

extern "C" {

	METHOD_id_t tmethod_id;
	int status = 0;


	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_add_precondition(METHOD_message_t* msg, va_list args);
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
		cout << "* * * * Teamcenter Login Success * * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";

		status = METHOD_find_method("ItemRevision", "ITEM_copy_rev", &tmethod_id);
		status = METHOD_add_pre_condition(tmethod_id, (METHOD_function_t)PLM_add_precondition, NULL);

		return status;
	}

	//Pre condition
	extern DLLAPI int PLM_add_precondition(METHOD_message_t* msg, va_list args) {
		bool flag = true;
		tag_t source_rev = NULLTAG;
		char* pname = NULL;
		source_rev = va_arg(args, tag_t);
		int noOfRel = 0;
		char** releases = NULL;
		AOM_ask_displayable_values(source_rev, "release_status_list", &noOfRel, &releases);
		AOM_ask_value_string(source_rev, "object_string", &pname);
		for (int i = 0; i < noOfRel; i++)
		{
			if (tc_strcmp(releases[i], "TCM Released") == 0)
			{
				flag = false;
				cout << "\nTCM Released\n\n";
			}
		}
		if (flag)
		{
			EMH_store_error(EMH_severity_error, PLM_error);
			return PLM_error;
		}
		return 0;
	}

	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "* * * * Teamcenter Logout Success * * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		return status;
	}
}