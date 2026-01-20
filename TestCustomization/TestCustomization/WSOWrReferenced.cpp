#include <iostream>
#include<fstream>
#include<tccore/tctype.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<tc/tc_startup.h>
#include<tcinit\tcinit.h>
#include<tccore/item.h>
#include<tc\emh.h>
#include<stdlib.h>
#include<string.h>
#include<tccore/workspaceobject.h>
#include<base_utils/TcResultStatus.hxx>
#include<base_utils/ScopedSmPtr.hxx>
#include<base_utils/ScopedPtr.hxx>
#include<base_utils/IFail.hxx>
using namespace Teamcenter;
using namespace std;

void display();

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	int hits = 0;
	tag_t* list = NULL;
	char* s1 = NULL;
	int referencers = 0;
	int* levels = 0;
	tag_t* tReferencers = NULL;
	char** relations = NULL;
	char* s2 = NULL;

	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			ResultStatus status(0);
			try {
				/*ifail = WSOM_find2("XSAXSAXz", &hits, &list);
				AOM_ask_value_string(list[0], "object_type", &s1);
				cout << "Type: " << s1 << '\n';*/
				tag_t tRevision = NULLTAG;
				
				status = ITEM_find_rev("000081", "A", &tRevision);
				cout << "tRevision: " << tRevision << endl;
				status = AOM_ask_value_string(tRevision, "object_type", &s1);
				cout << "s1: " << s1 << endl;
				/*if (tc_strcmp(s1, "ExtensionRevision") == 0)
				{*/
					status = WSOM_where_referenced2(tRevision, 1, &referencers, &levels, &tReferencers, &relations);
					cout << "Total no.of references found: " << referencers << endl;
					cout << "Levels of references found: " << *levels << endl;
					for (int i = 0; i < referencers; i++)
					{
						status = AOM_ask_value_string(tReferencers[i], "object_string", &s2);
						cout << "relations at: " << relations << endl;
						cout << "Referenced at: " << s2 << endl;
					}
				//}
			}
			catch (IFail& ex) {
				ifail = ex.ifail();
				scoped_smptr <char> message;
				EMH_ask_error_text(ifail, &message);
				//writeToFile(message.getString());
				TC_write_syslog("\nThe error is %s", message.getString());
				cout << "\nThe error is " << message.getString();
			}
		}
	}
	return ifail;
}

void display() {
	cout << "\n -u\t ---> Enter username";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
}
