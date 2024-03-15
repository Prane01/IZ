#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tccore/item.h>
#include<tccore/grm.h>
#include<fclasses/tc_string.h>
#include<tccore/aom_prop.h>
#define DLLAPI _declspec(dllexport)
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
		tag_t source_Item = NULLTAG;
		tag_t* secObj = NULLTAG;
		tag_t* rev_list = NULLTAG;
		int iCount = 0;
		int sCount = 0;
		tag_t* RelationList = NULLTAG;
		tag_t Relation = NULLTAG;
		tag_t new_relation = NULLTAG;
		tag_t new_relation2 = NULLTAG;
		tag_t cls_id = NULLTAG;
		char* name = NULLTAG;
		char* name2 = NULLTAG;
		int nRel = 0;
		int tRel = 0;

		cout << "Pre Action began...\n\n";

		status = ITEM_ask_item_of_rev(source_rev, &source_Item);

		if (ITEM_list_all_revs(source_Item, &iCount, &rev_list) == status)
		{
			GRM_list_relation_types(&tRel, &RelationList);
			if (GRM_list_secondary_objects_only(source_rev, NULLTAG, &sCount, &secObj) == status)
			{
				for (int i = 0; i < iCount; i++)
				{	
					if (source_rev == rev_list[i]) {
						for (int j = 0; j < sCount; j++) {
							POM_class_of_instance(secObj[j], &cls_id);
							POM_name_of_class(cls_id, &name);
							if (tc_strcmp(name, "Dataset") == 0)
							{
								for (int z = 0; z < tRel; z++)
								{
									GRM_find_relation(source_rev, secObj[j], RelationList[z], &Relation);
									if (Relation != NULLTAG)
									{
										GRM_create_relation(rev_list[i - 1], secObj[j], RelationList[z], NULLTAG, &new_relation);
										GRM_save_relation(new_relation);
										cout << "\n\nDone * * * ";
									}
								}
							}
						}
					}
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