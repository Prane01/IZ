#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<epm/epm.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<ce/ce.h>
#include<tccore/item.h>
#define DLLAPI _declspec(dllexport)
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
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int update_description(METHOD_message_t* msg, va_list args);
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
		
		status = METHOD_find_method("Item", "TC_assign_primary_obj_to_project", &tmethod_id);
		status = METHOD_add_action(tmethod_id, METHOD_post_action_type, (METHOD_function_t)update_description, NULL);
		return status;
	}

	extern DLLAPI int update_description(METHOD_message_t* msg, va_list args) {
		tag_t item = NULLTAG;
		item = va_arg(args, tag_t);
		tag_t curr_user = NULLTAG;
		char* uname = NULL;
		int revC = 0;
		tag_t* revList = NULLTAG;

		ifail = CE_current_user_session_tag(&curr_user);
		checkifail(); checkNullTag(curr_user);
		ifail = AOM_ask_value_string(curr_user, "user_id", &uname);
		checkifail();
		cout << uname << endl;
		ifail = AOM_set_value_string(item, "object_desc", uname);
		checkifail();
		ITEM_list_all_revs(item, &revC, &revList);

		if (revC > 0)
		{
			for (int i = 0; i < revC; i++)
			{
				ifail = AOM_set_value_string(revList[i], "object_desc", uname);
				checkifail();
			}
			cout << "* * * Description updated * * *\n\n";
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