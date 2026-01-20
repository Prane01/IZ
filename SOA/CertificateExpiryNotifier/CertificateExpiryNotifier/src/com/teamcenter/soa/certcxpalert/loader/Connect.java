package com.teamcenter.soa.certcxpalert.loader;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Properties;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.soa.exceptions.NotLoadedException;

public class Connect {

	/**
	 * @throws IOException
	 * @throws NotLoadedException
	 */
	/**
	 * @throws IOException
	 * @throws NotLoadedException
	 */
	public Connect() throws Exception, NotLoadedException {


		Date dTodayDate = new Date();
		SimpleDateFormat formatter = new SimpleDateFormat("yyMMddhhmmssMs");
		SimpleDateFormat formatter2 = new SimpleDateFormat("dd-MMM-yyyy");
		String formattedData = formatter.format(dTodayDate);
		String sUpdatedLastExecutionDate = formatter2.format(dTodayDate);

		String sPath = System.getenv("TEMP")+"\\CertificateExpiryLog_"+formattedData+".txt";
		FileWriter writer = new FileWriter(new File(sPath));
		
		//PROD Path
		String inputfileHBT = "E:\\HON_Deployments\\CDMS\\LastExecutionDateHBT.txt";
		//String inputfilePMT = "E:\\HON_Deployments\\CDMS\\LastExecutionDatePMT.txt";
		
		//TR Path
		//String inputfileHBT = "D:\\Apps\\HON_Deployments\\CDMS_11_10_2023\\LastExecutionDateHBT.txt";
		//String inputfilePMT = "D:\\Apps\\HON_Deployments\\CDMS_11_10_2023\\LastExecutionDatePMT.txt";
		
		//Local Path
		//String inputfileHBT = "D:\\HON_Deployments\\CDMS\\LastExecutionDateHBT.txt";
		//String inputfilePMT = "D:\\HON_Deployments\\CDMS\\LastExecutionDatePMT.txt";
		
		Properties properties = new Properties();
		try
		{
			properties.load(getClass().getResourceAsStream("CertificateExpiry.properties"));
			writer.write("\nINFO:Property file loaded successfully !!");
		}
		catch(Exception e) 
		{
			e.printStackTrace();
			writer.write("\nERROR:Failed while loading the property file"+e.getMessage());
		}



		writer.write("\n*********** CertificateExpiry Updation Utility : Daily Sync -"+formattedData+"************************\n\n");
		writer.write("\n*********** CertificateExpiry Updation Utility : Daily Sync -"+formattedData+"************************\n\n");

		if(properties != null)
		{
			String serverHost = properties.getProperty("TC_4T_URL");

			AppXSession session = new AppXSession(serverHost);
			try
			{
				String sUserName = properties.getProperty("USER_ID");
				String sUserPass = properties.getProperty("USER_PASS");

				session.login(sUserName,sUserPass,writer);
				
				//CertificateExpiryNotifyPMT seReportObj = new CertificateExpiryNotifyPMT(writer,properties);
				CertificateExpiryNotifyHBT seReportObj = new CertificateExpiryNotifyHBT(writer,properties);
				writer.write("\nINFO: User["+sUserName+"/"+sUserPass+"]has logged in successfully!!");

				writer.write("\nINFO: CDMS process --> START");
				
				//BA block
				BufferedReader br = new BufferedReader(new FileReader (inputfileHBT));
				String sLastExecutionDateHBT = br.readLine();
				br.close();
				FileWriter writer2 = new FileWriter(inputfileHBT, false);
				
				//IA block
				/*BufferedReader br2 = new BufferedReader(new FileReader (inputfilePMT));
				String sLastExecutionDatePMT = br2.readLine();
				br2.close();
				FileWriter writer3 = new FileWriter(inputfilePMT, false);*/

				//seReportObj.sendMainNotificationToStakeholdersPMT(sLastExecutionDatePMT);//IA
				seReportObj.sendMainNotificationToStakeholdersHBT(sLastExecutionDateHBT);//BA
				
				//BA block
				writer2.write(sUpdatedLastExecutionDate);
				writer2.close();
				
				//IA block
				/*writer3.write(sUpdatedLastExecutionDate);
				writer3.close();*/
				
				writer.close();
				session.logout();
			}
			catch(Exception ex)
			{
				ex.printStackTrace();
				System.out.println("\nERROR:LOGIN=Please contact Teamcenter administrator!!");
				writer.write("\nERROR:LOGIN=User could not able to login!!");
				writer.close();
				return;
			}	
		}
	}
	


	public static void main(String[] args) throws Exception, NotLoadedException {
		new Connect();
	}

}
