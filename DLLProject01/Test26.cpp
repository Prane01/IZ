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
#define PLM_error (EMH_USER_error_base +18)
using namespace std;

extern "C" {

	int status = 0;
	int ifail = 0;
	char* cError = NULL;
	int decisionn = 0;
	bool flag = false;
	int c2 = 0;
	tag_t* InnerChildren = NULLTAG;
	char* name = NULL;

	// Declaration of function
	int checkifail();
	int checkNullTag(tag_t tag);
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int get_properties(EPM_rule_message_t msg);
	int findChilds(tag_t* tchildren, int childs);
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
		EPM_register_rule_handler("custom-project", "custom-project", (EPM_rule_handler_t)get_properties);
		return status;
	}

	extern DLLAPI int get_properties(EPM_rule_message_t msg) {
		tag_t root_task = NULLTAG;
		int c = 0;
		tag_t* attachments = NULLTAG;
		tag_t tWindow = NULLTAG;
		tag_t tRule = NULLTAG;
		tag_t tTopLine = NULLTAG;
		int childs = 0;
		tag_t* tchildren = NULLTAG;
		int check = 0;
		int decision = 0;
		ifail = EPM_ask_root_task(msg.task, &root_task);
		checkifail();
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &c, &attachments);
		checkifail();
		cout << c << endl;
		ifail = BOM_create_window(&tWindow);
		checkifail();
		ifail = BOM_set_window_top_line(tWindow, NULLTAG, attachments[0], NULLTAG, &tTopLine);
		checkifail();
		ifail = BOM_line_ask_all_child_lines(tTopLine, &childs, &tchildren);
		checkifail();
		cout << childs << endl;
		ifail = AOM_ask_value_string(tTopLine, "bl_item_project_list", &name);
		checkifail();
		if (tc_strcmp(name, "") != 0)
		{
			findChilds(tchildren, childs);
			if (flag)
			{
				decision = EPM_nogo;
			}
			else {
				decision = EPM_go;
			}
			return decision;
		}
		AOM_ask_value_string(tTopLine, "bl_item_project_list", &name);
		EMH_store_error_s1(EMH_severity_error, PLM_error, name);
		return PLM_error;
	}

	int findChilds(tag_t* tchildren, int childs) {
		if (childs == 0)
		{
			return 0;
		}
		for (int i = 0; i < childs; i++)
		{
			ifail = AOM_ask_value_string(tchildren[i], "bl_item_project_list", &name);
			checkifail();
			cout << name << endl;
			if (tc_strcmp(name, "") == 0)
			{
				ifail = AOM_ask_value_string(tchildren[i], "bl_indented_title", &name);
				checkifail();
				EMH_store_error_s1(EMH_severity_error, PLM_error, name);
				flag = true;
			}
			ifail = BOM_line_ask_all_child_lines(tchildren[i], &c2, &InnerChildren);
			checkifail();
			if (c2 > 0)
			{
				findChilds(InnerChildren, c2);
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