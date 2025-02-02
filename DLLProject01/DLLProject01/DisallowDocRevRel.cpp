#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<pom/pom/pom.h>
#include<fclasses/tc_string.h>
#include<tccore/aom_prop.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +4)
using namespace std;

extern "C" {
	METHOD_id_t tmethod_id;
	int status = 0;

	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_add_preCondition(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	//Definition of function
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

		status = METHOD_find_method("ImanRelation", "GRM_create", &tmethod_id);
		status = METHOD_add_pre_condition(tmethod_id, (METHOD_function_t)PLM_add_preCondition, NULL);
		return status;
	}

	extern DLLAPI int PLM_add_preCondition(METHOD_message_t* msg, va_list args) {
		tag_t primary_obj = NULLTAG;
		primary_obj = va_arg(args, tag_t);
		tag_t secondary_obj = NULLTAG;
		secondary_obj = va_arg(args, tag_t);
		tag_t relation_type = NULLTAG;
		relation_type = va_arg(args, tag_t);
		tag_t class_idP = NULLTAG;
		char* classNameP = NULL;
		int supP = 0;
		tag_t* supClsP = NULLTAG;
		tag_t class_idS = NULLTAG;
		char* classNameS = NULL;
		int supS = 0;
		tag_t* supClsS = NULLTAG;

		char* t1 = NULL;
		char* t2 = NULL;

		char* s1 = NULL;

		POM_class_of_instance(primary_obj, &class_idP);
		POM_name_of_class(class_idP, &classNameP);
		cout << classNameP << endl;

		POM_class_of_instance(secondary_obj, &class_idS);
		POM_name_of_class(class_idS, &classNameS);
		cout << classNameS << endl;

		if (tc_strcmp(classNameP, "ItemRevision") == 0 && tc_strcmp(classNameS, "DocumentRevision") == 0)
		{
			AOM_ask_value_string(secondary_obj, "object_name", &s1);
			EMH_store_error_s1(EMH_severity_error, PLM_error, s1);
			return PLM_error;
		}

		if (tc_strcmp(classNameP, "ItemRevision") != 0)
		{
			POM_superclasses_of_class(class_idP, &supP, &supClsP);
			POM_name_of_class(supClsP[0], &t1);
			cout << t1 << endl;
		}
		
		if (tc_strcmp(classNameS, "DocumentRevision") != 0)
		{
			POM_superclasses_of_class(class_idS, &supS, &supClsS);
			POM_name_of_class(supClsS[0], &t2);
			cout << t2 << endl;
		}
		
		if (tc_strcmp(t1, "ItemRevision") == 0 && tc_strcmp(t2, "DocumentRevision") == 0)
		{
			AOM_ask_value_string(secondary_obj, "object_name", &s1);
			EMH_store_error_s1(EMH_severity_error, PLM_error, s1);
			return PLM_error;
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