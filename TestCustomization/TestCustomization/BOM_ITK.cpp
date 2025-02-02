#pragma warning(disable : 4996)
#undef DEBUG
#undef _DEBUG

#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <list>
#include <time.h>
#include <map>
#include <vector>

extern "C"
{
#include <tcinit/tcinit.h>
#include <tccore/aom.h>
#include <unidefs.h>
#include <epm/cr.h>
#include <epm/epm.h>
#include <ecm/ecm.h>
#include <pom/enq/enq.h>
#include <ps/ps.h>
#include <sa/sa.h>
#include <fclasses/tc_date.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include <lov/lov.h>
#include <tccore/tctype.h>
#include <sa/sa.h>
#include <tccore/grm.h>
#include <form/formtype.h>
#include <tc/tc_util.h>
#include <tccore/uom.h>
#include <tccore/idfr.h>
#include <textsrv/textserver.h>
#include <tc/preferences.h>
#include <ae/dataset.h>
#include <sa/tcvolume.h>
#include<epm/epm_task_template_itk.h>
#include <qry/qry.h>
#include<tccore/project.h>
}


//from bmide

#include<tc/tc_startup.h>
#include<tcinit/tcinit.h>
#include <tc/emh.h>
#include<vector>
#include<string>
#include <iostream>
#include <cstring>
#include<map>
#include<set>
#include<stdio.h>
#include <sa/user.h>
#include <sa/group.h>
#include <tccore/method.h>
#include <tccore/aom.h>
#include <tccore/custom.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <tccore/workspaceobject.h>
#include <ae/datasettype.h>
#include <tc/preferences.h>
#include <cfm/cfm.h>
#include <property/propdesc.h>
#include <fclasses/tc_string.h>
#include <algorithm>  
#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <epm/epm_task_template_itk.h>
#include <user_exits/epm_toolkit_utils.h>
#include <pom/pom/pom.h>
#include <ics/ics.h>
#include <ics/ics2.h>
#include <me/me.h>
#include <stdarg.h>
#include <stdlib.h>
#include <tccore/tctype.h>
#include <tccore/item_errors.h>
#include <time.h>
#include <tccore/grm.h>
#include <user_exits/epm_toolkit_utils.h>
#include <tccore/aom_prop.h>
#include <bom/bom.h>
#include <string>
#include <fstream>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <cstdio>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/ScopedPtr.hxx>
#include <base_utils/IFail.hxx>
#include <time.h>
#include <ctime>
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

const char* filename = "D:\\CSV\\CSV\\CsvFile.csv";
std::ofstream file;

void create_csv_file(const char* path) {
	
	file.open(filename, std::ios::binary);
	if (!file.is_open()) {
		perror("Error opening file");
	}
	std::string sLine = "";

	sLine.append("Object Name,Description\n");
	file.write(sLine.c_str(), sLine.size());
}




int ITK_CALL(int ifail)
{
	char* cError = NULL;

	if (ifail != 0)
	{
		EMH_ask_error_text(ifail, &cError);
		string text(cError);

		cout << "the error is" << cError;
		TC_write_syslog("The error is %s", cError);

	}
	return ifail;
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

string escapeForCSV(const std::string& value) {
	std::string escapedValue = "\""; // Start with a double quote
	for (char c : value) {
		if (c == '"') { // Escape double quotes by doubling them
			escapedValue += "\"\"";
		}
		else {
			escapedValue += c;
		}
	}
	escapedValue += "\""; // End with a double quote
	return escapedValue;
}

int ITK_user_main(int a, char* args[]) {

	int ifail = ITK_ok;
	ResultStatus status(0);
	tag_t item = NULLTAG;
	char* name = NULL;
	char* desc = NULL;

	try {

		char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

		char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

		char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

		ITK_init_to_login();
		status = ITK_init_module(uname, pass, grp);

		status = ITEM_find_item("000141", &item);
		status = AOM_ask_value_string(item, "object_name", &name);
		status = AOM_ask_value_string(item, "object_desc", &desc);

		create_csv_file(filename);

		string test = "tes,ing";
		string sLine = "\"" + string(name) + "\",\"" + string(desc) + "\",\"" + test + "\"";

		//sLine.append(escapeForCSV(name)).append(",").append(escapeForCSV(desc)).append(",").append(escapeForCSV(test.c_str()));


		file.write(sLine.c_str(), sLine.size());
		
		file.close();

		ITK_exit_module(true);

	}
	catch (IFail &ex) {

		cout << "\nException caught!";
		ifail = ex.ifail();
		ITKCALL(ifail);
		scoped_smptr <char> message;
		EMH_ask_error_text(ifail, &message);
		cout << "\nThe error is " << message.getString();
	}
	return ifail;
}