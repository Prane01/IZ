#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<epm/epm.h>
#include<tccore/aom_prop.h>
#include<tccore/grm.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +5)

using namespace std;

int status = 0;
char* cError = NULL;

int checkNullTag(tag_t tag);
extern "C" {

	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int add_custom_rule(EPM_rule_message_t msg);
	extern DLLAPI int add_custom_status(EPM_action_message_t msg);
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

		EPM_register_rule_handler("custom-check-sec-dataset", "Creating custom status", (EPM_rule_handler_t)add_custom_rule);
		EPM_register_action_handler("custom-status", "custom-status", (EPM_action_handler_t)add_custom_status);
		return status;
	}

	extern DLLAPI int add_custom_rule(EPM_rule_message_t msg) {
		EPM_decision_t decision = EPM_nogo;
		tag_t root_task = NULLTAG;
		int attCount = 0;
		tag_t* attachments = NULLTAG;
		tag_t class_idP = NULLTAG;
		char* class_nameP = NULL;
		tag_t class_idC = NULLTAG;
		char* class_nameC = NULL;
		bool flag = true;
		char* p1 = NULL;
		int secCount = 0;
		tag_t* secObjects = NULLTAG;
		int secDataCount = 0;
		tag_t* secDatasets = NULLTAG;
		int check = 0;

		status = EPM_ask_root_task(msg.task, &root_task);
		checkNullTag(root_task);
		status = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		if (attCount != 0)
		{
			for (int i = 0; i < attCount; i++)
			{
				GRM_list_secondary_objects_only(attachments[i], NULLTAG, &secCount, &secObjects);
				for (int z = 0; z < secCount; z++)
				{
					status = POM_class_of_instance(secObjects[z], &class_idP);
					status = POM_name_of_class(class_idP, &class_nameP);
					if (tc_strcmp(class_nameP, "DocumentRevision") == 0)
					{
						GRM_list_secondary_objects_only(secObjects[z], NULLTAG, &secDataCount, &secDatasets);
						for (int j = 0; j < secDataCount; j++)
						{
							status = POM_class_of_instance(secDatasets[j], &class_idC);
							status = POM_name_of_class(class_idC, &class_nameC);
							if (tc_strcmp(class_nameC, "Dataset") == 0)
							{
								flag = false;
								break;
							}
						}
						if (flag)
						{
							AOM_ask_value_string(secObjects[z], "object_string", &p1);
							EMH_store_error_s1(EMH_severity_error, PLM_error, p1);
							check++;
						}
					}
					flag = true;
				}
			}
			if (check == 0)
			{
				decision = EPM_go;
				cout << "... Released status has been set ...\n\n";
			}
			else {
				decision = EPM_nogo;
				return PLM_error;
			}
		}
		return decision;
	}

	extern DLLAPI int add_custom_status(EPM_action_message_t msg) {
		tag_t root_task = NULLTAG;
		int attCount = 0;
		tag_t* attachments = NULLTAG;
		tag_t class_id = NULLTAG;
		char* class_name = NULL;
		tag_t rel_status = NULLTAG;

		status = EPM_ask_root_task(msg.task, &root_task);
		checkNullTag(root_task);
		status = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		if (attCount != 0)
		{
			status = RELSTAT_create_release_status("CustomStatusDLL", &rel_status);
			checkNullTag(rel_status);
			status = RELSTAT_add_release_status(rel_status, attCount, attachments, true);
			cout << "... Released status has been set ...\n\n";
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

int checkNullTag(tag_t tag)
{
	if (tag == NULLTAG)
	{
		EMH_ask_error_text(status, &cError);
		cout << "The Error is: " << cError;
		exit(0);
	}
	else
	{
		return 0;
	}
}