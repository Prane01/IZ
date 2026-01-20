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
			const char* uid = "QWERTYUIHUJK";
			char* id = NULL;
			tag_t tag = (*(tag_t*)(uid));
			AOM_ask_value_string(tag, "item_id", &id);
			cout <<"id : " << id << endl;
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
		TC_write_syslog("  in file [" __FILE__ "], line [%d]\n\n", __LINE__);
		cout << "  in file [" __FILE__ "], line [%d]\n\n", __LINE__;
		cout << "\nThe error is " << message.getString();
	}
	return ifail;
}