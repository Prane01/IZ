//********************** HEADER FOR UTILITY ****************************************

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

#define HON_GENERIC_ERROR (EMH_TC_error_base +3)

#define HON_free_memory(p) {\
		if ( p != NULL ) {\
			MEM_free(p);\
			p = NULL;\
		}\
}

string filename = "D:\\Text Files\\output4.txt"; /*"/Teamcenter/Apps/outputFile.txt";*/
FILE *fptr = fopen(filename.c_str(), "a");

void writeToFile(string text) {

		if(fptr!=nullptr){
			fprintf(fptr, text.c_str());
		}
}

int ITK_CALL(int ifail)
{
	char* cError = NULL;
	string text;
	if (ifail != ITK_ok)
	{
		EMH_ask_error_text(ifail, &cError);
		string errorStr(cError);

		TC_write_syslog("Error occurred which is %s",cError);
		
		text="\n Error occurred ITK_CALL which is "+errorStr;
		writeToFile(text);
		exit(0);
	}
	return ifail;
}