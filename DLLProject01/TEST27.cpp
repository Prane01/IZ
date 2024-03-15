#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<epm/epm.h>
#include<bom/bom.h>
#include<tccore/aom.h>
#include<tccore/aom_prop.h>
using namespace std;
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +18)
int ifail = ITK_ok;
extern "C"
{
	extern int DLLAPI DLLProject01_register_callbacks();
	extern int DLLAPI DLLProject01_execute_callback(int* decision, va_list args);
	extern int DLLAPI check_project(EPM_rule_message_t msg);

	extern int DLLAPI DLLProject01_register_callbacks()
	{
		ifail = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)DLLProject01_execute_callback);
		cout << "Dll Registered: \n";
		return ifail;
	}
	extern int DLLAPI DLLProject01_execute_callback(int* decision, va_list args)
	{
		*decision = ALL_CUSTOMIZATIONS;
		cout << "_Login Successfull_: \n";
		ifail = EPM_register_rule_handler("check_for_projects", "check_for_projects", (EPM_rule_handler_t)check_project);
		return ifail;
	}
	extern int DLLAPI check_project(EPM_rule_message_t msg)
	{
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

		ifail = EPM_ask_root_task(msg.task, &root_task);
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		if (attCount > 0)
		{
			cout << attCount << endl;
			for (int i = 0; i < attCount; i++)
			{
				ifail = BOM_create_window(&tWindow);
				ifail = BOM_set_window_top_line(tWindow, NULLTAG, attachments[i], NULLTAG, &tTopLine);
				ifail = BOM_line_ask_all_child_lines(tTopLine, &childs, &tChildren);
				AOM_ask_value_string(tTopLine, "bl_item_project_list", &value);
				cout << value << endl;
				if (tc_strcmp(value, "") == 0)
				{
					AOM_ask_value_string(tTopLine, "bl_indented_title", &p1);
					EMH_store_error_s1(EMH_severity_error, PLM_error, p1);
					cout << p1 << endl;
				}
				else
				{
					check++;
				}
				cout << childs << endl;
				if (childs > 0)
				{
					for (int j = 0; j < childs; j++)
					{
						AOM_ask_value_string(tChildren[j], "bl_item_project_list", &value);
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
			cout << check << endl;
			if (check == (childs + 1))
			{
				decision = EPM_go;
			}
			else
			{
				decision = EPM_nogo;
			}
		}
		return decision;
	}
}