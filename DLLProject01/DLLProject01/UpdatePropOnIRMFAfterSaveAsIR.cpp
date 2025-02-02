#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tccore/grm.h>
#include<tccore/aom.h>
#include<tccore/aom_prop.h>

#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +3)
using namespace std;

extern "C" {

	METHOD_id_t tmethod_t;
	int status = 0;

	// Declaration
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_PostAction(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	// Definition
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

		status = METHOD_find_method("ItemRevision", "ITEM_create_from_rev", &tmethod_t);
		status = METHOD_add_action(tmethod_t, METHOD_post_action_type, (METHOD_function_t)PLM_PostAction, NULL);

		return status;
	}

	extern DLLAPI int PLM_PostAction(METHOD_message_t* msg, va_list args) {
		tag_t source_rev = NULLTAG;
		tag_t tRelationType = NULLTAG;
		tag_t* tSecObjs = NULLTAG;
		source_rev = va_arg(args, tag_t);
		int n_sec = 0;
		char* type = NULL;

		if (GRM_find_relation_type("IMAN_master_form_rev", &tRelationType) == status)
		{
			if (GRM_list_secondary_objects_only(source_rev, tRelationType, &n_sec, &tSecObjs) == status)
			{
				if (n_sec != 0)
				{
					for (int i = 0; i < n_sec; i++) {
						AOM_ask_value_string(tSecObjs[i], "object_type", &type);
						if (strcmp(type,"ItemRevision Master") == 0)
						{
							AOM_set_value_string(tSecObjs[i], "user_data_1", "HAALOOO after saveAs");
							AOM_save_without_extensions(tSecObjs[i]);
						}
					}
				}
				else
				{
					cout << "Don't have secondaries\n\n";
				}
			}
		}
		else
		{
			cout << "Cannot get Relation type\n\n";
		}
		return status;
	}

	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "* * * * Teamcenter Logout Success * * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		return status;
	}
}