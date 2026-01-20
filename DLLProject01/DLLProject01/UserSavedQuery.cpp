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
	extern DLLAPI int PLM_add_postAction(int *decision, va_list args);

	// Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		TC_write_syslog("*****Entered into DLLProject01_register_callbacks *****\n");
		status = CUSTOM_register_exit("ImanQuery", "BMF_QRY_custom_execute", PLM_add_postAction);
		TC_write_syslog("*****Exit from DLLProject01_register_callbacks *****\n");
		return status;
	}

	extern DLLAPI int PLM_add_postAction(int *decision, va_list args) {
		*decision = ALL_CUSTOMIZATIONS;
		TC_write_syslog("***** PLM_add_postAction *****\n");
		va_list largs;
		va_copy(largs, args);
		char *qName = va_arg(largs, char*);
		int num_args = va_arg(largs, int);
		char** names = va_arg(largs, char **);
		char** values = va_arg(largs, char **);
		int* num_found = va_arg(largs, int*);
		tag_t** found = va_arg(largs, tag_t **);
		va_end(largs);
		TC_write_syslog("Query Name: %s\n", qName);
		if (tc_strcmp(qName, "SM_get_all_parents") == 0)
		{
			TC_write_syslog("Query Args: %s\n", num_args);
			for (int i = 0; i < num_args; i++) {
				TC_write_syslog("Name: %s\n", names[i]);
				TC_write_syslog("Value: %s\n", values[i]);
			}
			TC_write_syslog("Found: %d\n",num_found);
			TC_write_syslog("Found Object: %d\n",found);

		}
		return status;
	}
}