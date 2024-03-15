#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<epm/epm.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<ce/ce.h>
#include<tccore/item.h>
#include<tccore/project.h>
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
	extern DLLAPI int add_project_businessObject(EPM_action_message_t msg);
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

		EPM_register_action_handler("custom-assign-project", "custom-assign-project", (EPM_action_handler_t)add_project_businessObject);
		return status;
	}

	extern DLLAPI int add_project_businessObject(EPM_action_message_t msg) {
		tag_t root_task = NULLTAG;
		int attCount = 0;
		tag_t* attachments = NULLTAG;
		int numOfArguments = 0;
		char* argument_name = NULL;
		char* argument_value = NULL;
		char* project_id = NULL;
		tag_t tProject = NULLTAG;
		char* Obj_type = NULL;

		ifail = EPM_ask_root_task(msg.task, &root_task);
		checkifail(); checkNullTag(root_task);
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		checkifail();
		if (attCount > 0)
		{
			numOfArguments = TC_number_of_arguments(msg.arguments);

			for (int i = 0; i < numOfArguments; i++)
			{
				ITK_ask_argument_named_value(TC_next_argument(msg.arguments), &argument_name, &argument_value);
				if (tc_strcmp(argument_name, "project_id") == 0)
				{
					project_id = (char*)MEM_alloc(100);
					//tc_strcpy(project_id, "");
					tc_strcpy(project_id, argument_value);
				}
			}
			ifail = PROJ_find(project_id, &tProject);
			checkifail();
			for (int j = 0; j < attCount; j++)
			{
				ifail = AOM_ask_value_string(attachments[j], "object_type", &Obj_type);
				checkifail();
				cout << Obj_type << endl;
				if (tc_strcmp(Obj_type, "ItemRevision") == 0)
				{
					ifail = PROJ_assign_objects(1, &tProject, 1, &attachments[j]);
					checkifail();
					cout << "Project assigned\n\n";
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