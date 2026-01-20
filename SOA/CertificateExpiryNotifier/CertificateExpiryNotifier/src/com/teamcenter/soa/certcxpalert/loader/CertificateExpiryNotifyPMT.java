package com.teamcenter.soa.certcxpalert.loader;

import java.io.FileWriter;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Properties;
import java.util.Set;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.workflow.WorkflowService;
import com.teamcenter.services.strong.workflow._2008_06.Workflow.ContextData;
import com.teamcenter.services.strong.workflow._2008_06.Workflow.InstanceInfo;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.Person;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.soa.exceptions.NotLoadedException;
import com.teamcenter.soa.obselencealert.util.DataManagement;
import com.teamcenter.soa.obselencealert.util.QueryManagement;
import com.teamcenter.soa.obselencealert.util.SendNotification;

public class CertificateExpiryNotifyPMT {

	private FileWriter writer;
	public Properties properties = null;
	HashMap<String, String> hmEmailIdItemIdMap = new HashMap<String, String>();
	HashMap<String, String> hmItemIdCatalogMap = new HashMap<String, String>();
	HashMap<String, String> hmItemIdExpiryMap = new HashMap<String, String>();
	HashMap<String, String> hmItemIdOwningUserMap = new HashMap<String, String>();

	public CertificateExpiryNotifyPMT(FileWriter writer, Properties properties) {
		this.writer = writer;
		this.properties = properties;
	}
	
	public void sendMainNotificationToStakeholdersPMT(String sLastExecutionDate)
	{
		try
		{
			SimpleDateFormat formatter=new SimpleDateFormat("dd-MMM-yyyy");
			SimpleDateFormat formatter2=new SimpleDateFormat("dd-MMM-yyyy HH:mm");
			Date dExpiry = formatter.parse(sLastExecutionDate);
			Date dCurrent = new Date();
			String dCurrentFormatted = formatter.format(dCurrent);
			dCurrent = formatter.parse(dCurrentFormatted);
			Calendar cExpiry = Calendar.getInstance();
			int iDifference = 0;
			if(dExpiry.before(dCurrent))
			{
				while(dExpiry.before(dCurrent))
				{
					iDifference++;
					cExpiry.setTime(dExpiry);
					cExpiry.add(Calendar.DATE, 1);
					dExpiry = cExpiry.getTime();
				}
			}
//			if(iDifference >= 7)
//			{
				System.out.println("****** Before executing monthwise alert notification ********************* ");
				sendMonthwiseNotificationToStakeholders(iDifference, formatter, formatter2);
				
				System.out.println("****** Before executing main notification (Before 12 month) ********************* ");
				
				ModelObject[] alResponse = QueryManagement.queryObjects("HON Compliance Document", new String[] {"ID","In Process","Release Status","Gold Business Enterprise (GBE)"}, new String[] {"*","FALSE","Released","Process Measurement & Control (PMC)"}, writer);
				
				if(alResponse!=null) 
				{
					System.out.println("Total document found: "+alResponse.length);
					writer.write("\n\nTotal document found: "+alResponse.length);
					
					DataManagement.loadProperties(alResponse, new String[] {"item_id","H4_hon_pmt_catalogs", "H4_hon_hbt_catalogs","h4_expiry_date","h4_line_of_business","h4_strategic_business_grp"}, writer);
					
					DataManagement.refreshObject(alResponse);
					for (ModelObject modelObject : alResponse) 
					{
						String sItemId = modelObject.getPropertyDisplayableValue("item_id");
						String sExpiryDate = modelObject.getPropertyDisplayableValue("h4_expiry_date");
						String sSBG = modelObject.getPropertyDisplayableValue("h4_strategic_business_grp");
						
						System.out.println("Item Id : "+sItemId+" , Exp Date : "+sExpiryDate+" , SBG : "+sSBG);
						writer.write("\n\nItem Id : "+sItemId+" , Exp Date : "+sExpiryDate+" , SBG : "+sSBG);

						List <String> sPMTCatelog = modelObject.getPropertyDisplayableValues("H4_hon_pmt_catalogs");
						System.out.print(" ,  Catalog Dumy: "+sPMTCatelog);
						writer.write(" ,  Catalog Dumy: "+sPMTCatelog+"\n");
						
						System.out.println(" Before 12months expiry alert for Process Measurement & Control");
						writer.write("Before 12months expiry alert for Process Measurement & Control");
				
						checkExpiryDate12MonthsPMT(sExpiryDate , sPMTCatelog , formatter , formatter2 , modelObject, iDifference);
						
						System.out.println(" After 12months expiry alert for Process Measurement & Control");
						writer.write("After 12months expiry alert for Process Measurement & Control");
					}
				}
//			}
		}catch(Exception e) {e.printStackTrace();}	
	}
	
	public void sendMonthwiseNotificationToStakeholders(int iDifference, SimpleDateFormat formatter, SimpleDateFormat formatter2)
	{
		try
		{
			ModelObject[] alResponse = QueryManagement.queryObjects("HON Compliance Document", new String[] {"ID","In Process","Release Status","Gold Business Enterprise (GBE)"}, new String[] {"*","TRUE","Released","Process Measurement & Control (PMC)"}, writer);
			
			if(alResponse!=null) 
			{
				System.out.println("Total document found: "+alResponse.length);
				writer.write("\n\nTotal document found: "+alResponse.length);
				
				DataManagement.loadProperties(alResponse, new String[] {"item_id","object_string","owning_user","H4_hon_pmt_catalogs","H4_hon_hbt_catalogs","h4_expiry_date","h4_line_of_business","h4_strategic_business_grp"}, writer);
				//SimpleDateFormat formatter=new SimpleDateFormat("dd-MMM-yyyy");
	
				DataManagement.refreshObject(alResponse);
				for (ModelObject modelObject : alResponse) 
				{
					String sObjectString = modelObject.getPropertyDisplayableValue("object_string");
					String sOwningUser = modelObject.getPropertyDisplayableValue("owning_user");
					String sItemId = modelObject.getPropertyDisplayableValue("item_id");
					String sExpiryDate = modelObject.getPropertyDisplayableValue("h4_expiry_date");
					String sSBG = modelObject.getPropertyDisplayableValue("h4_strategic_business_grp");
						
					System.out.println("Item Id : "+sItemId+" , Owning User : "+sOwningUser+" , Exp Date : "+sExpiryDate+" , SBG : "+sSBG);
					writer.write("\n\nItem Id : "+sItemId+" , Owning User : "+sOwningUser+" , Exp Date : "+sExpiryDate+" ,  SBG : "+sSBG);
					
					List <String> sPMTCatelog = modelObject.getPropertyDisplayableValues("H4_hon_pmt_catalogs");
					System.out.print(" ,  Catalog Dumy: "+sPMTCatelog);
					writer.write(" ,  Catalog Dumy: "+sPMTCatelog+"\n");
					
					System.out.println(" Before monthwise expiry alert for Process Measurement & Control");
					writer.write("Before monthwise expiry alert for Process Measurement & Control");
					
					checkExpiryDateMonthwisePMT(sOwningUser, sExpiryDate, sPMTCatelog , formatter , formatter2 , modelObject, sObjectString, iDifference);
					
					System.out.println(" After monthwise expiry alert for Process Measurement & Control");
					writer.write("After monthwise expiry alert for Process Measurement & Control");
				}
			}
		}catch(Exception e) {e.printStackTrace();}
	}
	

	private void checkExpiryDateMonthwisePMT(String sOwningUser, String sExpiryDate , List <String> sPMTCatelog , SimpleDateFormat formatter , SimpleDateFormat formatter2 , ModelObject modelObject, String sObjectString, int iDifference)
	{
		try
		{
			outerloop:
			if((sExpiryDate!=null && sExpiryDate.trim().length()>0) && (sPMTCatelog!=null))
			{					
				Calendar cExpiry = Calendar.getInstance();
				Date dExpiry = formatter2.parse(sExpiryDate);
				cExpiry.setTime(dExpiry);
				cExpiry.add(Calendar.HOUR, 5);
				cExpiry.add(Calendar.MINUTE, 30);
				dExpiry = cExpiry.getTime();
				Date dExpiry2 = new Date();
				String sExpriyDate = formatter.format(dExpiry);
				Date dCurrent = new Date();
				String dCurrentFormatted = formatter.format(dCurrent);
				System.out.println("Expiry Date : "+sExpriyDate+"\t Current Date : "+dCurrentFormatted);
				writer.write("\nExpiry Date : "+sExpriyDate+"\t Current Date : "+dCurrentFormatted);
				
				cExpiry.setTime(dExpiry);
				sExpiryDate = formatter.format(dExpiry);
				if(sExpriyDate.trim().equalsIgnoreCase(dCurrentFormatted))
				{
					System.out.println("Expiry Date alert");
					writer.write("\nExpiry Date alert");
					loadCatalogNotifiersPMT(sOwningUser, modelObject, sPMTCatelog, "Certificate Expiring Today Alert", formatter, formatter2, sObjectString);
					break outerloop;
				}
				
				innerloop:
				if(dExpiry.before(dCurrent))
				{
					while(dExpiry.before(dCurrent))
					{
						for(int j=0; j<iDifference; j++) {
							cExpiry.add(Calendar.DATE, +j);
							dExpiry = cExpiry.getTime();
							cExpiry.add(Calendar.DATE, -j);
							sExpriyDate = formatter.format(dExpiry);
							
							if(sExpriyDate.trim().equalsIgnoreCase(dCurrentFormatted))
							{
								System.out.println("Expiry Date missed");
								writer.write("\nExpiry Date missed");
								loadCatalogNotifiersPMT(sOwningUser, modelObject, sPMTCatelog, "Certificate Past Expiry Alert", formatter, formatter2, sObjectString);
							}
						}
						cExpiry.add(Calendar.MONTH, 1);
						dExpiry = cExpiry.getTime();
					}
				}
				else
				{
					cExpiry.setTime(dExpiry);
					cExpiry.add(Calendar.MONTH, -9);
					
					for(int j=0; j<iDifference; j++) {
						cExpiry.add(Calendar.DATE, +j);
						dExpiry2 = cExpiry.getTime();
						cExpiry.add(Calendar.DATE, -j);
						sExpriyDate = formatter.format(dExpiry2);
						
						if(sExpriyDate.trim().equalsIgnoreCase(dCurrentFormatted))
						{
							System.out.println("9 month alert // Sending Notification");
							writer.write("\n9 month alert // Sending Notification");
							loadCatalogNotifiersPMT(sOwningUser, modelObject, sPMTCatelog, "Certificate Expiry 9 Months Alert", formatter, formatter2, sObjectString);
							break innerloop;
						}
					}
					for(int i=6; i>=0; i--)
					{
						cExpiry.setTime(dExpiry);
						cExpiry.add(Calendar.MONTH, -i);

						for(int j=0; j<iDifference; j++) {
							cExpiry.add(Calendar.DATE, +j);
							dExpiry2 = cExpiry.getTime();
							cExpiry.add(Calendar.DATE, -j);
							sExpriyDate = formatter.format(dExpiry2);
						
							if(sExpriyDate.trim().equalsIgnoreCase(dCurrentFormatted))
							{
								System.out.println("Expiry alert // Sending Notification");
								writer.write("\nExpiry alert // Sending Notification");
								if(i>1) {
									loadCatalogNotifiersPMT(sOwningUser, modelObject, sPMTCatelog, "Certificate Expiry "+i+" Months Alert", formatter, formatter2, sObjectString);
								}
								else
								{
									loadCatalogNotifiersPMT(sOwningUser, modelObject, sPMTCatelog, "Certificate Expiry 1 Month Alert", formatter, formatter2, sObjectString);
								}
								break innerloop;
							}
						}
					}
				}
			}
			else {
				System.out.println("Expiry Date or sPMTCatelog Invalid");
				writer.write("Expiry Date or sPMTCatelog Invalid");;
			}
		}catch(Exception e) {e.printStackTrace();}	
		
	}

	private void checkExpiryDate12MonthsPMT(String sExpiryDate , List <String> sPMTCatelog , SimpleDateFormat formatter , SimpleDateFormat formatter2 , ModelObject modelObject, int iDifference)
	{
		try
		{
			if(sExpiryDate!=null && sExpiryDate.trim().length()>0) 
			{	
				innerloop:				
				if(sPMTCatelog!=null)
				{
					Calendar cExpiry = Calendar.getInstance();
					Date dExpiry = formatter2.parse(sExpiryDate);
					Date dExpiry2 = new Date();
					cExpiry.setTime(dExpiry);
					cExpiry.add(Calendar.HOUR, 5);
					cExpiry.add(Calendar.MINUTE, 30);
					dExpiry = cExpiry.getTime();
					String sExpriyDate = formatter.format(dExpiry);
					Date dCurrent = new Date();
					String dCurrentFormatted = formatter.format(dCurrent);
					System.out.println("Expiry Date : "+sExpriyDate+"\t Current Date : "+dCurrentFormatted);
					writer.write("\nExpiry Date : "+sExpriyDate+"\t Current Date : "+dCurrentFormatted);
					
					cExpiry.add(Calendar.YEAR, -1);
					dExpiry = cExpiry.getTime();
					for(int j=0; j<iDifference; j++) {
						cExpiry.setTime(dExpiry);
						cExpiry.add(Calendar.DATE, +j);
						dExpiry2 = cExpiry.getTime();	
						cExpiry.add(Calendar.DATE, -j);
						sExpriyDate = formatter.format(dExpiry2);
						
						System.out.println("Expiry Date considering 12 months criteria : "+sExpriyDate+"\t Current Date : "+dCurrentFormatted);
						
						if(sExpriyDate.trim().equalsIgnoreCase(dCurrentFormatted))
						{
							System.out.println("\n 12 months alert // Initiating WF");
							writer.write("\n 12 months alert // Initiating WF");
							InitiateWorkflow(modelObject, writer);
							break innerloop;
						}
					}
					//dExpiry = cExpiry.getTime();
				}
			}
		}catch(Exception e) {e.printStackTrace();}	
	}
	
	private void loadCatalogNotifiersPMT(String sOwningUser, ModelObject modelObject, List <String> sPMTCatelog, String sMailSubject, SimpleDateFormat formatter, SimpleDateFormat formatter2, String sObjectString)
	{
		try {
			Set<String> hsEmaillistUser=new HashSet<>();
			String sTotalEmailToList="";
			String sCatalogList = " ";
			int iCount = 0;
			
		for (String modelObject2 : sPMTCatelog) {
			ModelObject[] catalogResponse = QueryManagement.queryObjects("HON IA Catalog", new String[] {"Catalog Name"}, new String[] {modelObject2}, writer);

		if(catalogResponse!=null)
		{
				System.out.println("Catalog response length : "+catalogResponse.length);
				writer.write("\nCatalog response length : "+catalogResponse.length);
				DataManagement.loadProperties(catalogResponse, new String[] {"object_name","h4_notifier1","h4_notifier2","h4_notifier3"}, writer);	
				
				for (int iPmt=0;iPmt<catalogResponse.length;iPmt++) {
					System.out.println("Got IA Catalog Response");
					writer.write("\nGot IA Catalog Response");
				
					String object_name = catalogResponse[0].getPropertyDisplayableValue("object_name");
					String h4_Notifier1 = catalogResponse[0].getPropertyDisplayableValue("h4_notifier1");
					String h4_Notifier2 = catalogResponse[0].getPropertyDisplayableValue("h4_notifier2");
					String h4_Notifier3 = catalogResponse[0].getPropertyDisplayableValue("h4_notifier3");
					
					System.out.println("Catalog Name : "+object_name+"\nESS Notifier 1 : "+h4_Notifier1+"\nESS Notifier 2 : "+h4_Notifier2+"\nESS Notifier 3 : "+h4_Notifier3);
					writer.write("Catalog Name : "+object_name+"\nESS Notifier 1 : "+h4_Notifier1+"\nESS Notifier 2 : "+h4_Notifier2+"\nESS Notifier 3 : "+h4_Notifier3);
					
					if (iCount==0) {
						
						sCatalogList=""+object_name;
					}
					else {
						sCatalogList+=", "+object_name;
					}
					iCount++;	
					
					if(sOwningUser.equals("")==false)
					{
						User OwningUser = QueryManagement.findUser(sOwningUser);
						String mPersonEmailOwningUser=getEmails(OwningUser);
						System.out.println("Owning User : "+OwningUser.get_user_name()+"\nOwning User Email id : "+mPersonEmailOwningUser);
						writer.write("\nOwning User : "+OwningUser.get_user_name()+"\nOwning User Email id : "+mPersonEmailOwningUser);
						
						if(mPersonEmailOwningUser!=null)
						{
							hsEmaillistUser.add(mPersonEmailOwningUser);
						}
					}
					
					if(h4_Notifier1.equals("")==false)
					{
						User user1 = QueryManagement.findUser_fromName(h4_Notifier1);
						String mPersonEmail1=getEmails(user1);
						System.out.println("Notifier1 Name : "+user1.get_user_name()+"\nNotifier1 Email id : "+mPersonEmail1);
						writer.write("\nNotifier1 Name : "+user1.get_user_name()+"\nNotifier1 Email id : "+mPersonEmail1);
						
						if(mPersonEmail1!=null)
						{
							hsEmaillistUser.add(mPersonEmail1);
						}
					}
					else
					{
						System.out.println("Notifier1 is not present");
						writer.write("\nNotifier1 is not present");
					}
					
					if(h4_Notifier2.equals("")==false)
					{
						User user2 = QueryManagement.findUser_fromName(h4_Notifier2);
						String mPersonEmail2=getEmails(user2);
						System.out.println("Notifier2 Name : "+user2.get_user_name()+"\nNotifier2 Email id : "+mPersonEmail2);
						writer.write("\nNotifier2 Name : "+user2.get_user_name()+"\nNotifier2 Email id : "+mPersonEmail2);
						
						if(mPersonEmail2!=null)
						{
							hsEmaillistUser.add(mPersonEmail2);
						}
					}
					else
					{
						System.out.println("Notifier2 is not present");
						writer.write("\nNotifier2 is not present");
					}
					
					if(h4_Notifier3.equals("")==false)
					{
						User user3 = QueryManagement.findUser_fromName(h4_Notifier3);
						String mPersonEmail3=getEmails(user3);
						System.out.println("Notifier3 Name : "+user3.get_user_name()+"\nNotifier3 Email id : "+mPersonEmail3);
						writer.write("\nNotifier3 Name : "+user3.get_user_name()+"\nNotifier3 Email id : "+mPersonEmail3);
						
						if(mPersonEmail3!=null)
						{
							hsEmaillistUser.add(mPersonEmail3);
						}
					}
					else
					{
						System.out.println("Notifier3 is not present");
						writer.write("\nNotifier3 is not present");
					}
					
					if (hsEmaillistUser.size()>0) {
						int iCount2 = 0;
						for (String sMailID : hsEmaillistUser) {
							if (sMailID.trim().length()>0 &&sMailID!=null) {
								
								if (iCount2==0) {
									
									sTotalEmailToList=""+sMailID;
								}
								else {
									sTotalEmailToList+=","+sMailID;
								}
								iCount2++;
							}												
						}
					}
				}
			}
		}
		if (sTotalEmailToList.trim().length()>0&&sTotalEmailToList!=null) 
		{
			System.out.println("Catalog List: "+sCatalogList);
			writer.write("Catalog List: "+sCatalogList);
			SendNofiticationAndUpdatedocumentPMT(modelObject,sTotalEmailToList,sCatalogList,writer, sMailSubject, formatter, formatter2, sObjectString);
		}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private void InitiateWorkflow(ModelObject modelObject, FileWriter writer) {
		try {
		WorkflowService wfService = WorkflowService.getService(AppXSession.getConnection());
		DataManagementService dmService1= DataManagementService.getService(AppXSession.getConnection());
		ContextData contextData = new ContextData();
		String observerKey = "";
		String name = "Certificate Alert";
		String subject = "createInstance";
		String description = "Certificate Expiry Alert Description";
		
		contextData.processTemplate = "HON Certificate Expiry Alert Workflow IA";
		contextData.subscribeToEvents = false;
		contextData.subscriptionEventCount = 0;
		contextData.attachmentCount = 1;
		contextData.attachments = new String[]{ modelObject.getUid() };
		contextData.attachmentTypes = new int[]{1};
		
		InstanceInfo instanceInfo = wfService.createInstance(true,observerKey, name, subject, description, contextData);
		System.out.println("Inbox sent...");
		writer.write("\nInbox sent...");
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	private void SendNofiticationAndUpdatedocumentPMT(ModelObject modelObject, String sTotalEmailToList, String sCatalogList, FileWriter writer, String sMailSubject, SimpleDateFormat formatter, SimpleDateFormat formatter2, String sObjectString) {
		try
		{
			String DocumentItemId = modelObject.getPropertyDisplayableValue(CertificateExpiraryConstants.PROP_ITEM_ID);
			String[] sExpDate = modelObject.getPropertyDisplayableValue(CertificateExpiraryConstants.PROP_EXP_DATE).split("\\s+");
			Calendar cExpiry = Calendar.getInstance();
			Date dExpiry = formatter2.parse(sExpDate[0]+" "+sExpDate[1]);
			cExpiry.setTime(dExpiry);
			cExpiry.add(Calendar.HOUR, 5);
			cExpiry.add(Calendar.MINUTE, 30);
			dExpiry = cExpiry.getTime();
			sExpDate[0] = formatter.format(dExpiry);
			String MAIL_SUBJECT = sMailSubject;
			
			String sBodyContent = CertificateExpiraryUtil.getEmailBodyContentPMT(sExpDate[0], sObjectString, properties.getProperty("MAIL_BODY_CONTENT"), sCatalogList);
			SendNotification.sendNotifications(sTotalEmailToList, "", properties.getProperty("MAIL_FROM"),
					properties.getProperty("MAIL_HOST"), MAIL_SUBJECT, sBodyContent,
					properties.getProperty("MAIL_FOOTER_CONTENT"));
			writer.write("\nINFO: Mail is sent to above notifiers");
		} catch (Exception e) {
			e.printStackTrace();
			SendNotification.sendNotifications(properties.getProperty("MAIL_SUPPORT_TEAM_EMAIL"), "",
					properties.getProperty("MAIL_FROM"), properties.getProperty("MAIL_HOST"),
					"ERROR : Task Escalation utility failed.", e.getMessage(),
					properties.getProperty("MAIL_FOOTER_CONTENT"));
		}
		return;
	}
	
	private void SendNofiticationAndUpdatedocumentPMTcustom(ModelObject modelObject, String sTotalEmailToList, String sCatalogList, FileWriter writer, String sMailSubject, SimpleDateFormat formatter, SimpleDateFormat formatter2, String sObjectString) {
		try
		{
			String DocumentItemId = modelObject.getPropertyDisplayableValue(CertificateExpiraryConstants.PROP_ITEM_ID);
			String[] sExpDate = modelObject.getPropertyDisplayableValue(CertificateExpiraryConstants.PROP_EXP_DATE).split("\\s+");
			Calendar cExpiry = Calendar.getInstance();
			Date dExpiry = formatter2.parse(sExpDate[0]+" "+sExpDate[1]);
			cExpiry.setTime(dExpiry);
			cExpiry.add(Calendar.HOUR, 5);
			cExpiry.add(Calendar.MINUTE, 30);
			dExpiry = cExpiry.getTime();
			sExpDate[0] = formatter.format(dExpiry);
			String MAIL_SUBJECT = sMailSubject;
			
			String sBodyContent = CertificateExpiraryUtil.getEmailBodyContentPMT(sExpDate[0], sObjectString, properties.getProperty("MAIL_BODY_CONTENT"), sCatalogList);
			SendNotification.sendNotifications(sTotalEmailToList, "", properties.getProperty("MAIL_FROM"),
					properties.getProperty("MAIL_HOST"), MAIL_SUBJECT, sBodyContent,
					properties.getProperty("MAIL_FOOTER_CONTENT"));
			writer.write("\nINFO: Mail is sent to above notifiers");
		} catch (Exception e) {
			e.printStackTrace();
			SendNotification.sendNotifications(properties.getProperty("MAIL_SUPPORT_TEAM_EMAIL"), "",
					properties.getProperty("MAIL_FROM"), properties.getProperty("MAIL_HOST"),
					"ERROR : Task Escalation utility failed.", e.getMessage(),
					properties.getProperty("MAIL_FOOTER_CONTENT"));
		}
		return;
	}
	
	private String getEmails(User user) {
		String mPersonEmail=null;
			try {
				System.out.println(user.get_person().toString());
				Person person = user.get_person();
				ModelObject[] mPersonobj = { person };
				DataManagement.loadProperties(mPersonobj, new String[] { "PA9" }, writer);
				mPersonEmail = person.get_PA9();
			} catch (NotLoadedException e) {
				e.printStackTrace();
			}
		return mPersonEmail;
	}
	
}
