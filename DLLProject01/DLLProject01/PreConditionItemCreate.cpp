#include<iostream>
#include<tc/emh.h>
#include<tccore/method.h>
#include<tccore/custom.h>
#include<fclasses/tc_string.h>
#include<qry/qry.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +19)
using namespace std;

extern "C" {
	METHOD_id_t method;
	int status = 0;
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int preCondition(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "* * * * Teamcenter Login Success * * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";

		status = METHOD_find_method("Item", "ITEM_create", &method);
		status = METHOD_add_pre_condition(method, (METHOD_function_t)preCondition, NULLTAG);
		return status;
	}

	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "* * * * Teamcenter Logout Success * * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		return status;
	}

	extern DLLAPI int preCondition(METHOD_message_t* msg, va_list args) {
		char* item_id = va_arg(args, char*);
		char* item_name = va_arg(args, char*);
		char* type_name = va_arg(args, char*);
		tag_t tQuery = NULLTAG;
		int n_entries = 0;
		int n_found = 0;
		char** entries = NULL;
		char** values = NULL;

		char* userValues[2];

		tag_t* results = NULL;
		status = QRY_find2("Item_name_validate", &tQuery);
		status = QRY_find_user_entries(tQuery, &n_entries, &entries, &values);

		for (int i = 0; i < n_entries; i++)
		{
			if (tc_strcmp(entries[i], "Name") == 0) {
				userValues[i] = item_name;
				cout << entries[i] << " " << userValues[i] << endl;
			}
			if (tc_strcmp(entries[i], "Type") == 0) {
				userValues[i] = type_name;
				cout << entries[i] << " " << userValues[i] << endl;
			}
		}
		status = QRY_execute(tQuery, n_entries, entries, userValues, &n_found, &results);
		cout << n_found << endl;
		if (n_found > 0)
		{
			EMH_store_error_s2(EMH_severity_error, PLM_error, type_name, item_name);
			return PLM_error;
		}
		return status;
	}
}
