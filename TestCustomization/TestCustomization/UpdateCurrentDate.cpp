#include<iostream>
#include<tcinit/tcinit.h>
#include<tc/emh.h>
#include<base_utils/TcResultStatus.hxx>
#include<base_utils/ScopedSmPtr.hxx>
#include<base_utils/ScopedPtr.hxx>
#include<base_utils/IFail.hxx>
#include<tc/tc_macros.h>
#include<vector>
#include<sstream>
#include<tccore/project.h>
#include<tccore/aom.h>
#include<tccore/item.h>
#include<tccore/aom_prop.h>
#include<ctime>
#include<iomanip>
#include<fclasses/tc_date.h>
using namespace std;
using namespace Teamcenter;

#ifdef _WIN32
string path = "D:\\Files\\Text_Files\\outputFile.txt ";
#else
string path = "teamcenter/apps/volume";
#endif

#define IZ_free_memory(p) {\
		if ( p != NULL ) {\
			MEM_free(p);\
			p = NULL;\
		}\
}

void display() {
	cout << "\n -u\t ---> username";
	cout << "\n -p\t ---> password";
	cout << "\n -g\t ---> group";
}

vector<string> splitByDelimiter(const string& inputLine, char delimiter) {
	vector<string> result;
	stringstream ss(inputLine);
	string token;

	while (getline(ss, token, delimiter)) {
		result.push_back(token);
	}

	return result;
}

string getCurrentDateTime()
{
	std::time_t now = std::time(nullptr);
	std::tm localTime{};

	localtime_s(&localTime, &now);

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

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	ResultStatus status(0);

	try {
		const char* user = ITK_ask_cli_argument("-u=");
		const char* pass = ITK_ask_cli_argument("-p=");
		const char* grp = ITK_ask_cli_argument("-g=");

		if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function
			display();
			return ifail;
		}

		if ((user && tc_strcmp(user, "") != 0) && (pass && tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {
			ITK_init_to_login();
			status = ITK_init_module(user, pass, grp);
			time_t tRawTime;
			struct tm* timeInfo;
			char timeStamp[20];
			time(&tRawTime);
			timeInfo = new struct tm;
			gmtime_s(timeInfo, &tRawTime);
			strftime(timeStamp, sizeof(timeStamp), "%d-%m-%Y %H:%M:%S", timeInfo);
			TC_write_syslog("[%s] User '%s' login successful.", timeStamp, user);

			tag_t tItem = NULLTAG;
			ITKCALL(ITEM_find_item("000181", &tItem));
			date_t localDate = NULLDATE;
			string dateTime = getCurrentDateTime();
			ITKCALL(DATE_convert_formatted_string_to_date(dateTime.c_str(), "%Y-%m-%d %H:%M", false, true, &localDate));
			status = POM_set_env_info(POM_bypass_attr_update, FALSE, 0, 0, NULLTAG, NULL);
			status = POM_refresh_instances_any_class(1, &tItem, POM_modify_lock);
			status = POM_set_modification_date(tItem, localDate);
			status = POM_save_instances(1, &tItem, false);
			status = POM_refresh_instances_any_class(1, &tItem, POM_no_lock);
		}
		else {
			display();
			return ifail;
		}
	}
	catch (IFail& ex) {
		ifail = ex.ifail();
		scoped_smptr <char> message;
		EMH_ask_error_text(ifail, &message);
		//writeToFile(message.getString());
		TC_write_syslog("\nThe error is %s", message.getString());
		cout << "\nThe error is " << message.getString();
	}
	return ifail;
}