#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tcinit/tcinit.h>
#include<tccore/item.h>
#include<tc/preferences.h>
#include<fclasses/tc_string.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<lov/lov.h>
#define DLLAPI _declspec(dllexport)
using namespace std;

extern "C" {
	METHOD_id_t tmethod_id;
	int status = 0;
	int ifail = 0;
	char* cError = NULL;
	int prefCount = 0;
	char** values = NULL;
	tag_t tLOV = NULLTAG;
	tag_t tLOV1 = NULLTAG;
	char* filledPref01 = NULL;
	char* p1;
	char* p2;
	char** val = NULL;
	char** val1 = NULL;
	int see = 0;
	int numvalues = 0;
	int numvalues1 = 0;
	char* alpha = NULL;

	// Declaration of function
	int checkifail();
	int checkNullTag(tag_t tag);
	void set_value_on_prop02(const char* item_id);
	void set_value_on_prop01(const char* item_id);
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int set_preference_value(METHOD_message_t* msg, va_list args);
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
		status = METHOD_find_method("Item", "ITEM_create", &tmethod_id);
		status = METHOD_add_action(tmethod_id, METHOD_post_action_type, (METHOD_function_t)set_preference_value, NULL);
		return status;
	}

	extern DLLAPI int set_preference_value(METHOD_message_t* msg, va_list args) {
		const char* item_id = NULL;
		item_id = va_arg(args, char*);
		tag_t item = NULLTAG;
		item = va_arg(args, tag_t);
		tag_t tItem = NULLTAG;

		ifail = PREF_ask_char_values("set_property02", &prefCount, &values);
		ITEM_find_item(item_id, &tItem);
		PROP_ask_composite_property(tItem, "a2Prop1", &filledPref01);
		if (tc_strcmp(filledPref01, "") != 0)
		{
			int j = 0;
			for (int i = 0; i < prefCount; i++)
			{
				p1 = strtok(values[i], ":");
				while (p1 != NULL) {
					cout << p1 << endl;
					if (j % 2 == 0) { alpha = p1; }
					if (j % 2 == 1 && tc_strcmp(filledPref01, alpha) == 0){
						set_value_on_prop02(item_id);
						return status;
					}
					p1 = strtok(NULL, ":");
					j++;
				}
			}
		}
		else {
			PROP_ask_composite_property(tItem, "a2Prop2", &filledPref01);
			int j = 0;
			for (int i = 0; i < prefCount; i++)
			{
				p1 = strtok(values[i], ":");
				while (p1 != NULL) {
					cout << p1 << endl;
					if (j % 2 == 0){ alpha = p1; }
					if (j % 2 == 1 && tc_strcmp(filledPref01, p1) == 0){
						set_value_on_prop01(item_id);
						return status;
					}
					p1 = strtok(NULL, ":");
					j++;
				}
			}
		}
		return status;
	}

	void set_value_on_prop02(const char* item_id) {
		tag_t tItem = NULLTAG;
		ITEM_find_item(item_id, &tItem);
		AOM_refresh(tItem, true);
		cout << "\n\n Object CheckedOut Successfully..\n";
		AOM_set_value_string(tItem, "a2Prop2", p1);
		AOM_save_without_extensions(tItem);
		cout << "..Done..";
		AOM_refresh(tItem, false);
		cout << "\n\n Object CheckedIn Successfully..\n";
		
	}

	void set_value_on_prop01(const char* item_id) {
		tag_t tItem = NULLTAG;
		ITEM_find_item(item_id, &tItem);
		AOM_refresh(tItem, true);
		cout << "\n\n Object CheckedOut Successfully..\n";
		AOM_set_value_string(tItem, "a2Prop1", alpha);
		AOM_save_without_extensions(tItem);
		cout << "..Done..";
		AOM_refresh(tItem, false);
		cout << "\n\n Object CheckedIn Successfully..\n";	
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