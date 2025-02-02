#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<fclasses/tc_string.h>
#include<property/prop.h>
#include<ce/ce.h>
#include<pom/pom/pom.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +12)
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
	extern DLLAPI int pre_action_to_change_request_revision(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
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
		status = METHOD_find_method("ImanRelation", "GRM_create", &tmethod_id);
		status = METHOD_add_pre_condition(tmethod_id, (METHOD_function_t)pre_action_to_change_request_revision, NULL);
		return status;
	}

	extern DLLAPI int pre_action_to_change_request_revision(METHOD_message_t* msg, va_list args) {
		tag_t  primary_object = va_arg(args, tag_t);
		tag_t  secondary_object = va_arg(args, tag_t);
		char* current_role = NULL;
		char* release = NULL;
		char* val = NULL;
		char* s1 = NULL;
		tag_t classId = NULLTAG;
		char* className = NULL;
		tag_t userSession = NULLTAG;
		bool flag = false;
		POM_class_of_instance(primary_object, &classId);
		POM_name_of_class(classId, &className);
		cout << "Class : " << className << endl;
		if (tc_strcmp(className, "ChangeRequestRevision") == 0)
		{
			ifail = CE_current_user_session_tag(&userSession);
			checkifail();
			ifail = PROP_ask_composite_property(userSession, "role", &current_role);
			checkifail();
			cout << "current role : " << current_role << endl;
			ifail = PROP_ask_composite_property(primary_object, "release_status_list", &release);
			checkifail();
			ifail = PROP_ask_composite_property(secondary_object, "object_desc", &val);
			checkifail();
			if (tc_strcmp(release, "") != 0 || tc_strcmp(val, "") == 0 || tc_strcmp(current_role, "DBA") != 0)
			{
				if (tc_strcmp(release, "") != 0){
					flag = true;
					AOM_ask_value_string(primary_object, "object_string", &s1);
					EMH_store_error_s2(EMH_severity_error, PLM_error, s1, "is released");
				}
				if (tc_strcmp(val, "") == 0) {
					flag = true;
					AOM_ask_value_string(secondary_object, "object_string", &s1);
					EMH_store_error_s2(EMH_severity_error, PLM_error, s1, "description is null");
				}
				if (tc_strcmp(current_role, "DBA") != 0) {
					flag = true;
					EMH_store_error_s2(EMH_severity_error, PLM_error, "current role is not", "DBA");
				}
				if (flag)
				{
					return PLM_error;
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