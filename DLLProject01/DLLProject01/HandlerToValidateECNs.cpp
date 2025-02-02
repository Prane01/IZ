#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<epm/epm.h>
#include<epm/epm_task_template_itk.h>
#include<tccore/aom_prop.h>
#include<set>
#include<tc/tc_startup.h>
#include<fstream>
#include<tccore/aom.h>
#define DLLAPI _declspec(dllexport)
using namespace std;

extern "C" {
	int ifail = 0;
	char* cError = NULL;
	int status = 0;
	ofstream writeToFile;
	string filePath = "D:\\Teamcenter14\\TC_ROOT\\codeLogFile.txt";

	// Declaration of function
	void ITK_CALL(int ifail);
	void checkNullTag(tag_t tag);
	void approve_implements_objects(int iStatusCheck, tag_t tImplementsObjects);
	void cancel_implements_objects(int iStatusCheck, tag_t tImplementsObjects);
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int HON_ECN_Approve_ECR_on_Validation(EPM_action_message_t msg);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);

	//Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		EPM_register_action_handler("HON-ECN-Approve-ECR-on-Validation", "Approve ECR on ECN's Validation", (EPM_action_handler_t)HON_ECN_Approve_ECR_on_Validation);
		return status;
	}

	extern DLLAPI int HON_ECN_Approve_ECR_on_Validation(EPM_action_message_t msg)
	{
		writeToFile.open(filePath);
		if (!writeToFile.is_open()) {
			writeToFile << "File Is not opened" << endl;
		}
		writeToFile << "\n\n******NOTE: Entering HON_ECN_Approve_ECR_on_Validation Handler******\n";
		//variables
		int iFail = ITK_ok;
		int iAttachmentType = EPM_target_attachment;
		int iAttachments = 0;
		int iImplementedByObjects = 0;
		int iApproveCheck = 0;
		int iCancelCheck = 0;
		int iFoundECN = 0;
		int iImplementsObjects = 0;
		int iSizeOfSet = 0;

		char* cpECNObjType = NULL;
		char* cpClosure = NULL;
		char* cpDisposition = NULL;
		char* cpMaturity = NULL;

		tag_t tJob = NULLTAG;
		tag_t tRootTask = NULLTAG;

		tag_t* tAttachments = NULL;
		tag_t* tImplementsObjects = NULLTAG;
		tag_t* tImplementedByObjects = NULL;

		std::set<tag_t> tSetOfECNs;
		try
		{
			ITK_CALL(iFail = EPM_ask_job(msg.task, &tJob));
			writeToFile << "\nJob is " << tJob;
			ITK_CALL(iFail = EPM_ask_root_task(msg.task, &tRootTask));
			writeToFile << "\ntRootTask is " << tRootTask;
			ITK_CALL(iFail = EPM_ask_attachments(tRootTask, iAttachmentType, &iAttachments, &tAttachments));
			writeToFile << "\niAttachments is " << iAttachments;

			if (iAttachments == 0)
			{
				writeToFile << "\nAttchments founds are Zero.\n";
				return 0;
			}
			for (int iLoop = 0; iLoop < iAttachments; iLoop++)
			{
				ITK_CALL(iFail = WSOM_ask_object_type2(tAttachments[iLoop], &cpECNObjType));
				writeToFile << "\ncpECNObjType :" << cpECNObjType << endl;
				if (tc_strcmp(cpECNObjType, "A2Change_notiRevision") == 0)
				{
					iFoundECN += 1;
					ITK_CALL(iFail = AOM_ask_value_tags(tAttachments[iLoop], "CMImplements", &iImplementsObjects, &tImplementsObjects));
					writeToFile << "\niImplementsObjects :" << iImplementsObjects << endl;
					for (int cLoop = 0; cLoop < iImplementsObjects; cLoop++)
					{
						ITK_CALL(iFail = AOM_ask_value_tags(tImplementsObjects[cLoop], "CMImplementedBy", &iImplementedByObjects, &tImplementedByObjects));
						writeToFile << "\n in cLoop :" << cLoop << endl;
						for (int aLoop = 0; aLoop < iImplementedByObjects; aLoop++)
						{
							writeToFile << "\n in aLoop :" << aLoop << endl;
							auto result = tSetOfECNs.insert(tImplementedByObjects[aLoop]);
							if (result.second)
							{
								writeToFile << "\n result.second :" << result.second << endl;
								writeToFile << "\n result.first :" << *result.first << endl;
								ITK_CALL(iFail = AOM_ask_value_string(*result.first, "CMClosure", &cpClosure));
								ITK_CALL(iFail = AOM_ask_value_string(*result.first, "CMDisposition", &cpDisposition));
								ITK_CALL(iFail = AOM_ask_value_string(*result.first, "CMMaturity", &cpMaturity));

								if (tc_strcmp(cpClosure, "Closed") == 0 && tc_strcmp(cpDisposition, "Approved") == 0 && tc_strcmp(cpMaturity, "Complete") == 0)
								{
									iApproveCheck++;
									writeToFile << "\n iApproveCheck :" << iApproveCheck << endl;
								}
								if (tc_strcmp(cpClosure, "Canceled") == 0 && tc_strcmp(cpDisposition, "Disapproved") == 0 && tc_strcmp(cpMaturity, "Complete") == 0)
								{
									iCancelCheck++;
									writeToFile << "\n iCancelCheck :" << iCancelCheck << endl;
								}
							}
						}
					}
					iSizeOfSet = tSetOfECNs.size();
					writeToFile << "\n in iSizeOfSet :" << iSizeOfSet << endl;
					for (int dLoop = 0; dLoop < iImplementsObjects; dLoop++)
					{
						writeToFile << "\n in dLoop :" << dLoop << endl;
						if (iApproveCheck == iSizeOfSet)
						{
							approve_implements_objects(iApproveCheck, tImplementsObjects[dLoop]);
						}
						else if (iCancelCheck == iSizeOfSet)
						{
							cancel_implements_objects(iCancelCheck, tImplementsObjects[dLoop]);
						}
						else
						{
							writeToFile << "\nIn Else block\n";
							// Write code for Some are Approved and some Rejected/Canceled
						}
					}
				}
				if (iFoundECN != 0)
				{
					break;
				}
			}
		}
		catch (...)
		{
			if (iFail == ITK_ok)
			{
				writeToFile << "\nHON_ECN_Approve_ECR_on_Validation: Unhandled Exception.\n";
			}
		}
		MEM_free(tAttachments);
		MEM_free(tImplementedByObjects);
		//writeToFile << ("\n\n******NOTE: Exit from HON_ECN_Approve_ECR_on_Validation Handler******\n");
		writeToFile.close();
		return iFail;
	}
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "***** Teamcenter Logout Success *****\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		return status;
	}

	void approve_implements_objects(int iStatusCheck, tag_t tImplementsObjects) {
		int iFail = ITK_ok;
		int iAttachmentType = EPM_target_attachment;

		char* cImplementsObject = NULL;

		tag_t tProcessTemplate = NULLTAG;
		tag_t tNewProcess = NULLTAG;

		ITK_CALL(iFail = WSOM_ask_object_type2(tImplementsObjects, &cImplementsObject));
		if (tc_strcmp(cImplementsObject, "A2Change_reqRevision") == 0)
		{
			ITK_CALL(iFail = EPM_find_process_template("Approved", &tProcessTemplate));
			writeToFile << "\nProcessTemplate is " << tProcessTemplate << endl;
			ITK_CALL(iFail = EPM_create_process("Approved", NULL, tProcessTemplate, 1, &tImplementsObjects, &iAttachmentType, &tNewProcess));
			writeToFile << "\ntNewProcess is " << tNewProcess << endl;
			writeToFile << "\ntImplementsObjects Approved\n";
		}
		else {
			ITK_CALL(iFail = EPM_find_process_template("Praneeth Release Status", &tProcessTemplate));
			writeToFile << "\nProcessTemplate is " << tProcessTemplate << endl;
			ITK_CALL(iFail = EPM_create_process("Praneeth Release Status", NULL, tProcessTemplate, 1, &tImplementsObjects, &iAttachmentType, &tNewProcess));
			writeToFile << "\ntNewProcess is " << tNewProcess << endl;
			writeToFile << "\ntImplementsObjects Praneeth Release Status\n";
		}
	}

	void cancel_implements_objects(int iStatusCheck, tag_t tImplementsObjects) {
		int iFail = ITK_ok;
		int iAttachmentType = EPM_target_attachment;

		char* cImplementsObject = NULL;

		tag_t tProcessTemplate = NULLTAG;
		tag_t tNewProcess = NULLTAG;


		ITK_CALL(iFail = WSOM_ask_object_type2(tImplementsObjects, &cImplementsObject));
		if (tc_strcmp(cImplementsObject, "A2Change_reqRevision") == 0)
		{
			ITK_CALL(iFail = EPM_find_process_template("Rejected", &tProcessTemplate));
			writeToFile << "\nProcessTemplate is " << tProcessTemplate << endl;
			ITK_CALL(iFail = EPM_create_process("Rejected", NULL, tProcessTemplate, 1, &tImplementsObjects, &iAttachmentType, &tNewProcess));
			writeToFile << "\ntNewProcess is " << tNewProcess << endl;
			writeToFile << "\ntImplementsObjects Rejected\n";
		}
		else {
			ITK_CALL(iFail = EPM_find_process_template("Rejected", &tProcessTemplate));
			writeToFile << "\nProcessTemplate is " << tProcessTemplate << endl;
			ITK_CALL(iFail = EPM_create_process("Rejected", NULL, tProcessTemplate, 1, &tImplementsObjects, &iAttachmentType, &tNewProcess));
			writeToFile << "\ntNewProcess is " << tNewProcess << endl;
			writeToFile << "\ntImplementsObjects Rejected\n";
		}
	}

	void checkNullTag(tag_t tag)
	{
		if (tag == NULLTAG)
		{
			EMH_ask_error_text(ifail, &cError);
			writeToFile << "The Error is: " << cError;
		}
	}

	void ITK_CALL(int ifail)
	{
		if (ifail != ITK_ok)
		{
			EMH_ask_error_text(ifail, &cError);
			writeToFile << "Error: " << cError;
		}
	}
}