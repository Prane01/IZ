#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tccore/grm.h>
#include<tccore/grm_msg.h>
#include<fclasses/tc_string.h>
#include<tccore/aom.h>
#include<tccore/aom_prop.h>
#include<string>
#include<sstream>
#define DLLAPI _declspec(dllexport)
using namespace std;

extern "C" {

	int ifail = 0;
	char* cError = NULL;
	METHOD_id_t tmethod_id;
	int status = 0;

	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	int checkifail();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_add_preAction(METHOD_message_t* msg, va_list args);
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
		cout << "* * * Teamcenter Login Success * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";

		status = METHOD_find_method("Dataset", "AE_delete_dataset", &tmethod_id);
		status = METHOD_add_action(tmethod_id, METHOD_pre_action_type, (METHOD_function_t)PLM_add_preAction, NULL);
		return status;
	}

	extern DLLAPI int PLM_add_preAction(METHOD_message_t* msg, va_list args) {
		tag_t datasetTag = NULLTAG;
		datasetTag = va_arg(args, tag_t);
		int pCount = 0;
		tag_t* primary_objects = NULL;
		int secCount = 0;
		tag_t* secDatasets = NULL;
		int datasetsCount = 0;
		tag_t class_id = NULLTAG;
		char* className = NULL;
		char* base = NULL;
		char* item_id = NULL;
		char* item_revision_id = NULL;
		char newName[100];
		char* prime_type = NULL;
		tag_t prim_rev = NULLTAG;
		/*tag_t relType = NULLTAG;
		ifail = GRM_find_relation_type("IMAN_specification", &relType);
		checkifail();*/
		ifail = GRM_list_primary_objects_only(datasetTag, NULLTAG, &pCount, &primary_objects);
		checkifail();
		cout << pCount << endl;
		if (pCount > 0) {
			for (int i = 0; i < 1; i++)
			{
				AOM_ask_value_string(primary_objects[i],"object_type", &prime_type);
				cout << prime_type << endl;
				if (tc_strcmp(prime_type, "ItemRevision") == 0)
				{
					GRM_list_secondary_objects_only(primary_objects[i], NULLTAG, &secCount, &secDatasets);
					for (int i = 0; i < secCount; i++)
					{
						POM_class_of_instance(secDatasets[i], &class_id);
						POM_name_of_class(class_id, &className);
						if (tc_strcmp(className, "Dataset") == 0)
						{
							datasetsCount++;
						}
					}
				}
			}
		}
		cout << "Datasets : " << datasetsCount << endl;
		int count = 1;
		for (int i = secCount - 1; i >= 0; i--)
		{
			POM_class_of_instance(secDatasets[i], &class_id);
			POM_name_of_class(class_id, &className);
			if (count <= datasetsCount && tc_strcmp(className, "Dataset") == 0) {
				char num[100] = { 0 };
				sprintf(num, "%d", count);

				AOM_ask_value_string(primary_objects[0], "item_id", &item_id);
				//cout << item_id << endl;
				AOM_ask_value_string(primary_objects[0], "item_revision_id", &item_revision_id);
				//cout << item_revision_id << endl;
				AOM_ask_value_string(primary_objects[0], "object_name", &base);
				//cout << base << endl;

				tc_strcpy(newName, item_id);
				tc_strcat(newName, "/");
				tc_strcat(newName, item_revision_id);
				tc_strcat(newName, "/");
				tc_strcat(newName, num);
				tc_strcat(newName, "-");
				tc_strcat(newName, base);

				cout << "New name : " << newName << endl;

				AOM_set_value_string(secDatasets[i], "object_name", newName);
				AOM_save_without_extensions(secDatasets[i]);
				cout << "... Dataset name set ...\n\n";
				count++;
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