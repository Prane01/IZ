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

string filename = "D:\\CSV\\CSV\\CsvFile.csv";
FILE* file = fopen(filename.c_str(), "w");

void writeToFile(string text) {

	if (file != NULL) {
		fprintf(file, text.c_str());
	}
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

int ifail = 0;
char* cError = NULL;
ResultStatus status(0);

int ITK_user_main(int argc, char* argv[])
{
	try {
		const char* user = ITK_ask_cli_argument("-u=");
		const char* pass = ITK_ask_cli_argument("-p=");
		const char* grp = ITK_ask_cli_argument("-g=");

		ITK_init_to_login();
		status = ITK_init_module(user,pass,grp);

	}
	catch (IFail &ex) {
		cout << "\nException caught!";
		ifail = ex.ifail();
		ITKCALL(ifail);
		scoped_smptr <char> message;
		EMH_ask_error_text(ifail, &message);
		writeToFile(message.getString());
		cout << "\nThe error is " << message.getString();
	}
	return ifail;
}