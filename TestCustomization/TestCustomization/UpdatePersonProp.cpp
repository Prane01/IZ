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
#include<sa/person.h>
#include<sa/user.h>
#include <fstream>
#include  "MyException.h"
using namespace std;
using namespace Teamcenter;

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
	cout << "\n -file\t ---> input file path";
}

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	ResultStatus status(0);

	try {

		const char* user = ITK_ask_cli_argument("-u=");
		const char* pass = ITK_ask_cli_argument("-p=");
		const char* grp = ITK_ask_cli_argument("-g=");
		const char* userIds = ITK_ask_cli_argument("-id=");

		if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function
			display();
			return ifail;
		}

		if ((user && tc_strcmp(user, "") != 0) && (pass && tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {

			ITK_init_to_login();
			status = ITK_init_module(user, pass, grp);

			stringstream ss(userIds);
			string token;

			while (getline(ss, token, ',')) {
				cout << "USER ID :" << token << endl;
				tag_t tUser = NULLTAG;
				tag_t tPerson = NULLTAG;
				ITKCALL(SA_find_user2(token.c_str(), &tUser));
				if (tUser != NULLTAG) {
					ITKCALL(SA_ask_user_person(tUser, &tPerson));
					if (tPerson != NULLTAG) {
						cout << "Person tag : " << tPerson << endl;
						ITKCALL(AOM_refresh(tPerson, true));
						ITKCALL(AOM_set_value_string(tPerson, "i2_is_regulatory", "No"));
						ITKCALL(AOM_save_without_extensions(tPerson));
						ITKCALL(AOM_refresh(tPerson, false));
						scoped_smptr<char> personName;
						scoped_smptr<char> isReg;
						ITKCALL(AOM_ask_value_string(tPerson, "user_name", &personName));
						ITKCALL(AOM_ask_value_string(tPerson, "i2_is_regulatory", &isReg));
						cout << "i2_is_regulatory has been updated for " << personName.getString() << endl;
					}
				}
			}
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