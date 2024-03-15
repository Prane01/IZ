#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tccore/item.h>
#include<tccore/grm.h>
#include<fclasses/tc_string.h>
#include<tccore/aom_prop.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +20)
using namespace std;

extern "C" {

	METHOD_id_t tmethod_id;
	int status = 0;

	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_add_pre_action(METHOD_message_t* msg, va_list args);
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
		cout << "* * *  Teamcenter Login Success  * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";

		status = METHOD_find_method("ItemRevision", "IMAN_delete", &tmethod_id);
		status = METHOD_add_action(tmethod_id, METHOD_pre_action_type, (METHOD_function_t)PLM_add_pre_action, NULL);

		return status;
	}

	extern DLLAPI int PLM_add_pre_action(METHOD_message_t* msg, va_list args) {
		tag_t source_rev = NULLTAG;
		source_rev = va_arg(args, tag_t);
		tag_t* secObj = NULL;
		int sCount = 0;
		char* objName = NULL;
		tag_t cls_id = NULLTAG;
		char* name = NULLTAG;
		char* objType = NULLTAG;
		
		cout << "Pre Action began...\n\n";

		status = GRM_list_secondary_objects_only(source_rev, NULLTAG, &sCount, &secObj);
		cout << sCount << endl;
		if (sCount > 1)
		{
			int i = 0;
			bool flag = true;
			while(flag && i != sCount){
				status = POM_class_of_instance(secObj[i], &cls_id);
				status = POM_name_of_class(cls_id, &name);
				if (tc_strcmp(name, "Dataset") == 0)
				{
					AOM_ask_value_string(source_rev, "object_string", &objName);
					cout << objName << endl;
					EMH_store_error_s1(EMH_severity_error, PLM_error, objName);
					flag = false;
				}
				i++;
			}
			if (!flag)
			{
				return PLM_error;
			}
			else
			{
				return status;
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