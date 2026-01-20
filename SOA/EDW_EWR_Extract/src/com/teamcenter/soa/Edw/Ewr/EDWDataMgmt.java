
package com.teamcenter.soa.Edw.Ewr;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Properties;
import java.util.List;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.query.SavedQueryService;
import com.teamcenter.soa.Edw.Ewr.Util.EDWMessageUtil;
import com.teamcenter.soa.Edw.Ewr.Util.EDWTCQuery;
import com.teamcenter.soa.Edw.Ewr.Util.ProcessUtil;
import com.teamcenter.soa.Edw.Ewr.model.EWRData;
import com.teamcenter.soa.client.FileManagementUtility;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.Folder;
import com.teamcenter.soa.client.model.strong.RevisionRule;
import com.teamcenter.soa.client.model.strong.User;

class HCADInvalidArgumentException extends Exception {
	public HCADInvalidArgumentException(String str) {
		// calling the constructor of parent Exception
		super(str);
	}
}

class HCADInvalidFilePathException extends Exception {
	public HCADInvalidFilePathException(String str) {
		// calling the constructor of parent Exception
		super(str);
	}
}

class PasswordDecryptionException extends Exception {
	public PasswordDecryptionException(String str) {
		// calling the constructor of parent Exception
		super(str);
	}
}

public class EDWDataMgmt {

	public static String StrEDWDirPath = null;
	public static String StrProjectName = "";

	/* Static String Builder Definition for Log Writing */
	public static StringBuilder EDWLogBuilder = new StringBuilder();

	public static StringBuilder EDWDataBuilder = null;

	/* Static File Writer Definition for Log Writing */
	public static FileWriter EDWLogWriter = null;

	/* Teamcenter Static Service Definition */
	public static DataManagementService EDWDataManagementSevice = null;
	public static SavedQueryService EDWSavedQueryService = null;

	/* Teamcenter Static File Management Utility Definition */
	public static FileManagementUtility EDWFMSFileManagementUtility = null;

	/* Teamcenter Static Model Object Property Definition */
	public static Properties EDWCfgProps = new Properties();

	/* Teamcenter APAX Session Definition */
	AppXSession EDW_TC_SESSION = null;


	public static void main(String[] args) {
		String StrLogFilePath = null;
		boolean IsArgumentsCorrect = false;
		String StrObjectUID = "";

		try {
			
			String TC_4T_URL = args[0];
			String TC_LOGIN_USER = args[1];
			String TC_LOGIN_PSWD = args[2];
			String StrstartDate = args[3];
			String StrEndDate = args[4];
			String StrSearchCriteria = args[5];
			String StrCSVDirPath = args[6];
			String StrLogDirPath = args[7];
 
			if (TC_4T_URL.length() == 0 || TC_LOGIN_USER.length() == 0 || TC_LOGIN_PSWD.length() == 0
					|| StrstartDate.length() == 0 || StrEndDate.length() == 0 || StrSearchCriteria.length() == 0
					|| StrCSVDirPath.length() == 0 || StrLogDirPath.length() == 0) {
				System.out.println("ERROR: Inavalid Aruguments! Please check with System Administrator...");
				EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
						"Inavalid Aruguments! Please check with System Administrator...\n"));

				throw new HCADInvalidArgumentException("Invalid Arguments Recieved ...");
			} else {
				EDWLogBuilder
						.append(EDWMessageUtil.LogMessage("INFO", "Teamcenter 4T URL 	   : [" + TC_4T_URL + "]\n"));
				EDWLogBuilder.append(
						EDWMessageUtil.LogMessage("INFO", "Teamcenter Login User  : [" + TC_LOGIN_USER + "]\n"));
				EDWLogBuilder.append(
						EDWMessageUtil.LogMessage("INFO", "Teamcenter Login Pass  : [" + "*************" + "]\n"));
				EDWLogBuilder.append(
						EDWMessageUtil.LogMessage("INFO", "Query Start Date  	   : [" + StrstartDate + "]\n"));
				EDWLogBuilder
						.append(EDWMessageUtil.LogMessage("INFO", "Query End Date 	       : [" + StrEndDate + "]\n"));
				EDWLogBuilder.append(
						EDWMessageUtil.LogMessage("INFO", "Query Search Criteria  : [" + StrSearchCriteria + "]\n"));
				EDWLogBuilder.append(
						EDWMessageUtil.LogMessage("INFO", "CSV Dir Path           : [" + StrCSVDirPath + "]\n"));
				EDWLogBuilder.append(
						EDWMessageUtil.LogMessage("INFO", "LOG Dir Path           : [" + StrLogDirPath + "]\n"));
				EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "Argument received successfully ...\n"));
			}

			String StrFileTimeStamp = EDWMessageUtil.GetDateTimeStamp();
			StrLogFilePath = StrLogDirPath + File.separator + "EWR_ITK_Export_Log_" + StrFileTimeStamp + ".log";


			/* Configure Log Writer */
			EDWLogWriter = new FileWriter(new File(StrLogFilePath));

			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO",
					"***************    EDW EWR EXTRACT MANAGEMENT APPLICATION    ***************\n"));
			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO",
					"EDW EWR EXTRACT MANAGEMENT APPLICATION, Execution is Started ... \n"));

			EncryptDecryptMain EncryptDecryptMainObj = new EncryptDecryptMain();
			String StrPasswordDecrypt = EncryptDecryptMainObj.decrypt(TC_LOGIN_PSWD);

			if (StrPasswordDecrypt != null && StrPasswordDecrypt.length() > 0) {
				EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "Password Decryption is completed...\n"));
			} else {
				EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
						"Password Decryption is failed, Contact System Administrator ...\n"));

				throw new PasswordDecryptionException("Password Decryption is failed ...");
			}

			/* Teamcenter Login */
			AppXSession TC_Session = null;
			FileWriter writer = null;

			try {
				/* Get Teamcenter Session with Login credentials */
				TC_Session = new AppXSession(TC_4T_URL);

				User TC_USER = TC_Session.login(TC_LOGIN_USER, StrPasswordDecrypt, writer);

				System.out.println("***********************Logged in successfully**********************");

				/*
				 * Teamcenter Static Services from received Teamcenter Connection through APAX
				 * Session
				 */
				EDWDataManagementSevice = DataManagementService.getService(AppXSession.getConnection());

			} catch (Exception ex) {
				ex.printStackTrace();
				EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
						"Getting error during teamcenter Login... error message  :[" + ex.getMessage() + "]...\n"));
			}

			EDWLogBuilder.append(
					EDWMessageUtil.LogMessage("INFO", "EDW Appliation is connected with Teamcenter succesfully ...\n"));
			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "Teamenter Connection Details :: Server : ["
					+ AppXSession.getConnection().getServerAddress().toString() + "]\n"));


			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "Quering Engineering Work Request Revision...\n"));

			String StrCritariaFst = StrSearchCriteria + " After";
			String StrCritariaSec = StrSearchCriteria + " Before";

			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "EDW EWR Extraction process started ...\n"));
			try {
				EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO",
						"Quering Engineering Work Request Revision Process Started ...\n"));

				ArrayList<ModelObject> ObjectList = EDWTCQuery.PerformSearchQuery("Item Revision...",
						new String[] { StrCritariaFst, StrCritariaSec, "Type" },
						new String[] { StrstartDate, StrEndDate, "Engineering Work Request Revision" });

				EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO",
						"Quering Engineering Work Request Revision Process Completed ...\n"));
				EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO",
						"Object found in query : [ " + ObjectList.size() + " ] ...\n"));

				int ilines = 0;
				int fileCount = 1;
				FileWriter EDWDataWriter = null;

				if (ObjectList.size() > 0) {

					for (ModelObject RevObject : ObjectList) {

						String StrGovClsfn = "";
						String StrParentItemID = "";
						String StrParentRevID = "";

						EDWDataManagementSevice.getProperties(new ModelObject[] { RevObject },
								new String[] { "item_id", "last_mod_date", "item_revision_id", "gov_classification" });

						StrGovClsfn = RevObject.getPropertyDisplayableValue("gov_classification");
						StrParentItemID = RevObject.getPropertyDisplayableValue("item_id");

						StrParentRevID = RevObject.getPropertyDisplayableValue("item_revision_id");

						if (StrGovClsfn == null || StrGovClsfn.equalsIgnoreCase("ESO1.0")
								|| StrGovClsfn.equalsIgnoreCase("") || StrGovClsfn.equals("")) {
							EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO",
									"EWR Data extract started for EWR ID : [ " + StrParentItemID
											+ "] and Revision ID : [ " + StrParentRevID + "] and Gov Classification :["
											+ StrGovClsfn + "]...\n"));

							EDWDataBuilder = new StringBuilder();

							ProcessUtil processUtilObj = new ProcessUtil();

							processUtilObj.LoadAndPrintProperties(RevObject, EDWDataBuilder);

							try {
								EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO",
										"EDW CSV file writing process started ...\n"));

								String[] lines = EDWDataBuilder.toString().split("\n");

								String StrCSVFilePath = "";

								if (EDWDataWriter == null) {
									StrCSVFilePath = StrCSVDirPath + File.separator + "EWR_ITK_Export_" + StrFileTimeStamp
											+ "_" + fileCount + ".csv";
									EDWDataWriter = new FileWriter(StrCSVFilePath, StandardCharsets.UTF_8);
									EDWDataWriter.write(
											"\"puid\",\"item_id\",\"object_name\",\"object_type\",\"creation_date\",\"gov_classification\",\"h4_Reference_Links\",\"h4_release_status\",\"date_released\",\"owning_user\",\"owning_group\",\"last_mod_user\",\"h4_EWR_SW_Only\",\"h4_sbg\",\"h4_strategic_business_unit\",\"h4_gold_business_enterprise\",\"h4_line_of_business\",\"h4_Estimated_Engg_Effort\",\"h4_Product_Category\",\"h4_Product_Family\",\"h4_EWR_Origin_Region\",\"h4_EWR_Factory_Site\",\"h4_EWR_Status\",\"h4_Assigned_Engineer\",\"h4_EWR_Reviewer\",\"h4_Reqs_Completion_Date\",\"h4_Revenue_Impact\",\"h4_Margin_Impact\",\"h4_Customer_Contact\",\"h4_BOM_Change_Customer_Req\",\"h4_Contract_Number\",\"h4_Reason_for_Notification\",\"h4_accolade_project_tr\",\"h4_Request_Category\",\"h4_Request_Sub_Category\",\"h4_Owning_Function\",\"h4_Effort\",\"h4_Opportunity\",\"h4_Strategic_Fit\",\"h4_Rev_Impact\",\"h4_Severity_Frequency\",\"h4_Cust_Response_Field_Imp\",\"h4_EWRPriorityScore\",\"h4_EWRCOPQ\",\"h4_COPQ_Field_Impact\",\"h4_EWRYield_loss\",\"h4_EWRDPMO\",\"h4_Latest_Comments\",\"h4_Issue_Description\",\"h4_Business_Just_Impact\",\"h4_Changes_Required\",\"h4_Risks\",\"owning_project\",\"SKU.item_id\",\"SKU.object_name\",\"SKU.h4_accolade_combined_ids\",\"SKU.h4_accolade_project_tr\",\"WF.job_name\",\"WF.task_template\",\"WF.fnd0TaskPathName\",\"WF.task_result\",\"WF.state\",\"WF.comments\",\"WF.fnd0Priority\",\"WF.fnd0StartDate\",\"WF.fnd0EndDate\",\"WF.due_date\",\"WF.h4_escalation_level\",\"WF.the_task_duration\",\"WF.creation_date\",\"WF.last_mod_date\",\"WF.owning_user\",\"WF.fnd0WorkflowInitiator\",\"TASK.task_template\",\"TASK.fnd0TaskPathName\",\"TASK.task_result\",\"TASK.state\",\"TASK.comments\",\"TASK.fnd0Priority\",\"TASK.fnd0StartDate\",\"TASK.fnd0EndDate\",\"TASK.due_date\",\"TASK.h4_escalation_level\",\"TASK.the_task_duration\",\"TASK.creation_date\",\"TASK.last_mod_date\",\"TASK.task_type\",\"TASK.resp_party\",\"TASK.fnd0Performer\"\n");
									ilines++;
								}

								for (String line : lines) {

									if (!line.isEmpty()) {
										if (ilines < 100000) {
											ilines++;
											if (EDWDataWriter != null)
												EDWDataWriter.write(line + "\n");

										} else {
											ilines = 0;
											fileCount++;
											EDWDataWriter.close();
											StrCSVFilePath = StrCSVDirPath + File.separator + "EWR_ITK_Export_"
													+ StrFileTimeStamp + "_" + fileCount + ".csv";
											EDWDataWriter = new FileWriter(StrCSVFilePath, StandardCharsets.UTF_8);
											EDWDataWriter.write(
											"\"puid\",\"item_id\",\"object_name\",\"object_type\",\"creation_date\",\"gov_classification\",\"h4_Reference_Links\",\"h4_release_status\",\"date_released\",\"owning_user\",\"owning_group\",\"last_mod_user\",\"h4_EWR_SW_Only\",\"h4_sbg\",\"h4_strategic_business_unit\",\"h4_gold_business_enterprise\",\"h4_line_of_business\",\"h4_Estimated_Engg_Effort\",\"h4_Product_Category\",\"h4_Product_Family\",\"h4_EWR_Origin_Region\",\"h4_EWR_Factory_Site\",\"h4_EWR_Status\",\"h4_Assigned_Engineer\",\"h4_EWR_Reviewer\",\"h4_Reqs_Completion_Date\",\"h4_Revenue_Impact\",\"h4_Margin_Impact\",\"h4_Customer_Contact\",\"h4_BOM_Change_Customer_Req\",\"h4_Contract_Number\",\"h4_Reason_for_Notification\",\"h4_accolade_project_tr\",\"h4_Request_Category\",\"h4_Request_Sub_Category\",\"h4_Owning_Function\",\"h4_Effort\",\"h4_Opportunity\",\"h4_Strategic_Fit\",\"h4_Rev_Impact\",\"h4_Severity_Frequency\",\"h4_Cust_Response_Field_Imp\",\"h4_EWRPriorityScore\",\"h4_EWRCOPQ\",\"h4_COPQ_Field_Impact\",\"h4_EWRYield_loss\",\"h4_EWRDPMO\",\"h4_Latest_Comments\",\"h4_Issue_Description\",\"h4_Business_Just_Impact\",\"h4_Changes_Required\",\"h4_Risks\",\"owning_project\",\"SKU.item_id\",\"SKU.object_name\",\"SKU.h4_accolade_combined_ids\",\"SKU.h4_accolade_project_tr\",\"WF.job_name\",\"WF.task_template\",\"WF.fnd0TaskPathName\",\"WF.task_result\",\"WF.state\",\"WF.comments\",\"WF.fnd0Priority\",\"WF.fnd0StartDate\",\"WF.fnd0EndDate\",\"WF.due_date\",\"WF.h4_escalation_level\",\"WF.the_task_duration\",\"WF.creation_date\",\"WF.last_mod_date\",\"WF.owning_user\",\"WF.fnd0WorkflowInitiator\",\"TASK.task_template\",\"TASK.fnd0TaskPathName\",\"TASK.task_result\",\"TASK.state\",\"TASK.comments\",\"TASK.fnd0Priority\",\"TASK.fnd0StartDate\",\"TASK.fnd0EndDate\",\"TASK.due_date\",\"TASK.h4_escalation_level\",\"TASK.the_task_duration\",\"TASK.creation_date\",\"TASK.last_mod_date\",\"TASK.task_type\",\"TASK.resp_party\",\"TASK.fnd0Performer\"\n");
											EDWDataWriter.write(line + "\n");
											ilines = ilines + 2;
										}
									}
								}
							} catch (IOException ex) {
								ex.printStackTrace();
								EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
										"EDW CSV file process is failed with error meaasge  :[" + ex.getMessage()
												+ "]...\n"));

							}
						}

						EDWLogBuilder.append(
								EDWMessageUtil.LogMessage("INFO", "EDW EWR Extraction process completed ...\n"));
					}
				}
				if (EDWDataWriter != null)
					EDWDataWriter.close();
				System.out.println("****************** The process has completed ***************************");
			} catch (Exception ex) {
				ex.printStackTrace();
				EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
						"EDW EWR Extraction process is failed with error meaasge  :[" + ex.getMessage() + "]...\n"));

			}

			EDWLogBuilder
					.append(EDWMessageUtil.LogMessage("INFO", "Deliverable EWR Package  reading process complete..\n"));
		} catch (Exception ex) {
			ex.printStackTrace();
			EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
					"EDW DATA EXTARCTION APPLICATION is failed with error meaasge  :[" + ex.getMessage() + "]...\n"));

		}

		finally {

			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "LogOut the Teamcenter Session!!!\n"));
			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "EWR Export Completed ...\n"));
			AppXSession.logout();
			if (EDWLogWriter != null) {
				try {

					EDWLogWriter.write(EDWLogBuilder.toString());
					if (EDWLogWriter != null)
						EDWLogWriter.close();

				} catch (IOException e) {
					e.printStackTrace();
				}

			}

		}
	}

}