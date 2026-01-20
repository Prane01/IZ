package com.teamcenter.soa.loader;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Properties;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.soa.datasetexport.util.InitiatialPros;

public class Connect {

	public static AppXSession session;
	public static DataManagementService dmService;

	public Connect() {
		// TODO Auto-generated constructor stub
	}

	public Properties getProperties(FileWriter writer) throws IOException{
		
		Properties properties = new Properties();
		try {
			properties.load(getClass().getResourceAsStream("CreateOrUpdateItems.properties"));
			writer.write("\nINFO: CreateOrUpdateItems.properties file loaded successfully !!\n");
			
		} catch (Exception e) {
			e.printStackTrace();
			writer.write("\nERROR: unable to load CreateOrUpdateItems.properties file !!"+e.getMessage());
			return null;
		}
		return properties;
	}
	public static void main(String[] args) {
		
		try {
			
			Date todayDate = new Date();
			SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss");
			String formatted = formatter.format(todayDate);
			
			String sFolderLoc = System.getenv("TEMP")+"\\"+"Logs";
			
			File fileLoc = new File(sFolderLoc);
			
			if (fileLoc.exists()) {
				System.out.println("Logs at " + fileLoc + " location exists");
			} 
			else {
				System.out.println("Logs at " + fileLoc + " location does not exists...");
				System.out.println("Creating directory " + fileLoc);

				boolean isCreated = fileLoc.mkdir();
				if (isCreated) {
					System.out.println("Created directory " + fileLoc);
				} 
				else {
					System.out.println("Unable to directory " + fileLoc);
				}	
			}
			
			String sJarFileLog = sFolderLoc + File.separator + "Test_Data_" + formatted + ".txt";
			
			File logFile = new File(sJarFileLog);
			
			System.out.println("Attempting to write into file " + logFile.getAbsolutePath());
			
			FileWriter writer = new FileWriter(logFile);
			
			writer.write("\nJar file log location: " + logFile.getAbsolutePath());
			writer.write("\n**************** Data Test : Minute Sync - " + formatted + "****************\n\n");
			
			Connect testObj = new Connect();
			Properties properties = testObj.getProperties(writer);
			
			if (properties != null) {
				
				String serverHost = properties.getProperty("TC_4T_URL");
				
				session = new AppXSession(serverHost);
				
				try {
				
					String sUserName = properties.getProperty("USER_ID");
					String sPassword = properties.getProperty("USER_PASS");
					
					session.login(sUserName, sPassword, writer);
					dmService = DataManagementService.getService(AppXSession.getConnection());
					writer.write("\nINFO: User["+sUserName+"/"+sPassword+"]has logged in successfully!!");
					System.out.println("\nINFO: User["+sUserName+"/"+sPassword+"]has logged in successfully!!");
					
					InitiatialPros initProcess = new InitiatialPros();
					initProcess.initiateProcess(writer);
					
				} catch(Exception e) {
					
					System.out.println("\nERROR:LOGIN=Please contact Teamcenter administrator!!");
					writer.write("\nERROR:LOGIN=User could not able to login!!");
					writer.close();
					return;
					
				}
				
				try {
					session.logout();
					writer.write("\nINFO: session has logged out successfully!!");
					writer.close();
					return;
				} catch (Exception e) {
					System.out.println("ERROR: error while logging out.");
					writer.close();
				}
		
			}
			else {
				System.out.println("ERROR: Not able to read properties file.....");
			}
			writer.close();
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("Exception caught: " + e.getMessage());
		}
		

	}

}
