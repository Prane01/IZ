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
#include <windows.h>

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

void RecursiveFunction(tag_t folder, string location);
void downloadFile(tag_t pdf, string location);

#define HON_free_memory(p) {\
		if ( p != NULL ) {\
			MEM_free(p);\
			p = NULL;\
		}\
}

string FolderLoc = "D:\\Folder";

int ITK_CALL(int ifail)
{
	char* cError = NULL;

	if (ifail != 0)
	{
		EMH_ask_error_text(ifail, &cError);

		TC_write_syslog("The error is %s", cError);
		cout << "\n The error is " << cError;
		exit(0);
	}
	return ifail;
}

void rough() {

	tag_t *list, *Foldertags;
	int iCount, folders;
	char *folderName;

	WSO_search_criteria_t ct;
	WSOM_clear_search_criteria(&ct);

	tc_strcpy(ct.name, "Export Dataset from ItemRevision");
	WSOM_search(ct, &iCount, &list);
	cout << "\n The count of the hits is " << iCount;

	if (iCount == 0)
		exit(0);

	WSOM_search(ct, &iCount, &list);
	AOM_ask_value_tags(list[0], "contents", &folders, &Foldertags);
	AOM_ask_value_string(list[0], "object_name", &folderName);

	FolderLoc = FolderLoc + "\\" + folderName + "\"";

	string command = "mkdir \"" + FolderLoc;
	int result = system(command.c_str());


	for (int j = 0; j < folders; j++) {
		RecursiveFunction(Foldertags[j], FolderLoc);
	}

}

void RecursiveFunction(tag_t folder, string location) {
	cout << "\nInside recursive function";

	char* objtype, *objname;
	int icontents;
	tag_t* contents;


	AOM_ask_value_tags(folder, "contents", &icontents, &contents);
	AOM_ask_value_string(folder, "object_name", &objname);

	string loc = location + "\\" + objname;
	string command = "mkdir \"" + loc;
	int result = system(command.c_str());
	for (int i = 0; i < icontents; i++) {

		AOM_ask_value_string(contents[i], "object_type", &objtype);
		cout << "\nThe object type is " << objtype;

		if (tc_strcmp(objtype, "PDF") == 0) {
			downloadFile(contents[i], loc);
		}

		else if (tc_strcmp(objtype, "Folder") == 0) {
			RecursiveFunction(contents[i], loc);
		}
	}
}
void downloadFile(tag_t pdf, string location) {

	tag_t* ta_NamedRefs;
	int iCount, n;
	char** fileName;

	AE_ask_all_dataset_named_refs2(pdf, "PDF_Reference", &iCount, &ta_NamedRefs);

	for (int i = 0; i < iCount; i++) {
		ITK_CALL(IMF_export_file(ta_NamedRefs[i], location.c_str()));
		std::cout << "\nFile exported successfully!\n";
	}

}

int ITK_user_main(int argc, char *argv[])

{
	int iFail = 0;

	const char* uid = "infodba";
	const char* group = "dba";
	const char* pass1 = "infodba";

	iFail = ITK_init_module(uid, pass1, group);

	if (iFail == ITK_ok)
	{
		std::cout << "Login successful \n";
		rough();
	}
	else
	{
		ITK_CALL(iFail);
	}

	return iFail;
}