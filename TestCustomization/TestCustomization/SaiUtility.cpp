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
#include<pom/pom/pom.h>
#include<ics/ics.h>
#include<ics/ics2.h>
#include<me/me.h>
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
#include <sstream>

#include <iostream>
#include<fstream>
#include<string>
#include<stdio.h>
#include<cstdio>
#include <sstream>
#include<vector>
#include<stdlib.h>

using namespace std;

#define HON_free_memory(p) {\
		if ( p != NULL ) {\
			MEM_free(p);\
			p = NULL;\
		}\
}

string filename = "D:\\Honeywell\\SaidheepIMAN_save\\output.txt";
FILE *fptr = fopen(filename.c_str(), "a");

void writeToFile(string text) {

	if (fptr != NULL) {
		fprintf(fptr, text.c_str());
	}
}

int ITK_CALL(int ifail)
{
	char* cError = NULL;

	if (ifail != 0)
	{
		EMH_ask_error_text(ifail, &cError);
		string text(cError);

		writeToFile("The error is : " + text);
		TC_write_syslog("The error is %s", cError);
		exit(0);
	}
	return ifail;
}

void rough(char* uid, char* text) {

	tag_t itemRev = NULLTAG;
	TC_write_syslog("\nEntered into Roughh\n");
	ITK__convert_uid_to_tag(uid, &itemRev);
	TC_write_syslog("\n1111\n");
	ITK_CALL(AOM_refresh(itemRev, 1));
	TC_write_syslog("\n22222\n");
	ITK_CALL(AOM_set_value_string(itemRev, "object_desc", text));
	TC_write_syslog("\n33333\n");
	ITK_CALL(AOM_save_with_extensions(itemRev));
	TC_write_syslog("\n44444\n");
	ITK_CALL(AOM_refresh(itemRev, 0));

}

int ITK_user_main(int argc, char *argv[])

{
	int iFail = 0;


	const char* uid = "infodba";
	const char* group = "dba";
	const char* pass1 = "infodba";

	iFail = ITK_init_module(uid, pass1, group);

	char* itemUID = ITK_ask_cli_argument("-itemUID=");
	char* text = ITK_ask_cli_argument("-text=");

	if (iFail == ITK_ok)
	{
		writeToFile("\nEntered into the utility and logged successfully\n");
		TC_write_syslog("\nEntered into the utility and logged successfully\n", itemUID);
		TC_write_syslog("\nText value: ", text);
		std::cout << "Login successful \n";
		rough(itemUID, text);
	}
	else
	{
		ITK_CALL(iFail);
	}

	return iFail;
}