#include <tc/emh.h>
#include<vector>
#include<string>
#include <iostream>
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
		outFile.open("C:\\Users\\13351\\source\\repos\\outputLog2.txt");
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

			outFile << "\n**Start Secondary Ojbect Project **\n" << endl;

			//Get Solution Item

			tag_t SolutionRelationType = NULLTAG;

			status = GRM_find_relation_type(relation_type_name, &SolutionRelationType);

			outFile << "Got the tag of Afftected Item Type \n" << endl;

			int iSecCount = 0;

			//scoped_smptr<tag_t> tpSecondaryList;

			tag_t* tpSecondaryList = NULLTAG;

			status = GRM_list_secondary_objects_only(tforAttachments, SolutionRelationType, &iSecCount, &tpSecondaryList);

			outFile << "Got the list of Solution objects \n" << endl;

			if (iSecCount == 0) {

				outFile << "\n**SolutionItem Not Attachede**\n" << endl;



			}

			else {



				for (int k = 0; k < iSecCount; k++) {

					//scoped_smptr<char> cpSecObjType;

					char* cpSecObjType = NULL;

					status = WSOM_ask_object_type2(tpSecondaryList[k], &cpSecObjType); 			 //Get the Secondary Object Type

					//cout("Solution Item  : \n",k,cpSecObjType.getString());

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

						outFile << "\n Project member count : "<< Solutionmember_count;

					}



					int affectedItemListCount = 0;

					//scoped_smptr<tag_t> tECNProjectList;

					tag_t* taffectedItemProjectList = NULLTAG;



					ITKCALL(iFail = AOM_ask_value_tags(tforAttachments, "project_list", &affectedItemListCount, &taffectedItemProjectList));

					outFile << "\n Project Count is : "<< affectedItemListCount;





					if (affectedItemListCount == 0)//Afftected Item Project list

					{

						outFile << "\n**No Projects Assigned for this ECN**\n" << endl;

					}

					else {

						outFile << "\n**ECN have been Assigned for Multiple Projects**\n" << endl;

						//scoped_smptr<char> cpProjectName;

						char* cpProjectName = NULL;

						for (int k = 0; k < affectedItemListCount; k++) {



							ITKCALL(iFail = PROJ_ask_name2(taffectedItemProjectList[k], &cpProjectName));

							//cout("\n\nProject Name: ",cpProjectName.getString());

							int member_count = 0;

							tag_t* tMembers = NULLTAG;

							int administrator_count = 0;

							tag_t* tAdministrators = NULLTAG;

							int priviledged_user_count = 0;

							tag_t* tPriviledged_users = NULLTAG;

							ITKCALL(iFail = PROJ_ask_team(taffectedItemProjectList[k], &member_count, &tMembers, &administrator_count, &tAdministrators, &priviledged_user_count, &tPriviledged_users));

							outFile << "\n Project member count : "<< member_count;

							char* sMembers = NULL;

							for (int j = 0; j < member_count; j++) {

								ITKCALL(iFail = AOM_ask_value_string(tMembers[j], "object_name", &sMembers));

								outFile << "\n Project member names : "<< sMembers;

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
				outFile << "HON_Notify_CFT: Unhandled Exception.\n" << endl;

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

			outFile << "\n\n******* NOTE: Entering HON_Notify_CFT *******\n" << endl;



			ITKCALL(iFail = EPM_ask_job(msg.task, &tJob));

			outFile << "Job is  \n"<< tJob;

			ITKCALL(iFail = EPM_ask_root_task(tJob, &tRootTask));

			outFile << "Root task is  \n"<< tRootTask;

			ITKCALL(iFail = AOM_ask_name(tJob, &jobName));

			outFile << "Job Name is  \n"<< jobName;



			tcurrentTask = msg.task;

			outFile << "Task tag where the Handler Resides: "<< tcurrentTask;



			ITKCALL(iFail = AOM_ask_name(msg.task, &current_task_name));

			outFile << "Current task is  \n"<< current_task_name;









			ITKCALL(iFail = EPM_ask_attachments(tRootTask, EPM_target_attachment, &nAttachments, &tAttachments));

			outFile << " Target Attachment count is  \n"<< nAttachments;



			ITKCALL(EPM_args_process_args(tcurrentTask, msg.arguments, &iArgCount, &cpArgKeys, &cpArgValues));

			outFile << "\nNumber of Arguments Provided: \n"<< iArgCount;

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
						/*outFile << "the resource values :" << pppStringList << endl;*/




					}

				}

			}








		for (int i = 0; i < nAttachments; i++)

		{

			char* attachmentTypeName = NULL;



			ITKCALL(iFail = WSOM_ask_object_type2(tAttachments[i], &attachmentTypeName));

			outFile << "\n Object type : "<< attachmentTypeName;



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

					outFile << "\n Project member count : "<< owningmember_count;

					char* sowningMembers = NULL;

					char* UserName = NULL;

					for (int j = 0; j < owningmember_count; j++)

					{



						ITKCALL(iFail = AOM_ask_value_string(towningMembers[j], "object_name", &sowningMembers));

						ITKCALL(iFail = AOM_ask_value_string(towningMembers[j], "user_name", &UserName));

						outFile << "\n Project member names : "<< sowningMembers;

						sListOfUsers.insert(towningMembers[j]);





					}

				}



				int ECNProjectListCount = 0;

				//scoped_smptr<tag_t> tECNProjectList;

				tag_t* tECNProjectList = NULLTAG;



				ITKCALL(iFail = AOM_ask_value_tags(tAttachments[i], "project_list", &ECNProjectListCount, &tECNProjectList));

				outFile << "\n Project Count is : "<< ECNProjectListCount;





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

						//cout("\n\nProject Name: ",cpProjectName.getString());

						int member_count = 0;

						tag_t* tMembers = NULLTAG;

						int administrator_count = 0;

						tag_t* tAdministrators = NULLTAG;

						int priviledged_user_count = 0;

						tag_t* tPriviledged_users = NULLTAG;

						ITKCALL(iFail = PROJ_ask_team(tECNProjectList[k], &member_count, &tMembers, &administrator_count, &tAdministrators, &priviledged_user_count, &tPriviledged_users));

						outFile << "\n Project member count : "<< member_count;

						char* sMembers = NULL;

						for (int j = 0; j < member_count; j++) {

							ITKCALL(iFail = AOM_ask_value_string(tMembers[j], "object_name", &sMembers));

							outFile << "\n Project member names : "<< sMembers;

							sListOfUsers.insert(tMembers[j]);
						}
					}
				}
				iFail = get_SecondrayList("CMHasSolutionItem", tAttachments[i]);

				iFail = get_SecondrayList("CMHasProblemItem", tAttachments[i]);

				iFail = get_SecondrayList("CMHasImpactedItem", tAttachments[i]);
			}

			SAFE_SM_FREE(attachmentTypeName);

		}

		if (sListOfUsers.size() > 0) {


			


			// tag_t sMembersList = NULLTAG;

			char* stesMembers = NULL;

			char* cpInboxUser = NULL;
			//if (tc_strcmp(stesMembers, cpInboxUser) == 0) {}
			//  sMembersList=sListOfUsers.

			std::set<tag_t>::iterator sMembersList;

			std::set<char*> sOutofOfficeUsers;

			std::set<char*> sCheckListOfUsers;

			tag_t tUserCHeck = NULLTAG;

			logical cpUserIPclearance;

			for (sMembersList = sListOfUsers.begin(); sMembersList != sListOfUsers.end(); sMembersList++)

			{

             ITKCALL(iFail = AOM_ask_value_string(*sMembersList, "object_name", &stesMembers));
			 ITKCALL(iFail = AOM_ask_value_tag(*sMembersList, "user", &tUserCHeck));
			 ITKCALL(iFail = AOM_ask_value_logical(tUserCHeck, "is_out_of_office", &cpUserIPclearance));
				
			 outFile << "\n Project member names : "<< stesMembers;
			 outFile << "\n cpUserIPclearance : " << cpUserIPclearance;
			 if (cpUserIPclearance != 1)
			 {

				 for (int i = 0; i < piStringCount; i++)
				 {
					 outFile << "\nthe pppStringList[i] value : " << pppStringList[i] << endl;
					 if (strstr(stesMembers, pppStringList[i]) != NULL) {

						 if (sCheckListOfUsers.size() > 0) {
							 if (sCheckListOfUsers.find(stesMembers) != sCheckListOfUsers.end()) {
								 outFile << "\n Already assigned : " << stesMembers;
							 }
							 else {
								 outFile << "\n In Else block";
								 outFile << "\n User found to be Assigned : " << stesMembers;

								 ITKCALL(EPM_create_adhoc_signoff(tcurrentTask, *sMembersList, &nSignoffs, &tpSignoffs));

								 outFile << "\nThe number of signoffs created: " << nSignoffs;

								 ITKCALL(EPM_set_adhoc_signoff_selection_done(tcurrentTask, TRUE));

								 outFile << "\n\n-------The Task has been successfully assigned-------\n\n" << endl;

								 sCheckListOfUsers.insert(stesMembers);
							 }

						 }

						 else {

							 outFile << "\n User found to be Assigned : " << stesMembers;

							 ITKCALL(EPM_create_adhoc_signoff(tcurrentTask, *sMembersList, &nSignoffs, &tpSignoffs));

							 outFile << "\nThe number of signoffs created: " << nSignoffs;

							 ITKCALL(EPM_set_adhoc_signoff_selection_done(tcurrentTask, TRUE));

							 outFile << "\n\n-------The Task has been successfully assigned-------\n\n" << endl;

							 sCheckListOfUsers.insert(stesMembers);

							 //std::outFile << sMembersList << ' ';

						 }
					 }


					 //if (strstr(stesMembers, pppStringList[i]) != NULL) {

					 //	if (sCheckListOfUsers.size() > 0) {



					 //		if (sCheckListOfUsers.find(stesMembers) != sCheckListOfUsers.end()) {

					 //			outFile << "\n Already assing : ", pppStringList[i]);

					 //		}

					 //		else {

					 //			outFile << "\n Assigned User found : ", pppStringList[i]);

					 //			ITKCALL(EPM_create_adhoc_signoff(tcurrentTask, *sMembersList, &nSignoffs, &tpSignoffs));

					 //			outFile << "\nThe number of signoffs created: ", nSignoffs);

					 //			ITKCALL(EPM_set_adhoc_signoff_selection_done(tcurrentTask, TRUE));

					 //			outFile << "\n\n-------The Task has been successfully assigned-------\n\n" << endl;

					 //			sCheckListOfUsers.insert(stesMembers);

					 //			//std::outFile << sMembersList << ' ';

					 //		}

					 //	}

					 //	else {

					 //		outFile << "\n Assigned User found : ", pppStringList[i]);

					 //		ITKCALL(EPM_create_adhoc_signoff(tcurrentTask, *sMembersList, &nSignoffs, &tpSignoffs));

					 //		outFile << "\nThe number of signoffs created: ", nSignoffs);

					 //		ITKCALL(EPM_set_adhoc_signoff_selection_done(tcurrentTask, TRUE));

					 //		outFile << "\n\n-------The Task has been successfully assigned-------\n\n" << endl;

					 //		sCheckListOfUsers.insert(stesMembers);

					 //		//std::outFile << sMembersList << ' ';

					 //	}





					 //}



				 }
			 }
			 else {

				 outFile << "\n Out of Office user is : " << stesMembers;

				 tag_t inboxDelegate = NULLTAG;

				 char* cpOOOUserName = NULL;

				 ITKCALL(iFail = AOM_ask_value_string(tUserCHeck, "user_name", &cpOOOUserName));

				 auto result = sOutofOfficeUsers.insert(cpOOOUserName);

				 if (result.second)
				 {
					 outFile << "\n result.second : " << result.second << endl;
					 outFile << "\n result.first : " << *result.first << endl;

					 ITKCALL(iFail = AOM_ask_value_tag(tUserCHeck, "inbox_delegate", &inboxDelegate));

					 outFile << "\n inboxDelegate is : " << inboxDelegate;

					 ITKCALL(iFail = AOM_ask_value_string(inboxDelegate, "object_name", &cpInboxUser));

					 outFile << "\n cpInboxUser is : " << cpInboxUser;

					 //sListOfUsers.insert(inboxDelegate);

					 ITKCALL(EPM_create_adhoc_signoff(tcurrentTask, inboxDelegate, &nSignoffs, &tpSignoffs));

					 outFile << "\nThe number of signoffs created: " << nSignoffs;

					 ITKCALL(EPM_set_adhoc_signoff_selection_done(tcurrentTask, TRUE));

					 outFile << "\n\n-------The Task has been successfully assigned-------\n\n" << endl;

					 //sCheckListOfUsers.insert(cpInboxUser);

				 }
			 }

			}
		}
	}

	catch (...)

	{

		if (iFail == ITK_ok)

		{

			outFile << "HON_Notify_CFT: Unhandled Exception.\n" << endl;

			EMH_store_error(EMH_severity_error, CUSTOM_EXIT1);

			// MEMFREE(targetType);

		}

	}

	(tAttachments);

	//MEMFREE(attachmentTypeName);

	MEMFREE(sProjectId);

	MEMFREE(sMemberUser);

	//MEMFREE(sMembers);

	//MEMFREE(tMembers);

	MEMFREE(cprocessUID);

	MEMFREE(jobName);

	MEMFREE(current_task_name);

	MEMFREE(cprocessUID1);

	MEMFREE(property);





	outFile << "\n\n******* NOTE: Exit from HON_Notify_CFT *******\n" << endl;

	return iFail;

}

extern DLLAPI int exit_custom2(int *decision, va_list argv)
{
	*decision = ALL_CUSTOMIZATIONS;

	outFile << "\nclosed successfully";
	return iFail;

}





