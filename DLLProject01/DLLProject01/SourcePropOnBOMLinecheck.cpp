#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tccore/aom_prop.h>
#include<bom/bom_msg.h>
#include<bom/bom.h>
#include<pom/pom/pom.h>
#include<fclasses/tc_string.h>
#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +11)
using namespace std;

extern "C" {

	METHOD_id_t tmethod_id;
	int status = 0;

	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_add_preCondition(METHOD_message_t* msg, va_list args);
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

		status = METHOD_find_method("BOMLine", "BOMLine_add", &tmethod_id);
		status = METHOD_add_pre_condition(tmethod_id, (METHOD_function_t)PLM_add_preCondition, NULL);

		return status;
	}

	extern DLLAPI int PLM_add_preCondition(METHOD_message_t* msg, va_list args) {
		tag_t parent = NULLTAG;
		parent = va_arg(args, tag_t);
		tag_t item = NULLTAG;
		item = va_arg(args, tag_t);
		tag_t source_rev = NULLTAG;
		source_rev = va_arg(args, tag_t);
		char* type1 = NULL;
		char* idP = NULL;
		char* RidP = NULL;
		char* type2 = NULL;
		char* s1 = NULL;
		char* s2 = NULL;
		tag_t rev01 = NULLTAG;


		AOM_ask_value_string(parent, "bl_item_item_id", &idP);
		cout << idP << endl;
		AOM_ask_value_string(parent, "bl_rev_item_revision_id", &RidP);
		cout << RidP << endl;
		ITEM_find_rev(idP, RidP, &rev01);
		AOM_ask_value_string(rev01, "object_type", &type1);
		cout << type1 << endl;
		PROP_ask_composite_property(rev01, "source", &s1);
		cout << s1 << endl;
		AOM_ask_value_string(source_rev, "object_type", &type2);
		cout << type2 << endl;
		PROP_ask_composite_property(source_rev, "source", &s2);
		cout << s2 << endl;

		if (tc_strcmp(type1, "Part Revision") == 0 && tc_strcmp(type2, "Part Revision") == 0 && tc_strcmp(s1, s2) != 0)
		{
			AOM_ask_value_string(source_rev, "object_string", &type2);
			EMH_store_error_s1(EMH_severity_error, PLM_error, type2);
			cout << "Cannot attach\n\n";
			return PLM_error;
		}
		else
		{
			cout << "Revision attached\n\n";
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