#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<fclasses/tc_string.h>
#include<tccore/grm.h>
#include<vector>
#include<algorithm>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +15)
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
	extern DLLAPI int PLM_add_post_action(METHOD_message_t* msg, va_list args);
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

		status = METHOD_find_method("A2Custom_Relation01", "GRM_create", &tmethod_id);
		//status = METHOD_add_pre_condition(tmethod_id, (METHOD_function_t)PLM_add_precondition, NULL);
		status = METHOD_add_action(tmethod_id, METHOD_post_action_type, (METHOD_function_t)PLM_add_post_action, NULL);
		return status;
	}

	extern DLLAPI int PLM_add_post_action(METHOD_message_t* msg, va_list args) {
		tag_t primary_object = NULLTAG;
		primary_object = va_arg(args, tag_t);
		tag_t secondary_object = NULLTAG;
		secondary_object = va_arg(args, tag_t);
		tag_t relation_type = NULLTAG;
		relation_type = va_arg(args, tag_t);
		tag_t user_data = NULLTAG;
		user_data = va_arg(args, tag_t);
		tag_t* new_relation = NULL;
		new_relation = va_arg(args, tag_t*);
		int count = 0;
		int c = 0;
		GRM_relation_t* sec_list = NULL;
		char* verify01 = NULL;
		char* verify02 = NULL;
		tag_t class_id = NULL;
		vector<string> all_values;
		ifail = GRM_list_secondary_objects(primary_object, relation_type, &count, &sec_list);
		checkifail();
		AOM_ask_value_string(*new_relation, "a2PropOnRelation", &verify02);
		//cout << verify02 << endl;
		for (int i = 0; i < count; i++)
		{
			AOM_ask_value_string(sec_list[i].the_relation, "a2PropOnRelation", &verify01);
			//cout << verify01 << endl;
			auto result = find(all_values.begin(), all_values.end(), verify01);
			if (result == end(all_values) && tc_strcmp(verify01, verify02) != 0)
			{
				all_values.push_back(verify01);
			}
			else
			{
				EMH_store_error_s1(EMH_severity_error, PLM_error, verify02);
				return PLM_error;
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