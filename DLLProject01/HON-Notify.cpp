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
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedPtr.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/IFail.hxx>
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
#include<tccore/project.h>
#include<epm/epm_task_template_itk.h>
#include<epm/signoff.h>
#define DLLAPI _declspec(dllexport)
using namespace std;

int status = 0;
char* cError = NULL;
int checkNullTag(tag_t tag);
ofstream outFile;

std::set<tag_t> sListOfUsers;

extern "C" {
	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int HON_Notify_CFT(EPM_action_message_t msg);
	int get_SecondrayList(const char * 	relation_type_name, tag_t tforAttachments);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	//Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		outFile.open("C:\\Users\\13351\\source\\repos\\outputLog.txt");
		if (outFile.is_open()) { // if the file open
			outFile << " * * * * * * * * * * * * * * * * * * * * \n";
			outFile << " * * * Teamcenter Login Success * * * \n";
			outFile << " * * * * * * * * * * * * * * * * * * * * \n\n";
			EPM_register_action_handler("HON-Notify", "HON Notify Users", (EPM_action_handler_t)HON_Notify_CFT);
			return status;
		}
		else {
			cerr << "Unable to open file outputLog.txt";
			return 1; // Return an error code
		}
	}
	extern DLLAPI int  HON_Notify_CFT(EPM_action_message_t msg) {
		int iFail = ITK_ok;
		int nAttachments = 0;

		tag_t tJob = NULLTAG;

		tag_t tRootTask = NULLTAG;



		tag_t* tAttachments = NULL;



		char* sProjectId = NULL;

		tag_t tProjectId = NULLTAG;

		ResultStatus status(0);


		char* jobName = NULL;

		char* current_task_name = NULL;

		char* objectString = NULL;


		char* sMemberUser = NULL;


		int nSignoffs = 0;

		tag_t* tpSignoffs = NULLTAG;

		tag_t tcurrentTask = NULLTAG;

		char *cprocessUID = NULL;

		char *cprocessUID1 = NULL;

		int iArgCount = 0;

		char** cpArgKeys = NULL;

		char** cpArgValues = NULL;

		int iCount = 0;

		char* property = NULL;

		int piStringCount = 0;

		char** pppStringList = NULL;

		try {

			outFile << "\n\n******* NOTE: Entering HON_Notify_CFT *******\n";

			ITKCALL(iFail = EPM_ask_job(msg.task, &tJob));

			outFile << "Job is %d \n" << tJob;

			ITKCALL(iFail = EPM_ask_root_task(tJob, &tRootTask));

			outFile << "Root task is %d \n" << tRootTask;

			ITKCALL(iFail = AOM_ask_name(tJob, &jobName));

			outFile << "Job Name is %s \n" << jobName;

			tcurrentTask = msg.task;

			outFile << "Task tag where the Handler Resides: %d" << tcurrentTask;

			ITKCALL(iFail = AOM_ask_name(msg.task, &current_task_name));

			outFile << "Current task is %s \n" << current_task_name;

			ITKCALL(iFail = EPM_ask_attachments(tRootTask, EPM_target_attachment, &nAttachments, &tAttachments));

			outFile << " Target Attachment count is %d \n" << nAttachments;

			ITKCALL(EPM_args_process_args(tcurrentTask, msg.arguments, &iArgCount, &cpArgKeys, &cpArgValues));

			outFile << "\nNumber of Arguments Provided: %d\n" << iArgCount;

			for (iCount = 0; iCount < iArgCount; iCount++)

			{

				if (tc_strcmp(cpArgKeys[iCount], "member") == 0)

				{

					if (cpArgValues[iCount] != NULL && tc_strcmp(cpArgValues[iCount], "") != 0)

					{
						const char* separator = ",";
						char* actSeparator = const_cast<char*>(separator);
						ITKCALL(iFail = EPM__parse_string(cpArgValues[iCount], actSeparator, &piStringCount, &pppStringList));
					}

				}

			}

			for (int i = 0; i < nAttachments; i++)

			{

				char* attachmentTypeName = NULL;


				ITKCALL(iFail = WSOM_ask_object_type2(tAttachments[i], &attachmentTypeName));

				outFile << "\n Object type : %s" << attachmentTypeName;

				ITKCALL(iFail = AOM_ask_value_string(tAttachments[i], "object_string", &objectString));

				if (tc_strcmp(attachmentTypeName, "ChangeNoticeRevision") == 0)

				{

					ITKCALL(iFail = AOM_ask_value_tag(tAttachments[i], "owning_project", &tProjectId));



					if (tProjectId != NULLTAG)//ECN Project

					{

						int owningmember_count = 0;

						tag_t* towningMembers = NULLTAG;

						int owningadministrator_count = 0;

						tag_t* towningAdministrators = NULLTAG;

						int owningpriviledged_user_count = 0;

						tag_t* towningPriviledged_users = NULLTAG;



						ITKCALL(iFail = PROJ_ask_team(tProjectId, &owningmember_count, &towningMembers, &owningadministrator_count, &towningAdministrators, &owningpriviledged_user_count, &towningPriviledged_users));

						outFile << "\n Project member count : %d" << owningmember_count;

						char* sowningMembers = NULL;

						char* UserName = NULL;

						for (int j = 0; j < owningmember_count; j++)

						{



							ITKCALL(iFail = AOM_ask_value_string(towningMembers[j], "object_name", &sowningMembers));

							ITKCALL(iFail = AOM_ask_value_string(towningMembers[j], "user_name", &UserName));

							outFile << "\n Project member names : %s" << sowningMembers;

							sListOfUsers.insert(towningMembers[j]);





						}

					}



					int ECNProjectListCount = 0;

					//scoped_smptr<tag_t> tECNProjectList;

					tag_t* tECNProjectList = NULLTAG;



					ITKCALL(iFail = AOM_ask_value_tags(tAttachments[i], "project_list", &ECNProjectListCount, &tECNProjectList));

					outFile << "\n Project Count is : %d" << ECNProjectListCount;





					if (ECNProjectListCount == 0)//ECN Project list

					{

						outFile << "\n**No Projects Assigned for this ECN**\n";

					}

					else {

						outFile << "\n**ECN have been Assigned for Multiple Projects**\n";

						//scoped_smptr<char> cpProjectName;

						char* cpProjectName = NULL;

						for (int k = 0; k < ECNProjectListCount; k++) {



							ITKCALL(iFail = PROJ_ask_name2(tECNProjectList[k], &cpProjectName));

							//outFile << "\n\nProject Name: %s",cpProjectName.getString());

							int member_count = 0;

							tag_t* tMembers = NULLTAG;

							int administrator_count = 0;

							tag_t* tAdministrators = NULLTAG;

							int priviledged_user_count = 0;

							tag_t* tPriviledged_users = NULLTAG;

							ITKCALL(iFail = PROJ_ask_team(tECNProjectList[k], &member_count, &tMembers, &administrator_count, &tAdministrators, &priviledged_user_count, &tPriviledged_users));

							outFile << "\n Project member count : %d" << member_count;

							char* sMembers = NULL;

							for (int j = 0; j < member_count; j++) {

								ITKCALL(iFail = AOM_ask_value_string(tMembers[j], "object_name", &sMembers));

								outFile << "\n Project member names : %s" << sMembers;

								sListOfUsers.insert(tMembers[j]);

							}



						}







					}

					iFail = get_SecondrayList("CMHasProblemItem", tAttachments[i]);

				}

				SAFE_SM_FREE(attachmentTypeName);

			}



			if (sListOfUsers.size() > 0) {





				// tag_t sMembersList = NULLTAG;

				char* stesMembers = NULL;

				//  sMembersList=sListOfUsers.

				std::set<tag_t>::iterator sMembersList;

				std::set<char*> sCheckListOfUsers;

				for (sMembersList = sListOfUsers.begin(); sMembersList != sListOfUsers.end(); sMembersList++)

				{





					ITKCALL(iFail = AOM_ask_value_string(*sMembersList, "object_name", &stesMembers));

					outFile << "\n Project member names : %s" << stesMembers;

					for (int i = 0; i < piStringCount; i++) {



						if (strstr(stesMembers, pppStringList[i]) != NULL) {

							if (sCheckListOfUsers.size() > 0) {



								if (sCheckListOfUsers.find(stesMembers) != sCheckListOfUsers.end()) {

									outFile << "\n Already assing : %s" << pppStringList[i];

								}

								else {

									outFile << "\n Assigned User found : %s" << pppStringList[i];

									ITKCALL(EPM_create_adhoc_signoff(tcurrentTask, *sMembersList, &nSignoffs, &tpSignoffs));

									outFile << "\nThe number of signoffs created: %d" << nSignoffs;

									ITKCALL(EPM_set_adhoc_signoff_selection_done(tcurrentTask, TRUE));

									outFile << "\n\n-------The Task has been successfully assigned-------\n\n";

									sCheckListOfUsers.insert(stesMembers);

									//std::cout << sMembersList << ' ';

								}

							}

							else {

								outFile << "\n Assigned User found : %s" << pppStringList[i];

								ITKCALL(EPM_create_adhoc_signoff(tcurrentTask, *sMembersList, &nSignoffs, &tpSignoffs));

								outFile << "\nThe number of signoffs created: %d" << nSignoffs;

								ITKCALL(EPM_set_adhoc_signoff_selection_done(tcurrentTask, TRUE));

								outFile << "\n\n-------The Task has been successfully assigned-------\n\n";

								sCheckListOfUsers.insert(stesMembers);

								//std::cout << sMembersList << ' ';

							}





						}



					}

				}







			}







		}

		catch (...)

		{

			if (iFail == ITK_ok)

			{

				outFile << "HON-notify: Unhandled Exception.\n";

				//iFail = HON_GENERIC_ERROR;

				// HON_free_memory(targetType);

			}

		}

		//HON_free_memory(tAttachments);

		//HON_free_memory(attachmentTypeName);

		//HON_free_memory(sProjectId);

		//HON_free_memory(sMemberUser);

		//HON_free_memory(sMembers);

		//HON_free_memory(tMembers);

		//HON_free_memory(cprocessUID);

		//HON_free_memory(jobName);

		//HON_free_memory(current_task_name);

		//HON_free_memory(cprocessUID1);

		//HON_free_memory(property);





		outFile << "\n\n******* NOTE: Exit from HON_Notify_CFT *******\n";

		return iFail;

	}


	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		outFile << "* * * * * * * * * * * * * * * * * * * * \n\n";
		outFile << "***** Teamcenter Logout Success *****\n\n";
		outFile << "* * * * * * * * * * * * * * * * * * * * \n\n";
		outFile.close();
		return status;
	}

}


int get_SecondrayList(const char * 	relation_type_name, tag_t tforAttachments)

{

	int iFail = ITK_ok;

	ResultStatus status(0);

	try {

		outFile << "\n**Under Afftected Item **\n";

		outFile << "\n**Start Secondary Ojbect Project **\n";

		//Get Solution Item

		tag_t SolutionRelationType = NULLTAG;

		status = GRM_find_relation_type(relation_type_name, &SolutionRelationType);

		outFile << "Got the tag of Afftected Item Type \n";

		int iSecCount = 0;

		//scoped_smptr<tag_t> tpSecondaryList;

		tag_t* tpSecondaryList = NULLTAG;

		status = GRM_list_secondary_objects_only(tforAttachments, SolutionRelationType, &iSecCount, &tpSecondaryList);

		outFile << "Got the list of Solution objects \n";

		if (iSecCount == 0) {

			outFile << "\n**SolutionItem Not Attachede**\n";



		}

		else {



			for (int k = 0; k < iSecCount; k++) {

				//scoped_smptr<char> cpSecObjType;

				char* cpSecObjType = NULL;

				status = WSOM_ask_object_type2(tpSecondaryList[k], &cpSecObjType); 			 //Get the Secondary Object Type

				//outFile << "Solution Item %d :%s \n",k,cpSecObjType.getString());

				tag_t tSolProjectId = NULLTAG;

				ITKCALL(iFail = AOM_ask_value_tag(tpSecondaryList[k], "owning_project", &tSolProjectId));

				if (tSolProjectId != NULLTAG) {

					int Solutionmember_count = 0;

					tag_t* tSolutionMembers = NULLTAG;

					int Solutionadministrator_count = 0;

					tag_t* tSolutionAdministrators = NULLTAG;

					int Solutionpriviledged_user_count = 0;

					tag_t* tSolutionPriviledged_users = NULLTAG;



					ITKCALL(iFail = PROJ_ask_team(tSolProjectId, &Solutionmember_count, &tSolutionMembers, &Solutionadministrator_count, &tSolutionAdministrators, &Solutionpriviledged_user_count, &tSolutionPriviledged_users));

					outFile << "\n Project member count : %d"<< Solutionmember_count;

				}



				int affectedItemListCount = 0;

				//scoped_smptr<tag_t> tECNProjectList;

				tag_t* taffectedItemProjectList = NULLTAG;



				ITKCALL(iFail = AOM_ask_value_tags(tforAttachments, "project_list", &affectedItemListCount, &taffectedItemProjectList));

				outFile << "\n Project Count is : %d"<< affectedItemListCount;





				if (affectedItemListCount == 0)//Afftected Item Project list

				{

					outFile << "\n**No Projects Assigned for this ECN**\n";

				}

				else {

					outFile << "\n**ECN have been Assigned for Multiple Projects**\n";

					//scoped_smptr<char> cpProjectName;

					char* cpProjectName = NULL;

					for (int k = 0; k < affectedItemListCount; k++) {



						ITKCALL(iFail = PROJ_ask_name2(taffectedItemProjectList[k], &cpProjectName));

						//outFile << "\n\nProject Name: %s",cpProjectName.getString());

						int member_count = 0;

						tag_t* tMembers = NULLTAG;

						int administrator_count = 0;

						tag_t* tAdministrators = NULLTAG;

						int priviledged_user_count = 0;

						tag_t* tPriviledged_users = NULLTAG;

						ITKCALL(iFail = PROJ_ask_team(taffectedItemProjectList[k], &member_count, &tMembers, &administrator_count, &tAdministrators, &priviledged_user_count, &tPriviledged_users));

						outFile << "\n Project member count : %d"<< member_count;

						char* sMembers = NULL;

						for (int j = 0; j < member_count; j++) {

							ITKCALL(iFail = AOM_ask_value_string(tMembers[j], "object_name", &sMembers));

							outFile << "\n Project member names : %s"<< sMembers;

							sListOfUsers.insert(tMembers[j]);

						}



					}







				}

			}

		}

	}

	catch (...)

	{

		if (iFail == ITK_ok)

		{

			outFile << "HON_Notify: Unhandled Exception.\n";

			//iFail = HON_GENERIC_ERROR;

			// HON_free_memory(targetType);

		}
	}
	return ITK_ok;
}

