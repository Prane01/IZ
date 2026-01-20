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
#include<sa/am.h>
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
			tag_t tRevision = NULLTAG;
			tag_t effACL = NULLTAG;
			tag_t aclId = NULLTAG;
			tag_t* datasets = NULL;
			int nACL = 0;
			int Pcount = 0;
			int n = 0;
			int rules = 0;
			//scoped_smptr<char> acl_display_name;
			scoped_smptr<char> acl_name;
			char** propNames = NULL;
			char** crules = NULL;

			/*status =AM_list_rules(&rules, &crules);
			for (int i = 0; i < rules; i++) {
				cout << i << " = " << crules[i] << endl;
			}*/
			status = ITEM_find_rev("000029", "A", &tRevision);
			status = AM_find_acl("For_IR_and_Sec_Dataset", &aclId);
			/*status = AM_get_effective_acl(tRevision, &effACL, &nACL);
			cout << "nACL : " << nACL << endl;*/
			status = AOM_ask_prop_names(aclId, &Pcount, &propNames);
			/*status = AOM_ask_value_string(aclId, "object_string", &acl_name);
			cout << " acl_name = " << acl_name.getString() << endl;
			status = AOM_ask_value_tags(tRevision, "IMAN_specification", &n, &datasets);*/
			for (int i = 0; i < Pcount; i++) {
				cout << i << " = " << propNames[i] << endl;
			}
			/*status = AOM_ask_value_string(effACL, "rule_name", &acl_name);
			cout << "acl_name : " << acl_name.getString() << endl;*/
			/*status = AM_ask_acl_display_name(effACL, &acl_display_name);
			cout << "acl_display_name : " << acl_display_name.getString() << endl;*/
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
		TC_write_syslog("\nThe error is %s", message.getString());
		cout << "\nThe error is " << message.getString();
	}
	return ifail;
}