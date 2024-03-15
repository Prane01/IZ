#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<epm/epm.h>
#include<tccore/grm.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<ce/ce.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +10)
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
	extern DLLAPI int check_specific_group(EPM_rule_message_t msg);
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
		EPM_register_rule_handler("custom-check-owning-login-group", "custom-check-owning-login-group", (EPM_rule_handler_t)check_specific_group);
		return status;
	}

	extern DLLAPI int check_specific_group(EPM_rule_message_t msg) {
		tag_t root_task = NULLTAG;
		int attCount = 0;
		tag_t* attachments = NULL;
		int decision = 0;
		bool flag = false;
		tag_t userSession = NULLTAG;
		char* current_group = NULL;
		char* owning_group = NULL;
		char* p1 = NULL;
		ifail = CE_current_user_session_tag(&userSession);
		checkifail();
		ifail = PROP_ask_composite_property(userSession, "group", &current_group);
		checkifail();
		cout << "Current group : "<< current_group << endl;
		ifail = EPM_ask_root_task(msg.task, &root_task);
		checkifail();
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &attCount, &attachments);
		checkifail();
		if (attCount > 0 && attachments != NULL)
		{
			for (int i = 0; i < attCount; i++) {
				ifail = PROP_ask_composite_property(attachments[i], "owning_group", &owning_group);
				checkifail();
				cout << "Owning group : " << current_group << endl;
				if (tc_strcmp(current_group, owning_group) != 0)
				{
					ifail =AOM_ask_value_string(attachments[i], "object_string", &p1);
					checkifail();
					EMH_store_error_s1(EMH_severity_error, PLM_error, p1);
					flag = true;
				}
			}
			if (flag)
			{
				decision = EPM_nogo;
				return PLM_error;
			}
			else {
				decision = EPM_go;
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