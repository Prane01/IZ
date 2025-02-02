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

EPM_action_message_t msg;
int  ITK_user_main(int argc, char *argv[])
{
	
	//variables
	int iFail = ITK_ok;
	int iAttachmentType = EPM_target_attachment;
	int nAttachments = 0;
	int i = 0;

	char* includeTypes = NULL;

	tag_t tJob = NULLTAG;
	tag_t tRootTask = NULLTAG;
	tag_t* tAttachments = NULL;
	tag_t tPrevRev = NULLTAG;

	string attachmentUIDs;

	std::map<std::string, std::string> argsMap;
	std::vector<std::string> argList;

	argList.push_back("include_type");

	try
	{

		TC_write_syslog("\n\n******NOTE: Entering HON_link_Design_and_part_on_release******\n");

		//ITK_CALL(iFail = getActionHandlerArgs(msg, argsMap, argList));
		TC_write_syslog(" argList size is %d \n", argList.size());

		if (argList.size() > 0)
		{
			if (argsMap.find("include_type") != argsMap.end() && argsMap["include_type"].c_str() != NULL && tc_strcmp(argsMap["include_type"].c_str(), "") != 0)
			{
				includeTypes = (char *)MEM_alloc(256 * sizeof(char));
				tc_strcpy(includeTypes, argsMap["include_type"].c_str());
			}
		}


		string includeTypeStr(includeTypes);

		TC_write_syslog(" includeTypes are %s \n", includeTypeStr.c_str());
		TC_write_syslog(" current task is %d \n", msg.task);

		ITK_CALL(iFail = EPM_ask_job(msg.task, &tJob));
		TC_write_syslog(" Job is %d \n", tJob);
		ITK_CALL(iFail = EPM_ask_root_task(tJob, &tRootTask));
		TC_write_syslog(" Root task is %d \n", tRootTask);
		ITK_CALL(iFail = EPM_ask_attachments(tRootTask, iAttachmentType, &nAttachments, &tAttachments));
		TC_write_syslog(" Target Attachment count is %d \n", nAttachments);

		if (nAttachments > 0)
		{
			for (i = 0; i < nAttachments; i++)
			{
				tPrevRev = NULLTAG;
				char *uid = NULL;
				char* object_string = NULL;

				ITK_CALL(iFail = AOM_ask_value_string(tAttachments[i], "object_string", &object_string));
				TC_write_syslog("\n object string : %s\n", object_string);

				ITK__convert_tag_to_uid(tAttachments[i], &uid);
				TC_write_syslog("\n The UID is : %s", uid);

				std::string escaped_uid = uid;
				size_t pos = 0;
				while ((pos = escaped_uid.find('$', pos)) != std::string::npos)
				{
					escaped_uid.insert(pos, "\\"); pos += 2;
				}

				attachmentUIDs += escaped_uid;
				attachmentUIDs += '~';

				HON_free_memory(object_string);
				HON_free_memory(uid);

			}

			TC_write_syslog("\n\n attachment UId:  %s", attachmentUIDs.c_str());
			TC_write_syslog("\n\n include type :  %s", includeTypeStr.c_str());

			TC_write_syslog("\n\n*************** Before calling Utility ***************** %d", iFail);

			//string passwordFile =
			//string sUtilityCmd = "cd /webvol/Apps/SPLM/TcRoot13/bin/; HON_Link_Design_and_Part_Utility -u=infodba -g=dba -pf=\"" + passwordFile + "\" -targetObjUID=\"" + attachmentUIDs + "\" -includeType=\"" + includeTypeStr + "\"";

			string sUtilityCmd = "/Teamcenter/Apps/SPLM/TcRoot13/bin/Hon-link.sh \"" + attachmentUIDs + "\" \"" + includeTypeStr + "\"";

			TC_write_syslog("\n*************** The command is : %s", sUtilityCmd.c_str());
			int p = system(sUtilityCmd.c_str());
			TC_write_syslog("\nThe system command return value is :   %d\n", p);
			TC_write_syslog("\n*************** After Comming back from Utility  QA 29thAug24  ***************  %d\n", iFail);
		}

	}
	catch (...)
	{
		if (iFail == ITK_ok)
		{
			TC_write_syslog("HON_link_Design_and_part_on_release: Unhandled Exception.\n");
		//	iFail = HON_GENERIC_ERROR;
		}
	}

	HON_free_memory(tAttachments);
	HON_free_memory(includeTypes);

	TC_write_syslog("\n\n******NOTE: Exit From HON_link_Design_and_part_on_release handler******\n");

	return iFail;
}
