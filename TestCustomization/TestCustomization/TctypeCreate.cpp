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
#include<tccore/tctype.h>
#include<tccore/aom_prop.h>
#include<time.h>
#include<ctime>
using namespace std;
using namespace Teamcenter;

#define IZ_free_memory(p) {\
		if ( p != NULL ) {\
			MEM_free(p);\
			p = NULL;\
		}\
}

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	ResultStatus status(0);

	try {
		const char* user = ITK_ask_cli_argument("-u=");
		const char* pass = ITK_ask_cli_argument("-p=");
		const char* grp = ITK_ask_cli_argument("-g=");

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
		tag_t objType = NULLTAG;
		tag_t cInput = NULLTAG;
		tag_t bObject = NULLTAG;
		status = TCTYPE_ask_type("Item",&objType);
		status = TCTYPE_construct_create_input(objType, &cInput);
		TC_write_syslog("\ncInput %d\n", cInput);
		status = AOM_set_value_string(cInput, "object_name", "Test Create Input 01");
		status = AOM_set_value_string(cInput, "object_desc", "Test Create Input Description 01");	
		status = TCTYPE_create_object(cInput, &bObject);
		status = AOM_save_without_extensions(bObject);
		TC_write_syslog("\nbObject %d\n", bObject);
	}
	catch (IFail &ex) {
		ifail = ex.ifail();
		ITKCALL(ifail);
		scoped_smptr <char> message;
		cout << "\nThe error is " << message.getString();
	}
	return ifail;
}