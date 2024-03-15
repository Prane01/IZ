#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<epm/epm.h>
#include<bom/bom.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +8)
using namespace std;

extern "C" {

	int status = 0;
	int ifail = 0;
	char* cError = NULL;

	// Declaration of function
	int checkifail();
	int checkNullTag(tag_t tag);
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int bomLine_property_check(EPM_rule_message_t msg);
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
		cout << "* * * * Teamcenter Login Success * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * \n\n";

		EPM_register_rule_handler("custom-bomline-property", "custom-bom-property", (EPM_rule_handler_t)bomLine_property_check);
		return status;
	}

	extern DLLAPI int bomLine_property_check(EPM_rule_message_t msg) {
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
		int decision = 0;
		int bvrCount = 0;
		tag_t* bvrs = NULL;
		ifail = EPM_ask_root_task(msg.task, &root_task);
		checkifail();
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		checkifail();
		if (attCount > 0)
		{
			cout << attCount << endl;
			for (int i = 0; i < attCount; i++)
			{
				ifail = ITEM_rev_list_all_bom_view_revs(attachments[i], &bvrCount, &bvrs);
				if (bvrCount != 0)
				{
					ifail = BOM_create_window(&tWindow);
					checkifail();
					ifail = BOM_set_window_top_line(tWindow, NULLTAG, attachments[i], NULLTAG, &tTopLine);
					checkifail();
					ifail = BOM_line_ask_all_child_lines(tTopLine, &childs, &tChildren);
					checkifail();
					cout << childs << endl;
					if (childs > 0)
					{
						for (int j = 0; j < childs; j++)
						{
							AOM_ask_value_string(tChildren[j], "bl_all_notes", &value);
							cout << value << endl;
							if (tc_strcmp(value, "") == 0)
							{
								AOM_ask_value_string(tChildren[j], "bl_indented_title", &p1);
								EMH_store_error_s1(EMH_severity_error, PLM_error, p1);
								cout << p1 << endl;
							}
							else
							{
								check++;
							}
							value = NULL;
						}
					}
					BOM_close_window(tWindow);
				}
				else {
					AOM_ask_value_string(attachments[i], "bl_indented_title", &p1);
					EMH_store_error_s1(EMH_severity_error, PLM_error, p1);
					return PLM_error;
				}
			}
			cout << check << endl;
			if (check == childs)
			{
				decision = EPM_go;
			}
			if (check != childs)
			{
				decision = EPM_nogo;
				//return PLM_error;
			}
		}
		return decision;
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