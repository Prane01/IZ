package com.teamcenter.soa.EDW.AMPL;

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
import com.teamcenter.soa.EDW.AMPL.Util.EDWMessageUtil;
import com.teamcenter.soa.EDW.AMPL.Util.EDWTCQuery;
import com.teamcenter.soa.EDW.AMPL.Util.ProcessUtil;
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

	public static StringBuilder EDWDataBuilder = new StringBuilder();

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

	@SuppressWarnings("unused")
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

//        	String TC_4T_URL   		= "https://gplm.honeywell.com/tc";
//        	String TC_LOGIN_USER    = "btp-sponsor";
//        	String TC_LOGIN_PSWD    = "xeE6AbotdfzybMI0lQ229Lcq46rsSTt4";
//        	String StrstartDate		= "01-Jan-2025 00:00:00";
//        	String StrEndDate		= "14-Feb-2025 23:59:59";
//        	String StrSearchCriteria= "Modified";
//        	String StrCSVDirPath	= "D:\\SAIDHEEP\\Logs\\ampl\\prod";
//        	String StrLogDirPath	= "D:\\SAIDHEEP\\Logs\\ampl\\prod";

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

			StrLogFilePath = StrLogDirPath + File.separator + "AMPL_ITK_Export_Log_" + StrFileTimeStamp + ".log";

			String StrTimeStamp = EDWMessageUtil.GetDateTimeStamp();
			/* Configure Log Writer */
			EDWLogWriter = new FileWriter(new File(StrLogFilePath));

			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO",
					"***************    EDW AMPL EXTRACT MANAGEMENT APPLICATION    ***************\n"));
			EDWLogBuilder.append(
					EDWMessageUtil.LogMessage("INFO", "EDW AMPL EXTRACT APPLICATION, Execution is Started ... \n"));

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
			Folder HomeFolder = null;
			FileWriter writer = null;

			try {
				/* Get Teamcenter Session with Login credentials */
				TC_Session = new AppXSession(TC_4T_URL);

				User TC_USER = TC_Session.login(TC_LOGIN_USER, StrPasswordDecrypt, writer);

				System.out.println("*********************** Logged in successfully **********************");

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

			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "Quering Manufacturer Part Revision...\n"));

			String StrCritariaFst = StrSearchCriteria + " After";
			String StrCritariaSec = StrSearchCriteria + " Before";

			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "EDW AMPL Extraction process started ...\n"));
			try {
				EDWLogBuilder.append(
						EDWMessageUtil.LogMessage("INFO", "Quering Manufacturer Part Revision Process Started ...\n"));

				ArrayList<ModelObject> ObjectList = EDWTCQuery.PerformSearchQuery("Item Revision...",
						new String[] { StrCritariaFst, StrCritariaSec, "Type" },
						new String[] { StrstartDate, StrEndDate, "Manufacturer Part Revision" });

				EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO",
						"Quering Manufacturer Part Revision Process Completed ...\n"));
				EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO",
						"Object found in query : [ " + ObjectList.size() + " ] ...\n"));

				ProcessUtil processUtilObj = new ProcessUtil();

				processUtilObj.LoadAndPrintProperties(ObjectList, StrCSVDirPath, StrLogDirPath, StrFileTimeStamp);

				EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "AMPL Parsing is completed...\n"));

				EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "EDW AMPL Extraction process completed ...\n"));
			} catch (Exception ex) {
				ex.printStackTrace();
				EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
						"EDW AMPL Extraction process is failed with error meaasge  :[" + ex.getMessage() + "]...\n"));

			}

			EDWLogBuilder.append(
					EDWMessageUtil.LogMessage("INFO", "Deliverable AMPL Package  reading process complete..\n"));

		} catch (Exception ex) {
			ex.printStackTrace();
			EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
					"EDW DATA EXTARCTION APPLICATION is failed with error meaasge  :[" + ex.getMessage() + "]...\n"));

		}

		finally {

			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "LogOut the Teamcenter Session!!!\n"));
			EDWLogBuilder.append(EDWMessageUtil.LogMessage("INFO", "AMPL Export Completed ...\n"));

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