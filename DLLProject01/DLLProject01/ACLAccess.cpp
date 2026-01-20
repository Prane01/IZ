#pragma once
#undef DEBUG
#undef _DEBUG
#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<epm/epm.h>
#include<tccore/aom_prop.h>
#include<fstream>
#include<base_utils/TcResultStatus.hxx>
#include<base_utils/ScopedPtr.hxx>
#include<base_utils/IFail.hxx>
#include<base_utils/ScopedSmPtr.hxx>
#include<tc/tc_macros.h>
#include<sa/am.h>
#define DLLAPI _declspec(dllexport)
using namespace std;
using namespace Teamcenter;

int status = 0;
ofstream outFile;
METHOD_id_t tmethod_id;
extern "C" {
	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int apply_acl(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	//Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		outFile.open("C:\\Users\\13351\\source\\repos\\outputLog.txt");
		if (outFile.is_open()) { // if the file open
			outFile << " * * * * * * * * * * * * * * * * * * * * \n";
			outFile << " * * * Teamcenter Login Success * * * \n";
			outFile << " * * * * * * * * * * * * * * * * * * * * \n\n";
			status = METHOD_find_method("ImanRelation", "GRM_create", &tmethod_id);
			status = METHOD_add_action(tmethod_id, METHOD_pre_action_type, (METHOD_function_t)apply_acl, NULL);
			return status;
		}
		else {
			cerr << "Unable to open file outputLog.txt";
			return 1; // Return an error code
		}
	}

	extern DLLAPI int apply_acl(METHOD_message_t* msg, va_list args) {
		va_list largs;
		va_copy(largs, args);
		tag_t tRevision = va_arg(largs, tag_t);
		tag_t tDataset = va_arg(largs, tag_t);
		va_end(largs);
		tag_t effACL = NULLTAG;
		int nACL = 0;
		ResultStatus status(ITK_ok);
		scoped_smptr<char> acl_display_name;
		status = AM_get_effective_acl(tRevision, &effACL, &nACL);
		status = AM_ask_acl_display_name(effACL, &acl_display_name);
		outFile << "acl_display_name : " << acl_display_name.getString() << endl;
		return 0;
	}

	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		outFile << "* * * * * * * * * * * * * * * * * * * * \n\n";
		outFile << "***** Teamcenter Logout Success *****\n\n";
		outFile << "* * * * * * * * * * * * * * * * * * * * \n\n";
		outFile.close();
		return status;
	}
}