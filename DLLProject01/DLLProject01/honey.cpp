#include <iostream>
#include <tc/emh.h>
#include<vector>
#include<string>
#include <cstring>
#include<map>
#include<set>
#include<stdio.h>
#include<tccore/project.h>
//#include<unistd.h>
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

//#include <curl/curl.h>

#include <base_utils/TcResultStatus.hxx>

#include <base_utils/ScopedPtr.hxx>

#include <base_utils/ScopedSmPtr.hxx>

#include <base_utils/IFail.hxx>




//#include "H4_Common_Utils.hxx"



//Newly added
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
#include<tc/tc_macros.h>
#include<epm/signoff.h>
#include<mrocore/mrocore.h>
using namespace std;

std::set<tag_t> sListOfUsers;

#define CUSTOM_EXIT1 (EMH_USER_error_base +8)
#define DLLAPI _declspec(dllexport)
int iFail = 0;
METHOD_id_t method_id;
ofstream outFile;

extern "C"
{

	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int entry_custom1(int *decision, va_list argv);
	extern DLLAPI int exit_custom2(int *decision, va_list argv);
	extern DLLAPI int Action_handler(EPM_action_message_t msg);


	extern DLLAPI int DLLProject01_register_callbacks()
	{

		CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)entry_custom1);
		CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)exit_custom2);
		return iFail;

	}

	extern DLLAPI int entry_custom1(int *decision, va_list argv)
	{
		*decision = ALL_CUSTOMIZATIONS;
		outFile.open("C:\\Users\\13351\\source\\repos\\outputLog.txt");
		if (outFile.is_open()) { // if the file open
			outFile << " * * * * * * * * * * * * * * * * * * * * \n";
			outFile << " * * * Teamcenter Login Success * * * \n";
			outFile << " * * * * * * * * * * * * * * * * * * * * \n\n";
			ITKCALL(EPM_register_action_handler("HON-Notify", "Custom Handler to add reviewers HON-Notify", (EPM_action_handler_t)Action_handler));
			return iFail;
		}
		else {
			cerr << "Unable to open file outputLog.txt";
			return 1; // Return an error code
		}
		return iFail;
	}
	int get_SecondrayList(const char * 	relation_type_name, tag_t tforAttachments)

	{

		int iFail = ITK_ok;

		ResultStatus status(0);

		try {

			outFile << "\n**Under Afftected Item **\n" << endl;

			outFile << "\n**Start Secondary Object Project **\n" << endl;

			//Get Solution Item

			tag_t SolutionRelationType = NULLTAG;

			status = GRM_find_relation_type(relation_type_name, &SolutionRelationType);

			outFile << "Got the tag of Afftected Item Type \n" << endl;

			int iSecCount = 0;

			//scoped_smptr<tag_t> tpSecondaryList;

			tag_t* tpSecondaryList = NULLTAG;

			status = GRM_list_secondary_objects_only(tforAttachments, SolutionRelationType, &iSecCount, &tpSecondaryList);

			outFile << "Got the list of Problem objects \n" << endl;

			if (iSecCount == 0) {

				outFile << "\n** ProblemItem Not Attached**\n" << endl;

			}

			else {
				for (int k = 0; k < iSecCount; k++) {

					//scoped_smptr<char> cpSecObjType;

					char* cpSecObjType = NULL;

					status = WSOM_ask_object_type2(tpSecondaryList[k], &cpSecObjType); 			 //Get the Secondary Object Type

					outFile << "Problem Item :%s \n" << cpSecObjType;

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

						outFile << "\n Project member count : %d" << Solutionmember_count;

					}



					int affectedItemListCount = 0;

					//scoped_smptr<tag_t> tECNProjectList;

					tag_t* taffectedItemProjectList = NULLTAG;



					ITKCALL(iFail = AOM_ask_value_tags(tforAttachments, "project_list", &affectedItemListCount, &taffectedItemProjectList));

					outFile << "\n affectedItemListCount is : %d" << affectedItemListCount;





					if (affectedItemListCount == 0)//Afftected Item Project list

					{

						outFile << "\n** No Projects Assigned for this ECN **\n" << endl;

					}

					else {

						outFile << "\n**ECN have been Assigned for Multiple Projects**\n" << endl;

						//scoped_smptr<char> cpProjectName;

						char* cpProjectName = NULL;

						for (int k = 0; k < affectedItemListCount; k++) {



							ITKCALL(iFail = PROJ_ask_name2(taffectedItemProjectList[k], &cpProjectName));

							//cout("\n\nProject Name: %s",cpProjectName.getString());

							int member_count = 0;

							tag_t* tMembers = NULLTAG;

							int administrator_count = 0;

							tag_t* tAdministrators = NULLTAG;

							int priviledged_user_count = 0;

							tag_t* tPriviledged_users = NULLTAG;

							ITKCALL(iFail = PROJ_ask_team(taffectedItemProjectList[k], &member_count, &tMembers, &administrator_count, &tAdministrators, &priviledged_user_count, &tPriviledged_users));

							outFile << "\n Project member count : %d" << member_count;

							char* sMembers = NULL;

							for (int j = 0; j < member_count; j++) {

								ITKCALL(iFail = AOM_ask_value_string(tMembers[j], "object_name", &sMembers));

								outFile << "\n Project member names : %s" << sMembers;

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

				outFile << "HON-Notify: Unhandled Exception.\n" << endl;

				EMH_store_error(EMH_severity_error, CUSTOM_EXIT1);

				// MEMFREE(targetType);

			}

		}
		return ITK_ok;
	}
}
DLLAPI int  Action_handler(EPM_action_message_t msg)

{
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



	//char* pch=NULL;







	//int nMembers = 0;



	char* sMemberUser = NULL;

	//	tag_t tMemberUser = NULLTAG;



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

		outFile << "\n\n******* NOTE: Entering HON-Notify *******\n" << endl;



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
			const char* constvalue = cpArgValues[iCount];
			if (tc_strcmp(cpArgKeys[iCount], "member") == 0)

			{

				if (cpArgValues[iCount] != NULL && tc_strcmp(cpArgValues[iCount], "") != 0)

				{



					const char* actSeparator = ",";
					char *pointer = const_cast<char*>(actSeparator);
					ITKCALL(iFail = EPM__parse_string(constvalue, pointer, &piStringCount, &pppStringList));


					outFile << "\n piStringCount : %d" << piStringCount;


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

						outFile << "\n towningMembers object_names : %s" << sowningMembers;

						ITKCALL(iFail = AOM_ask_value_string(towningMembers[j], "user_name", &UserName));

						outFile << "\n UserName user_names : %s" << UserName;

						outFile << "\n Project member names : %s" << sowningMembers;

						sListOfUsers.insert(towningMembers[j]);

					}

				}



				int ECNProjectListCount = 0;

				//scoped_smptr<tag_t> tECNProjectList;

				tag_t* tECNProjectList = NULLTAG;



				ITKCALL(iFail = AOM_ask_value_tags(tAttachments[i], "project_list", &ECNProjectListCount, &tECNProjectList));

				outFile << "\n ECNProjectListCount is : %d" << ECNProjectListCount;





				if (ECNProjectListCount == 0)//ECN Project list

				{

					outFile << "\n**No Projects Assigned for this ECN**\n" << endl;

				}

				else {

					outFile << "\n**ECN have been Assigned for Multiple Projects**\n" << endl;

					//scoped_smptr<char> cpProjectName;

					char* cpProjectName = NULL;

					for (int k = 0; k < ECNProjectListCount; k++) {



						ITKCALL(iFail = PROJ_ask_name2(tECNProjectList[k], &cpProjectName));

						outFile << "\n\nProject Name: %s" << cpProjectName;

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

							outFile << "\nsMembers member names : %s" << sMembers;

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

				outFile << "\n stesMembers names : %s" << stesMembers;

				for (int i = 0; i < piStringCount; i++)
				{

					const char* actSeparator = "/";
					char** group_value = NULL;
					int count = 0;
					tag_t tgroup = NULLTAG;
					int number = 0;
					tag_t *tmem = NULLTAG;
					char *grpmem_val = NULL;
					char *userName = NULL;
					char *pointer = const_cast<char*>(actSeparator);
					ITKCALL(iFail = EPM__parse_string(pppStringList[i], pointer, &count, &group_value));
					for (int x = 0; x < count; x++) {
						ITKCALL(SA_find_group(group_value[x], &tgroup));
						ITKCALL(SA_find_groupmembers_by_group(tgroup, &number, &tmem));
						for (int t = 0; t < number; t++) {
							ITKCALL(AOM_ask_value_string(tmem[t], "object_string", &grpmem_val));
							outFile << "\n stesMembers : %s" << stesMembers;
							outFile << "\n grpmem_val : %s" << grpmem_val;
							//cout << "the grp member value" << grpmem_val << endl;
							if (strstr(stesMembers, grpmem_val) != NULL) {
								ITKCALL(AOM_ask_value_string(tmem[t], "user_name", &userName));
								outFile << "\n userName : %s" << userName;
								if (strstr(stesMembers, userName) != NULL)
								{
									if (sCheckListOfUsers.size() > 0) {
										if (sCheckListOfUsers.find(stesMembers) != sCheckListOfUsers.end()) {

											outFile << "\n Already assigned : %s" << grpmem_val;
										}

										else {

											outFile << "\n Assigned User found : %s" << grpmem_val;

											ITKCALL(EPM_create_adhoc_signoff(tcurrentTask, *sMembersList, &nSignoffs, &tpSignoffs));

											outFile << "\nThe number of signoffs created: %d" << nSignoffs;

											ITKCALL(EPM_set_adhoc_signoff_selection_done(tcurrentTask, TRUE));

											outFile << "\n\n-------The Task has been successfully assigned-------\n\n" << endl;

											sCheckListOfUsers.insert(stesMembers);


										}

									}

									else {

										outFile << "\n Assigned User found : %s" << grpmem_val;

										ITKCALL(EPM_create_adhoc_signoff(tcurrentTask, *sMembersList, &nSignoffs, &tpSignoffs));

										outFile << "\nThe number of signoffs created: %d" << nSignoffs;

										ITKCALL(EPM_set_adhoc_signoff_selection_done(tcurrentTask, TRUE));

										outFile << "\n\n-------The Task has been successfully assigned-------\n\n" << endl;

										sCheckListOfUsers.insert(stesMembers);

									}
								}
							}
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

			outFile << "HON_Notify: Unhandled Exception.\n" << endl;

			EMH_store_error(EMH_severity_error, CUSTOM_EXIT1);

			// MEMFREE(targetType);

		}

	}

	(tAttachments);

	//MEMFREE(attachmentTypeName);

	MEM_free(sProjectId);

	MEM_free(sMemberUser);

	//MEMFREE(sMembers);

	//MEMFREE(tMembers);

	MEM_free(cprocessUID);

	MEM_free(jobName);

	MEM_free(current_task_name);

	MEM_free(cprocessUID1);

	MEM_free(property);





	outFile << "\n\n******* NOTE: Exit from HON-Notify *******\n" << endl;

	return iFail;

}

extern DLLAPI int exit_custom2(int *decision, va_list argv)
{
	*decision = ALL_CUSTOMIZATIONS;

	outFile << "\n *** closed successfully ***";
	return iFail;

}





