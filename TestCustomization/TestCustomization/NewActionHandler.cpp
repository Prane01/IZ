//#include <H4_PEM_exists/H4_PEM_WF_Handlers.hxx>
#include<iostream>

//Main Function
int CheckIsApproved_andRemovetargetobj(EPM_action_message_t msg)
{
	ResultStatus status(0);
	int iFail = ITK_ok;

	try
	{
		TC_write_syslog("******************** NewActionHandler Execution Started *************************************\n");

		tag_t tMDR_Obj = NULLTAG;
		vector<tag_t> rejected_attachements;

		//Getting root task
		TC_write_syslog("Getting root task....\n");

		tag_t job = NULLTAG;
		status = EPM_ask_job(msg.task, &job);

		tag_t root_task = NULLTAG;
		status = EPM_ask_root_task(job, &root_task);

		//Getting root task attachments
		TC_write_syslog("Getting root task attachments....\n");
		int icount = 0;
		scoped_smptr<tag_t> t_attachments;
		status = EPM_ask_attachments(root_task, EPM_target_attachment, &icount, &t_attachments);

		//Traversing root task attachments for MDR Deliverable
		TC_write_syslog("Traversing root task attachments for MDR Deliverable....\n");
		scoped_smptr<char> qc_level;
		for (int m = 0; m < icount; m++)
		{
			tag_t t_mdr_object_type = NULLTAG;
			status = TCTYPE_ask_object_type(t_attachments[m], &t_mdr_object_type);

			scoped_smptr<char> s_MDRobjectType_name;
			status = TCTYPE_ask_name2(t_mdr_object_type, &s_MDRobjectType_name);
			TC_write_syslog("Target Object Type: %s\n", s_MDRobjectType_name.getString());

			//Checking if target attachment object type matches with the preference value
			logical mdrvalidateType = false;
			for (int o = 0; o < ipref_count; o++)
			{
				if (tc_strcasecmp(s_MDRobjectType_name.getString(), s_pref_values[o]) == 0)
				{
					TC_write_syslog("MDR Object Type matches with the preference value....\n");
					mdrvalidateType = true;
					break;
				}
			}

			if (mdrvalidateType)
			{
				//Checking if MDR Revision
				if (tc_strcmp(s_MDRobjectType_name.getString(), "Mdt0MDRDelRevision") == 0)
				{

					status = AOM_ask_value_string(t_attachments[m], "h4_qc_level", &qc_level);
					TC_write_syslog("Found the Value of QC Level : %s.....\n", qc_level.getString());
					break;
				}
			}
		}

		//Traversing root task attachments
		TC_write_syslog("Traversing root task attachments....\n");
		for (int j = 0; j < icount; j++)
		{
			tag_t t_object_type = NULLTAG;
			status = TCTYPE_ask_object_type(t_attachments[j], &t_object_type);

			scoped_smptr<char> s_objectType_name;
			status = TCTYPE_ask_name2(t_object_type, &s_objectType_name);
			TC_write_syslog("Target Object Type: %s\n", s_objectType_name.getString());

			//Checking if target attachment object type matches with the preference value
			logical validateType = false;
			for (int i = 0; i < ipref_count; i++)
			{
				if (tc_strcasecmp(s_objectType_name.getString(), s_pref_values[i]) == 0)
				{
					TC_write_syslog("Target Object Type matches with the preference value....\n");
					validateType = true;
					break;
				}
			}

			if (validateType)
			{
				//Checking if MDR Revision
				if (tc_strcmp(s_objectType_name.getString(), "Mdt0MDRDelRevision") == 0)
				{
					tMDR_Obj = t_attachments[j];
					TC_write_syslog("Found MDR Revision tag.....\n");
					continue;
				}

				//Creating Response Object
				status = CreateResponseObj(t_attachments[j]);

				//Checking Review State on target attachment i.e MDR Del & Instances
				TC_write_syslog("Checking Review State of target attachment.....\n");
				scoped_smptr<char> s_docOrderpop;
				status = AOM_ask_value_string(t_attachments[j], "h4_qc_review_state", &s_docOrderpop);

				//Check for Instances
				scoped_smptr<char> s_value;
				logical  b_submittal = false;
				if (tc_strcmp(s_objectType_name.getString(), "Mdt0MDRDelRevision") != 0)
				{
					//Checking Review Status on Instances
					status = AOM_ask_value_string(t_attachments[j], "h4_review_status", &s_value);
					TC_write_syslog("Value of 'h4_review_status' property of Target object %s is : %s\n", s_objectType_name.getString(), s_value.getString());

					//Clearing the comment property value to default ie empty on Instances
					status = AOM_refresh(t_attachments[j], TRUE);
					TC_write_syslog("Clearing the comment property value to default ie empty.....\n");
					status = AOM_set_value_string(t_attachments[j], "h4_approv_reject_comment", "");

					//Checking Submittal status of Instances
					TC_write_syslog("Checking Submittal Status of target attachment.....\n");
					status = AOM_ask_value_logical(t_attachments[j], "h4_is_submittal", &b_submittal);
					//TC_write_syslog("Found the Value of Submittal :%B .....\n",b_submittal);
					if (b_submittal)
					{
						TC_write_syslog("Found the Value of Submittal is TRUE\n");
					}
					else
					{
						TC_write_syslog("Found the Value of Submittal is FALSE\n");
					}
				}

				//Checking if target attachment is Rejected or Approved
				tag_t* tQC_approANDrejObj = NULL;
				/////
				scoped_smptr<char> t_attachmentType;
				status = AOM_ask_value_string(t_attachments[j], "object_type", &t_attachmentType);
				TC_write_syslog("t_attachments[j] Type:: %s\n", t_attachmentType.getString());
				/////
				if (tc_strcmp(s_value.getString(), "Rejected") == 0)
				{
					TC_write_syslog("Target attachment is Rejected....\n");

					rejected_attachements.push_back(t_attachments[j]);
					TC_write_syslog("Moving target attachment to Rejected list....\n");

					//Setting Review Status to default value Approved
					TC_write_syslog("Setting Review Status to default value Approved....\n");
					status = AOM_set_value_string(t_attachments[j], "h4_review_status", "Approved");

					//Saving and Checking In target attachment
					TC_write_syslog("Saving the changes to target attachment.....\n");
					status = AOM_save_without_extensions(t_attachments[j]);
					TC_write_syslog("Checking In the target attachment.....\n");
					status = AOM_refresh(t_attachments[j], FALSE);

					//Setting Rejected Release statuses to target attachment
					if (tc_strcmp(s_docOrderpop.getString(), "Order 1") == 0)
					{
						tag_t t_release_status_QCOneReject = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC1_Rejected", &t_release_status_QCOneReject);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCOneReject, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC1_Rejected on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCOneReject);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 2") == 0)
					{
						tag_t t_release_status_QCTwoReject = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC2_Rejected", &t_release_status_QCTwoReject);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCTwoReject, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC2_Rejected on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCTwoReject);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 3") == 0)
					{
						tag_t t_release_status_QCThreeReject = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC3_Rejected", &t_release_status_QCThreeReject);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCThreeReject, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC3_Rejected on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCThreeReject);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 4") == 0)
					{
						tag_t t_release_status_QCFourReject = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC4_Rejected", &t_release_status_QCFourReject);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCFourReject, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC4_Rejected on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCFourReject);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 5") == 0)
					{
						tag_t t_release_status_QCFiveReject = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC5_Rejected", &t_release_status_QCFiveReject);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCFiveReject, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC5_Rejected on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCFiveReject);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 6") == 0)
					{
						tag_t t_release_status_QCSixReject = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC6_Rejected", &t_release_status_QCSixReject);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCSixReject, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC6_Rejected on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCSixReject);
						}
						/////
					}
				}
				else if (tc_strcmp(s_value.getString(), "Approved") == 0)
				{
					TC_write_syslog("Target attachment is Approved....\n");

					//Saving and Checking In target attachment
					TC_write_syslog("Saving the changes to target attachment.....\n");
					status = AOM_save_without_extensions(t_attachments[j]);
					TC_write_syslog("Checking In the target attachment.....\n");
					status = AOM_refresh(t_attachments[j], FALSE);

					//Setting Approved Release statuses to target attachment

					if ((tc_strcmp(s_docOrderpop.getString(), "Order 1") == 0) && (tc_strcmp(qc_level.getString(), "1") == 0) && b_submittal)
					{
						tag_t t_release_status_QCOneApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC_Approved", &t_release_status_QCOneApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCOneApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCOneApprove);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 1") == 0)
					{
						tag_t t_release_status_QCOneApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC1_Approved", &t_release_status_QCOneApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCOneApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC1_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCOneApprove);
						}
						/////
					}
					if ((tc_strcmp(s_docOrderpop.getString(), "Order 2") == 0) && (tc_strcmp(qc_level.getString(), "2") == 0) && b_submittal)
					{
						tag_t t_release_status_QCTwoApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC_Approved", &t_release_status_QCTwoApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCTwoApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCTwoApprove);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 2") == 0)
					{
						tag_t t_release_status_QCTwoApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC2_Approved", &t_release_status_QCTwoApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCTwoApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC2_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCTwoApprove);
						}
						/////
					}
					if ((tc_strcmp(s_docOrderpop.getString(), "Order 3") == 0) && (tc_strcmp(qc_level.getString(), "3") == 0) && b_submittal)
					{
						tag_t t_release_status_QCThreeApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC_Approved", &t_release_status_QCThreeApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCThreeApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCThreeApprove);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 3") == 0)
					{
						tag_t t_release_status_QCThreeApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC3_Approved", &t_release_status_QCThreeApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCThreeApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC3_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCThreeApprove);
						}
						/////
					}
					if ((tc_strcmp(s_docOrderpop.getString(), "Order 4") == 0) && (tc_strcmp(qc_level.getString(), "4") == 0) && b_submittal)
					{
						tag_t t_release_status_QCFourApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC_Approved", &t_release_status_QCFourApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCFourApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCFourApprove);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 4") == 0)
					{
						tag_t t_release_status_QCFourApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC4_Approved", &t_release_status_QCFourApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCFourApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC4_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCFourApprove);
						}
						/////
					}
					if ((tc_strcmp(s_docOrderpop.getString(), "Order 5") == 0) && (tc_strcmp(qc_level.getString(), "5") == 0) && b_submittal)
					{
						tag_t t_release_status_QCFiveApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC_Approved", &t_release_status_QCFiveApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCFiveApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCFiveApprove);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 5") == 0)
					{
						tag_t t_release_status_QCFiveApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC5_Approved", &t_release_status_QCFiveApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCFiveApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC5_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCFiveApprove);
						}
						/////
					}
					if ((tc_strcmp(s_docOrderpop.getString(), "Order 6") == 0) && (tc_strcmp(qc_level.getString(), "6") == 0) && b_submittal)
					{
						tag_t t_release_status_QCSixApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC_Approved", &t_release_status_QCSixApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCSixApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCSixApprove);
						}
						/////
					}
					else if (tc_strcmp(s_docOrderpop.getString(), "Order 6") == 0)
					{
						tag_t t_release_status_QCSixApprove = NULLTAG;
						status = RELSTAT_create_release_status("H4_QC6_Approved", &t_release_status_QCSixApprove);

						tQC_approANDrejObj = &t_attachments[j];
						status = RELSTAT_add_release_status(t_release_status_QCSixApprove, 1, tQC_approANDrejObj, true);
						TC_write_syslog("Setting release status H4_QC6_Approved on target attachment....\n");

						// Release deliverable files
						if (tc_strcmp(t_attachmentType.getString(), "H4_Delv_DocumentRevision") == 0 || tc_strcmp(t_attachmentType.getString(), "H4_DesignRevision") == 0) {
							// Function to get Deliverable files and release them
							getDeliverableFilesFromDeliverables(t_attachments[j], t_release_status_QCSixApprove);
						}
						/////
					}
				}

			}
		}

		int rejection_count = 0;
		rejection_count = rejected_attachements.size();
		TC_write_syslog("Rejected attachment count is: %d\n", rejection_count);

		if (rejection_count > 0)
		{
			tag_t* rejection_list = NULL;
			rejection_list = rejected_attachements.data();
			scoped_smptr<tag_t> attachment_copy_tags;
			int revisionCount = 0;
			vector<tag_t> workflowAttachments;
			vector<int> attachmentTypes;

			for (int k = 0; k < rejection_count; k++)
			{
				//Getting Owning User and Owning Group of Rejected objects
				tag_t owningUser = NULLTAG;
				status = AOM_ask_owner(rejection_list[k], &owningUser);

				tag_t owningGroup = NULLTAG;
				status = AOM_ask_group(rejection_list[k], &owningGroup);

				// Getting Item object of Rejected Revision object
				tag_t itemOfAttachment = NULLTAG;
				status = ITEM_ask_item_of_rev(rejection_list[k], &itemOfAttachment);

				// Getting object type tag
				tag_t type_tag = NULLTAG;
				status = TCTYPE_ask_object_type(rejection_list[k], &type_tag);

				//Getting object type name
				scoped_smptr<char> objectType_name;
				status = TCTYPE_ask_name2(type_tag, &objectType_name);

				// Getting next revision id using pattern
				scoped_smptr<char> nextId;
				status = NR_pattern_next_value(objectType_name.get(), "item_revision_id", itemOfAttachment, "", "", "", itemOfAttachment, "", "", "ANN", &nextId);

				// Getting revise input tag
				tag_t revise_input_tag = NULLTAG;
				status = TCTYPE_construct_operationinput(type_tag, TCTYPE_OPERATIONINPUT_REVISE, &revise_input_tag);

				//Setting next Revision Id on Revise Input
				AOM_set_value_string(revise_input_tag, "item_revision_id", nextId.getString());

				tag_t attachment = rejection_list[k];
				tag_t *attachment_tags = NULL;
				attachment_tags = &attachment;

				tag_t *revise_input_tags = NULL;
				revise_input_tags = &revise_input_tag;

				// Getting deep copy data tags
				int attached_object_count = 0;
				scoped_smptr<tag_t> deepcopydata_tags;
				status = TCTYPE_ask_deepcopydata(rejection_list[k], TCTYPE_OPERATIONINPUT_REVISE, &attached_object_count, &deepcopydata_tags);

				int n_ints_in_list = 0;
				int *all_attached_object_count = NULL;
				int n_tags_in_list = 0;
				tag_t *all_deepcopydata_tags = NULL;

				if (attached_object_count > 0)
				{
					add_int_to_int_array(attached_object_count, &n_ints_in_list, &all_attached_object_count);
					for (int j = 0; j < attached_object_count; j++)
					{
						add_tag_to_tag_array(deepcopydata_tags[j], &n_tags_in_list, &all_deepcopydata_tags);
					}
				}

				// Revising the objects
				scoped_smptr<int> ifails;
				status = TCTYPE_revise_objects(1, attachment_tags, revise_input_tags, all_attached_object_count, all_deepcopydata_tags, &attachment_copy_tags, &ifails);

				// Getting error messages
				scoped_smptr<char> error_message_string;
				status = EMH_ask_error_text(ifails[0], &error_message_string);
				TC_write_syslog("Syslog: EMH_ask_error_text");
				TC_write_syslog("\t%d %s\n", ifails[0], error_message_string.getString());

				if (attachment_copy_tags[0] != NULLTAG)
				{
					TC_write_syslog("Rejected attachment %d is revised successfully!\n", k + 1);
					revisionCount++;

					//Changing ownership of new revision
					status = AOM_refresh(attachment_copy_tags[0], TRUE);
					status = AOM_set_ownership(attachment_copy_tags[0], owningUser, owningGroup);
					status = AOM_save_without_extensions(attachment_copy_tags[0]);
					status = AOM_refresh(attachment_copy_tags[0], FALSE);

					//Pushing new revision to Target attachment vector
					workflowAttachments.push_back(attachment_copy_tags[0]);
					attachmentTypes.push_back(EPM_target_attachment);
				}
			}

			//Removing Rejected attachments from Target
			TC_write_syslog("Removing Rejected attachments from Target.....\n");

			//Checking Out Root Rask
			status = AOM_refresh(root_task, TRUE);

			//Removing attachments
			status = EPM_remove_attachments(root_task, rejection_count, rejection_list);

			//Saving the changes to Root Task
			status = AOM_save_without_extensions(root_task);

			//Checking In Root Rask
			status = AOM_refresh(root_task, FALSE);

			TC_write_syslog("Removal of Rejected attachment is complete.....\n");

			if (revisionCount > 0)
			{
				//Initiating HON_PEM_QC_Rework_Workflow on new revision objects
				TC_write_syslog("Initiating HON_PEM_QC_Rework_Workflow on new revision objects.....\n");

				scoped_smptr<char> mdrRevisionObjectString;
				status = AOM_ask_value_string(tMDR_Obj, "object_string", &mdrRevisionObjectString);

				char processName[1000];
				tc_strcpy(processName, "HON_PEM_QC_Rework_Workflow : ");
				tc_strcat(processName, mdrRevisionObjectString.getString());

				tag_t t_processTemplate = NULLTAG;
				status = EPM_find_process_template("HON_PEM_QC_Rework_Workflow", &t_processTemplate);

				int attachmentCount = revisionCount + rejection_count + 1;

				//Pushing Reference attachments
				for (int n = 0; n < rejection_count; n++)
				{
					workflowAttachments.push_back(rejection_list[n]);
					attachmentTypes.push_back(EPM_reference_attachment);
				}

				workflowAttachments.push_back(tMDR_Obj);
				attachmentTypes.push_back(EPM_target_attachment);

				tag_t* workflowAttachments_t = workflowAttachments.data();
				int* iAttachmentTypes = attachmentTypes.data();

				//Initiating HON_PEM_QC_Rework_Workflow process
				tag_t newProcess = NULLTAG;
				status = EPM_create_process(processName, "", t_processTemplate, attachmentCount, workflowAttachments_t, iAttachmentTypes, &newProcess);

				if (newProcess != NULLTAG)
				{
					TC_write_syslog("HON_PEM_QC_Rework_Workflow initiated successfully on new revision objects.....\n");
				}
			}
		}

		TC_write_syslog("******************** CheckIsApproved_andRemovetargetobj Execution Completed *************************************\n");

	}
	catch (const IFail& ex)
	{
		iFail = ex.ifail();
		scoped_smptr<char> message;
		EMH_ask_error_text(iFail, &message);
		TC_write_syslog("Error : %s\n", message.getString());
	}

	return iFail;
}
