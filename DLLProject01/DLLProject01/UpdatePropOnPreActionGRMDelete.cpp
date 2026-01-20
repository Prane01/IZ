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
#include<string>
#include<ctime>
#include<sstream>
#include<iomanip>
#include<tccore/grm.h>
#include<fclasses/tc_date.h>
#define DLLAPI _declspec(dllexport)
using namespace std;
using namespace Teamcenter;

ofstream outFile;
METHOD_id_t tmethod_id;
extern "C" {
	ResultStatus status(0);
	int ifail = ITK_ok;
	string getCurrentDateTime();

	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int updatePropertyOnSecondary(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	//Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return ifail;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		outFile.open("C:\\Users\\13351\\source\\repos\\outputLog.txt");
		if (outFile.is_open()) { // if the file open
			outFile << " * * * * * * * * * * * * * * * * * * * * \n";
			outFile << " * * * Teamcenter Login Success * * * \n";
			outFile << " * * * * * * * * * * * * * * * * * * * * \n\n";
			status = METHOD_find_method("IMAN_specification", "IMAN_delete", &tmethod_id);
			status = METHOD_add_action(tmethod_id, METHOD_pre_action_type, (METHOD_function_t)updatePropertyOnSecondary, NULL);
			return ifail;
		}
		else {
			cerr << "Unable to open file outputLog.txt";
			return 1; // Return an error code
		}
	}

	extern DLLAPI int updatePropertyOnSecondary(METHOD_message_t* msg, va_list args) {
		va_list largs;
		va_copy(largs, args);
		tag_t tRelation = va_arg(largs, tag_t);
		va_end(largs);

		try {
			date_t localDate = NULLDATE;
			tag_t tPrimaryObj = NULLTAG;
			tag_t tSecondaryObj = NULLTAG;
			tag_t RelationType = NULLTAG;
			char* PrimaryObjectType = NULL;
			char* SecondaryObjectType = NULL;

			status = GRM_ask_primary(tRelation, &tPrimaryObj);
			status = GRM_ask_secondary(tRelation, &tSecondaryObj);

			if (tPrimaryObj != NULLTAG && tSecondaryObj != NULLTAG)
			{
				status = WSOM_ask_object_type2(tPrimaryObj, &PrimaryObjectType);
				status = WSOM_ask_object_type2(tSecondaryObj, &SecondaryObjectType);

				if (tc_strcmp(PrimaryObjectType, "ItemRevision") == 0 && tc_strcmp(SecondaryObjectType, "ItemRevision") == 0)
				{
					string dateTime = getCurrentDateTime();
					status = DATE_convert_formatted_string_to_date(dateTime.c_str(), "%Y-%m-%d %H:%M", false, true, &localDate);
					status = POM_set_env_info(POM_bypass_attr_update, FALSE, 0, 0, NULLTAG, NULL);
					status = POM_refresh_instances_any_class(1, &tSecondaryObj, POM_modify_lock);
					status = POM_set_modification_date(tSecondaryObj, localDate);
					status = POM_save_instances(1, &tSecondaryObj, POM_no_lock);
					status = POM_refresh_instances_any_class(1, &tSecondaryObj, POM_no_lock);
				}
				else {
					TC_write_syslog("\n Primary or Secondary object type is not matched");
					return ITK_ok;
				}
			}
			else {
				TC_write_syslog("\n Primary or Secondary object tag is NULL");
				return ITK_ok;
			}
		}
		catch (IFail& ex) {
			ifail = ex.ifail();
			scoped_smptr <char> message;
			EMH_ask_error_text(ifail, &message);
			TC_write_syslog("\nThe error is %s", message.getString());
		}
		TC_write_syslog("\nExiting from the IMAN_delete Pre-Action Extension Code block");
		return ifail;
	}

	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		outFile << "* * * * * * * * * * * * * * * * * * * * \n\n";
		outFile << "***** Teamcenter Logout Success *****\n\n";
		outFile << "* * * * * * * * * * * * * * * * * * * * \n\n";
		outFile.close();
		return ifail;
	}

	string getCurrentDateTime()
	{
		std::time_t now = std::time(nullptr);
		std::tm localTime{};

		#ifdef _WIN32
				localtime_s(&localTime, &now);   // Windows
		#else
				localtime_r(&now, &localTime);   // Linux/Unix
		#endif

		std::ostringstream dateTimeStream;
		dateTimeStream << (1900 + localTime.tm_year) << "-"
			<< std::setw(2) << std::setfill('0') << (localTime.tm_mon + 1) << "-"
			<< std::setw(2) << std::setfill('0') << localTime.tm_mday << " "
			<< std::setw(2) << std::setfill('0') << localTime.tm_hour << ":"
			<< std::setw(2) << std::setfill('0') << localTime.tm_min;

		return dateTimeStream.str();
	}
}