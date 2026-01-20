#include<iostream>
#include<tcinit/tcinit.h>
#include<tc/emh.h>
#include<base_utils/TcResultStatus.hxx>
#include<base_utils/ScopedSmPtr.hxx>
#include<base_utils/ScopedPtr.hxx>
#include<base_utils/IFail.hxx>
#include<tc/tc_macros.h>
#include<vector>
#include<string.h>
#include<sstream>
#include<tccore/project.h>
#include<tccore/aom.h>
#include<tccore/item.h>
#include<tccore/aom_prop.h>
#include<time.h>
#include<ctime>
#include<pom/pom/pom.h>
#include<sa/user.h>
#include  "MyException.h"
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
			struct tm *timeInfo;
			char timeStamp[20];
			time(&tRawTime);
			timeInfo = new struct tm;
			gmtime_s(timeInfo, &tRawTime);
			strftime(timeStamp, sizeof(timeStamp), "%d-%m-%Y %H:%M:%S", timeInfo);
			TC_write_syslog("[%s] User '%s' login successful.", timeStamp, user);

			tag_t tUser = NULLTAG;
			char* cpUserName = NULL;
			tag_t tPerson = NULLTAG;
			char* cpCountry = NULL;

			status = POM_get_user(&cpUserName, &tUser);
			cout << "tUser %d\n" << tUser;
			TC_write_syslog("tUser '%d'\n", tUser);

			cout << "cpUserName %s\n" << cpUserName;
			TC_write_syslog("cpUserName '%s'\n", cpUserName);

			status = SA_ask_user_person(tUser, &tPerson);
			cout << "tPerson %d\n" << tPerson;
			TC_write_syslog("tPerson '%d'\n", tPerson);
			status = AOM_ask_value_string(tPerson, "PA5", &cpCountry);
			cout << "cpCountry %s\n" << cpCountry;
			TC_write_syslog("cpCountry '%s'\n", cpCountry);
		}
		else {
			display();
			return ifail;
		}
	}
	catch (IFail &ex) {
		ifail = ex.ifail();
		scoped_smptr <char> message;
		EMH_ask_error_text(ifail, &message);
		//writeToFile(message.getString());
		TC_write_syslog("\nThe error is %s", message.getString());
		cout << "\nThe error is " << message.getString();
	}
	return ifail;
}