#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tccore/aom_prop.h>
#include<fclasses/tc_string.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +3)
using namespace std;

extern "C" {

	METHOD_id_t tmethod_id;
	int status = 0;

	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_add_postAction(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	// Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "* * * Teamcenter Login Success * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		status = METHOD_find_method("ImanQuery", "BMF_QRY_custom_execute", &tmethod_id);
		status = METHOD_add_action(tmethod_id, METHOD_post_action_type, PLM_add_postAction, NULL);
		return status;
	}

	extern DLLAPI int PLM_add_postAction(METHOD_message_t* /*msg*/, va_list args) {
		char *qName = va_arg(args, char*);
		int num_args = va_arg(args, int);
		char** names = va_arg(args, char **);
		char** values = va_arg(args, char **);
		int* num_found = va_arg(args, int*);
		tag_t** found = va_arg(args, tag_t **);
		va_end(args);
		cout << "object_string : " << endl;
		if (tc_strcmp(qName, "01_TC_set_query_where_run") == 0)
		{
			tag_t tObject = NULLTAG;
			ITK__convert_uid_to_tag(values[0], &tObject);

			*num_found = 1;
			*found = (tag_t *)MEM_alloc(*num_found * sizeof(tag_t));
			(*found)[0] = tObject;
			char* object_string;
			AOM_ask_value_string(tObject, "object_string", &object_string);
			cout << "object_string : " << object_string << endl;
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
}