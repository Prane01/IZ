#include<iostream>
#include<fstream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<epm/epm.h>
#include<bom/bom.h>
#include<cfm/cfm.h>
#include<tccore/aom_prop.h>
#define DLLAPI _declspec(dllexport)
using namespace std;

extern "C" {

	int status = 0;
	int ifail = 0;
	char* cError = NULL;
	fstream fout;
	int cc = 1;

	// Declaration of function
	int checkifail();
	int checkNullTag(tag_t tag);
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int get_properties(EPM_action_message_t msg);
	void findChilds(tag_t* tchildren, int childs);
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
		cout << " * * * Teamcenter Login Success * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * \n\n";
		EPM_register_action_handler("custom-get-properties", "custom-get-properties", (EPM_action_handler_t)get_properties);
		return status;
	}

	extern DLLAPI int get_properties(EPM_action_message_t msg) {
		tag_t root_task = NULLTAG;
		int c = 0;
		tag_t* attachments = NULLTAG;
		tag_t tWindow = NULLTAG;
		tag_t tRule = NULLTAG;
		tag_t tTopLine = NULLTAG;
		int childs = 0;
		tag_t* tchildren = NULLTAG;
		
		ifail = EPM_ask_root_task(msg.task, &root_task);
		checkifail();
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &c, &attachments);
		checkifail();
		cout << c << endl;
		ifail = BOM_create_window(&tWindow);
		checkifail();
		ifail = CFM_find("Latest Working", &tRule);
		checkifail();
		ifail = BOM_set_window_config_rule(tWindow, tRule);
		checkifail();
		ifail = BOM_set_window_top_line(tWindow, NULLTAG, attachments[0], NULLTAG, &tTopLine);
		checkifail();
		ifail = BOM_line_ask_all_child_lines(tTopLine, &childs, &tchildren);
		checkifail();
		fout.open("D:\\testprop.csv", ios::out);
		findChilds(tchildren, childs);
		return status;
	}

	void findChilds(tag_t* tchildren, int childs) {
		int c2 = 0;
		const char* props[5] = {"bl_level_starting_0", "bl_child_item", "bl_rev_item_revision_id", "bl_quantity", "bl_uom"};
		char* values = NULL;
		tag_t* InnerChildren = NULLTAG;
		char* name = NULL;
		if (childs == 0)
		{
			fout.close();
			exit(0);
		}
		else {
			for (int i = 0; i < childs; i++)
			{
				ifail = AOM_ask_value_string(tchildren[i], "bl_indented_title", &name);
				checkifail();
				cout << name << endl;
				//fout << name << endl;
				for (int j = 0; j < 5; j++)
				{
					ifail = PROP_ask_composite_property(tchildren[i], props[j], &values);
					checkifail();
					cout << props[j] << " : " << values << endl;
					if (cc == 1)
					{
						int z = 0;
						while (z < 5) {
							if (z != 4)
							{
								fout << props[z] << "|";
							}
							if (z == 4)
							{
								fout << props[z] << endl;
								cc--;
							}
							z++;
						}
					}
					if (j != 4)
					{
						fout << values << "|";
					}
					if (j == 4)
					{
						fout << values << endl;
					}
				}
				ifail = BOM_line_ask_all_child_lines(tchildren[i], &c2, &InnerChildren);
				checkifail();
				if (c2 > 0)
				{
					findChilds(InnerChildren, c2);
				}
			}
		}
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