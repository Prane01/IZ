#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<epm/epm.h>
#include<bom/bom.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#define DLLAPI _declspec(dllexport)
using namespace std;

extern "C" {
	int status = 0;
	int ifail = 0;
	char* cError = NULL;
	

	// Declaration of function
	int checkifail();
	int checkNullTag(tag_t tag);
	void release_status(char* id, char* rev);
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int release_bom_childs(EPM_action_message_t msg);
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
		EPM_register_action_handler("custom-bom-release", "custom-bom-release", (EPM_action_handler_t)release_bom_childs);
		return status;
	}

	extern DLLAPI int release_bom_childs(EPM_action_message_t msg) {
		tag_t root_task = NULLTAG;
		int attCount = 0;
		tag_t* attachments = NULLTAG;
		tag_t tWindow = NULLTAG;
		tag_t tTopLine = NULLTAG;
		int childs = 0;
		tag_t tbv = NULLTAG;
		tag_t* tChildren = NULLTAG;
		int n = 0;
		char** values = NULL;
		char* value = NULL;
		char* p1 = NULL;
		int check = 0;
		int relc = 0;
		int decision = EPM_go;
		tag_t tchild = NULLTAG;
		
		int length = 0;
		tag_t* release = new tag_t[length];
		char* type = NULL;
		ifail = EPM_ask_root_task(msg.task, &root_task);
		checkifail();
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		checkifail();
		if (attCount > 0)
		{
			cout << attCount << endl;
			for (int i = 0; i < attCount; i++)
			{
				AOM_ask_value_string(attachments[i], "object_type", &type);
				if (tc_strcmp(type, "ItemRevision") == 0) {
					ifail = BOM_create_window(&tWindow);
					checkifail();
					ifail = BOM_set_window_top_line(tWindow, NULLTAG, attachments[i], NULLTAG, &tTopLine);
					checkifail();
					ifail = BOM_line_ask_all_child_lines(tTopLine, &childs, &tChildren);
					checkifail();
					length = childs;
					cout << childs << endl;
					if (childs > 0)
					{
						int z = 0;
						for (int j = 0; j < childs; j++)
						{
							char* id = NULL;
							char* rev = NULL;
							AOM_ask_displayable_values(tChildren[j], "bl_rev_release_status_list", &relc, &values);
							cout << values[0] << endl;
							if (tc_strcmp(values[0], "") == 0)
							{
								AOM_ask_value_string(tChildren[j], "bl_item_item_id", &id);
								AOM_ask_value_string(tChildren[j], "bl_rev_item_revision_id", &rev);
								release_status(id, rev);
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
	void release_status(char* id, char* rev) {
		tag_t child = NULLTAG;
		tag_t rel_status = NULLTAG;
		ITEM_find_rev(id, rev, &child);
		status = RELSTAT_create_release_status("CustomStatusDLL", &rel_status);
		checkNullTag(rel_status);
		status = RELSTAT_add_release_status(rel_status, 1, &child, true);
		cout << "... Released status has been set ...\n\n";
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