#pragma once
#undef DEBUG
#undef _DEBUG
#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<epm/epm.h>
#include<tccore/aom_prop.h>
#include<fstream>
#include<base_utils/TcResultStatus.hxx>
#include<base_utils/ScopedPtr.hxx>
#include<base_utils/IFail.hxx>
#include<base_utils/ScopedSmPtr.hxx>
#include<tc/tc_macros.h>
#define DLLAPI _declspec(dllexport)
using namespace std;
using namespace Teamcenter;

int status = 0;
ofstream outFile;

extern "C" {
	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int delete_approved_status(EPM_action_message_t msg);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	//Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		outFile.open("C:\\Users\\13351\\source\\repos\\outputLog.txt");
		if (outFile.is_open()) { // if the file open
			outFile << " * * * * * * * * * * * * * * * * * * * * \n";
			outFile << " * * * Teamcenter Login Success * * * \n";
			outFile << " * * * * * * * * * * * * * * * * * * * * \n\n";
			EPM_register_action_handler("IZ-delete-status", "Deleting status", (EPM_action_handler_t)delete_approved_status);
			return status;
		}
		else {
			cerr << "Unable to open file outputLog.txt";
			return 1; // Return an error code
		}
	}

	extern DLLAPI int delete_approved_status(EPM_action_message_t msg) {
		tag_t root_task = NULLTAG;
		int attCount = 0;
		tag_t* attachments = NULL;
		tag_t class_id = NULLTAG;
		char* class_name = NULL;
		//char** class_name1 = NULL;
		tag_t rel_status = NULLTAG;

		//TCTYPE_set_create_display_value(root_task, "item_id", 1, class_name1);

		status = EPM_ask_root_task(msg.task, &root_task);
		status = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		if (attCount > 0)
		{
			for (int i = 0; i < attCount; i++)
			{
				status = POM_class_of_instance(attachments[i], &class_id);
				status = POM_name_of_class(class_id, &class_name);
				int n_status = 0;
				tag_t* statusList = NULL;
				tag_t tAttrId = NULLTAG;
				if (tc_strcmp(class_name, "ItemRevision") == 0)
				{
					status = WSOM_ask_release_status_list(attachments[i], &n_status, &statusList);
					status = POM_attr_id_of_attr("release_status_list", "WorkspaceObject",&tAttrId);
					for (int ii = 0; ii < n_status; ii++)
					{
						scoped_smptr<char> name;
						status = AOM_ask_value_string(statusList[ii], "name", &name);
						if (strcmp(name.get(), "Approved") == 0)
						{
							cout << name.get() << endl;
							tag_t status_to_delete = statusList[ii];
							ITKCALL(POM_refresh_instances_any_class(1, &attachments[i], POM_modify_lock));
							ITKCALL(POM_remove_from_attr(1, &attachments[i], tAttrId, ii, 1));
							logical unload = true;
							ITKCALL(POM_save_instances(1, &attachments[i], unload));
							/*ITKCALL(POM_refresh_instances_any_class(1, &status_to_delete,POM_delete_lock));
							ITKCALL(POM_delete_instances(1,&status_to_delete));*/
						}
					}
					outFile << "* * * Released status has been set * * *\n\n";
				}
			}
		}
		return status;
	}

	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		outFile << "* * * * * * * * * * * * * * * * * * * * \n\n";
		outFile << "***** Teamcenter Logout Success *****\n\n";
		outFile << "* * * * * * * * * * * * * * * * * * * * \n\n";
		outFile.close();
		return status;
	}
}