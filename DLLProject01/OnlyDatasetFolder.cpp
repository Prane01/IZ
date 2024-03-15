#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<fclasses/tc_string.h>
#include<property/prop.h>
#include<ce/ce.h>
#include<pom/pom/pom.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +13)
using namespace std;

extern "C" {
	METHOD_id_t tmethod_id;
	int status = 0;
	int ifail = 0;
	char* cError = NULL;

	// Declaration of function
	int checkifail();
	int checkNullTag(tag_t tag);
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int pre_action_to_allow_only_datasets(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
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
		status = METHOD_find_prop_method("A2Datasets_Collection", "contents", "create", &tmethod_id);
		status = METHOD_add_action(tmethod_id, METHOD_pre_action_type, (METHOD_function_t)pre_action_to_allow_only_datasets, NULL);
		return status;
	}

	extern DLLAPI int pre_action_to_allow_only_datasets(METHOD_message_t* msg, va_list args) {
		
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