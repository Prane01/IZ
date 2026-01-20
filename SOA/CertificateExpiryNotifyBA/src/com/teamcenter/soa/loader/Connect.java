package com.teamcenter.soa.loader;

import java.io.File;
import java.io.FileWriter;
import java.text.SimpleDateFormat;
import java.time.DayOfWeek;
import java.time.LocalDate;
import java.time.temporal.TemporalAdjusters;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.query.SavedQueryService;
import com.teamcenter.soa.client.FileManagementUtility;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.Folder;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.soa.exceptions.NotLoadedException;
import com.teamcenter.soa.util.*;

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

public class Connect {

	/* Teamcenter Static Service Definition */
	public static DataManagementService BADataManagementSevice = null;
	public static SavedQueryService BASavedQueryService = null;

	/* Teamcenter Static File Management Utility Definition */
	public static FileManagementUtility BAFMSFileManagementUtility = null;

	/* Teamcenter Static Model Object Property Definition */
	public static Properties properties = new Properties();

	public static FileWriter writer;
	/* Teamcenter APAX Session Definition */
	AppXSession BA_TC_SESSION = null;

	public Connect() throws Exception, NotLoadedException {
		Date dTodayDate = new Date();
		SimpleDateFormat formatter = new SimpleDateFormat("yyMMddhhmmssMs");
		SimpleDateFormat formatter2 = new SimpleDateFormat("dd-MMM-yyyy");
		String formattedData = formatter.format(dTodayDate);
		String sUpdatedLastExecutionDate = formatter2.format(dTodayDate);
		try {
			String sPath = System.getenv("TEMP") + "\\CertificateExpiryLog_" + formattedData + ".txt";
			writer = new FileWriter(new File(sPath));
			properties.load(getClass().getResourceAsStream("CertificateExpiry.properties"));
			MailConfiguration.properties = properties;
			writer.write("\nINFO:Property file loaded successfully !!");
		} catch (Exception e) {
			e.printStackTrace();
			writer.write("\nERROR: Failed to load property file 'CertificateExpiry.properties'. Message=["
					+ e.getMessage() + "], Cause=[" + e.getClass().getName() + "]\n");
		}
		writer.write("\n*********** CertificateExpiry Updation Utility : Daily Sync -" + formattedData
				+ "************************\n\n");
	}

	@SuppressWarnings("unused")
	public static void main(String[] args) {

		try {

			Connect connect = new Connect();
//        	String TC_4T_URL   		= args[0];
//        	String TC_LOGIN_USER    = args[1];
//        	String TC_LOGIN_PSWD    = args[2];

			String TC_4T_URL = "https://gplmqa.honeywell.com/tc";
			String TC_LOGIN_USER = "btp-sponsor";
			String TC_LOGIN_PSWD = "xeE6AbotdfzybMI0lQ229Lcq46rsSTt4";

			if (TC_4T_URL.length() == 0 || TC_LOGIN_USER.length() == 0 || TC_LOGIN_PSWD.length() == 0) {
//				System.out.println("ERROR: Inavalid Aruguments! Please check with System Administrator...");
				throw new HCADInvalidArgumentException("Invalid Arguments Recieved ...");
			} else {
//				System.out.println("Aruguments Received...");
				writer.write("INFO Teamcenter 4T URL 	   : [" + TC_4T_URL + "]\n");
				writer.write("Teamcenter Login User  : [" + TC_LOGIN_USER + "]\n");
				writer.write("Teamcenter Login Pass  : [" + "*************" + "]\n");
			}

			EncryptDecryptMain EncryptDecryptMainObj = new EncryptDecryptMain();
			String StrPasswordDecrypt = EncryptDecryptMainObj.decrypt(TC_LOGIN_PSWD);

			if (StrPasswordDecrypt != null && StrPasswordDecrypt.length() > 0) {
				writer.write("INFO Password Decryption is completed...\n");
			} else {
				writer.write("ERROR Password Decryption is failed, Contact System Administrator ...\n");
				throw new PasswordDecryptionException("Password Decryption is failed ...");
			}

			/* Teamcenter Login */
			AppXSession TC_Session = null;
			Folder HomeFolder = null;

			try {
				/* Get Teamcenter Session with Login credentials */
				TC_Session = new AppXSession(TC_4T_URL);

				User TC_USER = TC_Session.login(TC_LOGIN_USER,StrPasswordDecrypt,writer);
//				User TC_USER = TC_Session.login(TC_LOGIN_USER, TC_LOGIN_PSWD, writer);

				System.out.println("***********************Logged in successfully**********************");
				writer.write("***********************Logged in successfully**********************");
				/*
				 * Teamcenter Static Services from received Teamcenter Connection through APAX
				 * Session
				 */
				BADataManagementSevice = DataManagementService.getService(AppXSession.getConnection());

			} catch (Exception ex) {
				ex.printStackTrace();
				System.out.println("ERROR: Teamcenter login failed. Message=[" + ex.getMessage() + "], Cause=["
						+ ex.getClass().getName() + "]");
				writer.write("ERROR: Teamcenter login failed. Message=[" + ex.getMessage() + "], Cause=["
						+ ex.getClass().getName() + "]\n");
			}

			BATCQuery QueryObj = new BATCQuery();

			try {

				List<String> notificationFreqs = new ArrayList<>();
				LocalDate todayDate = LocalDate.now();
//				LocalDate todayDate = LocalDate.of(2025, 6, 30);

				writer.write("todayDate : \n" + todayDate);

				// 1. Weekly: Execute if today is MONDAY
				if (todayDate.getDayOfWeek() == DayOfWeek.MONDAY) {
					System.out.println("Weekly condition met (Monday).");
					writer.write("Weekly condition met (Monday).\n");
					notificationFreqs.add("Weekly");
				}

//		     // 2. Bi-Weekly Trigger only for 1st and 3rd Monday of the month
//		        if(notificationFreqs.contains("Weekly")) {
//		        	List<LocalDate> mondaysThisMonth = getMondaysOfMonth(todayDate.getYear(), todayDate.getMonthValue());
//		            if (todayDate.equals(mondaysThisMonth.get(0)) || todayDate.equals(mondaysThisMonth.get(2))) {
//		                System.out.println("Bi-Weekly condition met (1st or 3rd Monday of month).");
//		                writer.write("Bi-Weekly condition met (even week, Monday).\n");
//						notificationFreqs.add("Bi-Weekly");
//		            }
//		        }

				// 2. Bi-Weekly Trigger: 15th and last day of the month (runs independently)
				int dayOfMonth = todayDate.getDayOfMonth();
				int lastDay = todayDate.with(TemporalAdjusters.lastDayOfMonth()).getDayOfMonth();

				if (dayOfMonth == 15 || dayOfMonth == lastDay) {
					System.out.println("Bi-Weekly condition met (15th or last day of the month).");
					writer.write("Bi-Weekly condition met (15th or last day of the month).\n");
					notificationFreqs.add("Bi-Weekly");
				}

				// 3. Monthly: Execute if today is the last day of the month
				LocalDate lastDayOfMonth = todayDate.with(TemporalAdjusters.lastDayOfMonth());
				if (todayDate.isEqual(lastDayOfMonth)) {
					System.out.println("Monthly condition met (last day of month).");
					writer.write("Monthly condition met (last day of month).\n");
					notificationFreqs.add("Monthly");
				}
				ArrayList<ModelObject> BACatalogObjectList = new ArrayList<ModelObject>();
				ArrayList<ModelObject> TempBACatalogs = new ArrayList<ModelObject>();

				// Dev
        		TempBACatalogs = QueryObj.PerformSearchQuery("HON BA Catalog", new String[] { "Notification Frequency" },
						new String[] { "Weekly"});
//				TempBACatalogs = QueryObj.PerformSearchQuery("HON BA Catalog",
//						new String[] { "Catalog Name", "Notification Frequency" },
//						new String[] { "Supra Catalog & Onity Catalog", "Monthly" });

				BACatalogObjectList.addAll(TempBACatalogs);

				System.out.println("**********");
				System.out.println("Total " + notificationFreqs.size() + " ObjectList : " + BACatalogObjectList.size());
				System.out.println("**********");
				writer.write(
						"Total " + notificationFreqs.size() + " ObjectList : " + BACatalogObjectList.size() + "\n");
				ProcessCatalog catalog = new ProcessCatalog();
				MailConfiguration mailCon = new MailConfiguration(properties);
				catalog.processCatalogObjects(BACatalogObjectList);
				System.out.println("*** getAllMaps ***");
				Map<String, Map<String, Set<String>>> allCategories = catalog.getAllMaps();
				System.out.println("*** configMail ***");
				mailCon.configMail(allCategories);

//		        if(notificationFreqs.size() > 0) { 
//		        	for(int i = 0; i < notificationFreqs.size(); i++) {
//		        		TempBACatalogs = QueryObj.PerformSearchQuery("HON BA Catalog", new String[] { "Notification Frequency" },
//								new String[] { notificationFreqs.get(i) });
//		        		
//		        		BACatalogObjectList.addAll(TempBACatalogs);
//						System.out.println("**********");
//						System.out.println("Freq " + (i+1) + " ObjectList : " + TempBACatalogs.size());
//						System.out.println("**********");
//						writer.write("Freq " + (i+1) + " ObjectList : " + TempBACatalogs.size()+"\n");
//		        	}
//		        	System.out.println("**********");
//					System.out.println("Total " + notificationFreqs.size() + " ObjectList : " + BACatalogObjectList.size());
//					System.out.println("**********");
//					writer.write("Total " + notificationFreqs.size() + " ObjectList : " + BACatalogObjectList.size()+"\n");
//					ProcessCatalog catalog = new ProcessCatalog();
//					MailConfiguration mailCon = new MailConfiguration(properties);
//					catalog.processCatalogObjects(BACatalogObjectList);
//					System.out.println("*** getAllMaps ***");
//					Map<String, Map<String, Set<String>>> allCategories = catalog.getAllMaps();
//					System.out.println("*** configMail ***");
//					mailCon.configMail(allCategories);
//		        }
//		        else {
//		        	System.out.println("No Notification or Not the date to trigger");
//		        	writer.write("No Notification or Not the date to trigger");
//		        	writer.close();
//		        }

				TC_Session.logout();
				writer.close();
			} catch (Exception ex) {
				ex.printStackTrace();
				System.out.println("ERROR: BA Catalog processing failed. Message=[" + ex.getMessage() + "], Cause=["
						+ ex.getClass().getName() + "]");
				writer.write("ERROR: BA Catalog processing failed. Message=[" + ex.getMessage() + "], Cause=["
						+ ex.getClass().getName() + "]\n");
				writer.close();
			}

		} catch (Exception ex) {
			ex.printStackTrace();
			System.out.println("FATAL ERROR: BA Data Application failed. " + "Message=[" + ex.getMessage() + "], "
					+ "Cause=[" + ex.getClass().getName() + "]");
			try {
				if (writer != null) {
					writer.write("FATAL ERROR: BA Data Application failed. " + "Message=[" + ex.getMessage() + "], "
							+ "Cause=[" + ex.getClass().getName() + "]\n");
					writer.close();
				}
			} catch (Exception logEx) {
				logEx.printStackTrace();
			}
		}
	}

	// Utility to find all Mondays in a given month
	public static List<LocalDate> getMondaysOfMonth(int year, int month) {
		List<LocalDate> mondays = new ArrayList<>();
		LocalDate date = LocalDate.of(year, month, 1);

		// Loop through the month
		while (date.getMonthValue() == month) {
			if (date.getDayOfWeek() == DayOfWeek.MONDAY) {
				mondays.add(date);
			}
			date = date.plusDays(1);
		}
		return mondays;
	}
}