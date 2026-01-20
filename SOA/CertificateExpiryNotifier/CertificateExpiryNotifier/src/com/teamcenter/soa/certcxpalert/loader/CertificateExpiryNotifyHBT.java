package com.teamcenter.soa.certcxpalert.loader;

import java.io.FileWriter;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Properties;
import java.util.Set;
import java.util.concurrent.TimeUnit;

import com.sun.mail.imap.protocol.UID;
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
import com.teamcenter.soa.certcxpalert.loader.CertificateExpiraryConstants;
import com.teamcenter.soa.certcxpalert.loader.CertificateExpiraryUtil;

public class CertificateExpiryNotifyHBT {

	private FileWriter writer;
	public Properties properties = null;
	HashMap<String, String> hmEmailIdItemIdMap = new HashMap<String, String>();
	HashMap<String, String> hmItemIdCatalogMap = new HashMap<String, String>();
	HashMap<String, String> hmItemIdExpiryMap = new HashMap<String, String>();
	HashMap<String, String> hmItemIdOwningUserMap = new HashMap<String, String>();
	HashMap<String, String> hmItemIdObjectStringMap = new HashMap<String, String>();

	public CertificateExpiryNotifyHBT(FileWriter writer, Properties properties) {
		this.writer = writer;
		this.properties = properties;
	}
	
	public void sendMainNotificationToStakeholdersHBT(String sLastExecutionDate)
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
			if(iDifference >= 7)
			{
				System.out.println("****** Before executing monthwise alert notification ********************* ");
				sendMonthwiseNotificationToStakeholders(iDifference, formatter, formatter2);
				
				System.out.println("****** Before executing main notification (Before 6 month) ********************* ");


				ModelObject[] alResponse = QueryManagement.queryObjects("HON Compliance Document", new String[] {"ID","In Process","Release Status","Strategic Business Group (SBG)"}, new String[] {"*","FALSE","Released","BA"}, writer);
				
				if(alResponse!=null) 
				{
					System.out.println("Total document found: "+alResponse.length);
					writer.write("\n\nTotal document found: "+alResponse.length);
					
					DataManagement.loadProperties(alResponse, new String[] {"item_id", "object_string", "H4_hon_pmt_catalogs", "H4_hon_hbt_catalogs","h4_expiry_date","h4_line_of_business","h4_strategic_business_grp"}, writer);
					
					DataManagement.refreshObject(alResponse);
					for (ModelObject modelObject : alResponse) 
					{
						String sObjectString = modelObject.getPropertyDisplayableValue("object_string");
						String sItemId = modelObject.getPropertyDisplayableValue("item_id");
						String sExpiryDate = modelObject.getPropertyDisplayableValue("h4_expiry_date");
						String sSBG = modelObject.getPropertyDisplayableValue("h4_strategic_business_grp");
						
						System.out.println("Item Id : "+sItemId+" , Exp Date : "+sExpiryDate+" , SBG : "+sSBG);
						writer.write("\n\nItem Id : "+sItemId+" , Exp Date : "+sExpiryDate+" , SBG : "+sSBG);
						
						List <String> sHBTCatelog = modelObject.getPropertyDisplayableValues("H4_hon_hbt_catalogs");
						System.out.print(" ,  Catalog Dumy: "+sHBTCatelog);
						writer.write(" ,  Catalog Dumy: "+sHBTCatelog+"\n");
						
						System.out.println(" Before 6 months expiry alert for BA");
						writer.write("Before 6 months expiry alert for BA");
						
						checkExpiryDate6MonthsHBT(sItemId, sExpiryDate, sHBTCatelog , formatter , formatter2 , modelObject, iDifference, sObjectString);
						
						System.out.println(" After 6 months expiry alert for BA");
						writer.write("After 6 months expiry alert for BA");
					}
				}
				SendNofiticationAndUpdatedocumentHBT();
			}
		}catch(Exception e) {e.printStackTrace();}	
	}
	
	public void sendMonthwiseNotificationToStakeholders(int iDifference, SimpleDateFormat formatter, SimpleDateFormat formatter2)
	{
		try
		{
			ModelObject[] alResponse = QueryManagement.queryObjects("HON Compliance Document", new String[] {"ID","In Process","Release Status","Strategic Business Group (SBG)"}, new String[] {"*","TRUE","Released","BA"}, writer);

			//ModelObject[] alResponse = QueryManagement.queryObjects("HON Compliance Document", new String[] {"ID","In Process","Release Status","Strategic Business Group (SBG)"}, new String[] {"3015-0812-001","TRUE","Released","BA"}, writer);

			if(alResponse!=null) 
			{
				System.out.println("Total document found: "+alResponse.length);
				writer.write("\n\nTotal document found: "+alResponse.length);
				
				DataManagement.loadProperties(alResponse, new String[] {"item_id","object_string","owning_user","H4_hon_pmt_catalogs","H4_hon_hbt_catalogs","h4_expiry_date","h4_line_of_business","h4_strategic_business_grp"}, writer);
				//SimpleDateFormat formatter=new SimpleDateFormat("dd-MMM-yyyy");
	
				DataManagement.refreshObject(alResponse);
				for (ModelObject modelObject : alResponse) 
				{
					String sOwningUser = modelObject.getPropertyDisplayableValue("owning_user");
					String sItemId = modelObject.getPropertyDisplayableValue("item_id");
					String sExpiryDate = modelObject.getPropertyDisplayableValue("h4_expiry_date");
					String sSBG = modelObject.getPropertyDisplayableValue("h4_strategic_business_grp");
					String sObjectString = modelObject.getPropertyDisplayableValue("object_string");
						
					System.out.println("Item Id : "+sItemId+" , Owning User : "+sOwningUser+" , Exp Date : "+sExpiryDate+" , SBG : "+sSBG+" , Object String : "+sObjectString);
					writer.write("\n\nItem Id : "+sItemId+" , Owning User : "+sOwningUser+" , Exp Date : "+sExpiryDate+" ,  SBG : "+sSBG+" , Object String : "+sObjectString);

					List <String> sHBTCatelog = modelObject.getPropertyDisplayableValues("H4_hon_hbt_catalogs");
					System.out.print(" ,  Catalog Dumy: "+sHBTCatelog);
					writer.write(" ,  Catalog Dumy: "+sHBTCatelog+"\n");
					
					System.out.println("monthly expiry alert for HBT");
					writer.write("monthly expiry alert for HBT");

					checkExpiryDateMonthwiseHBT(sOwningUser, sItemId, sExpiryDate, sHBTCatelog , formatter , formatter2 , modelObject, iDifference, sObjectString);
				}
			}
		}catch(Exception e) {e.printStackTrace();}
	}
	
	private void checkExpiryDateMonthwiseHBT(String sOwningUser, String sItemId, String sExpiryDate , List <String> sHBTCatelog , SimpleDateFormat formatter , SimpleDateFormat formatter2 , ModelObject modelObject, int iDifference, String sObjectString)
	{
		try
		{
			
			outerloop:
			if((sExpiryDate!=null && sExpiryDate.trim().length()>0) && (sHBTCatelog!=null))
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
					loadCatalogNotifiersHBT(sOwningUser, modelObject, sHBTCatelog, sItemId, sExpiryDate, sObjectString);
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
							//System.out.println("Expiry Date : "+sExpriyDate+"\t Current Date : "+dCurrentFormatted);
							
							if(sExpriyDate.trim().equalsIgnoreCase(dCurrentFormatted))
							{
								System.out.println("Expiry Date missed");
								writer.write("\nExpiry Date missed");
								loadCatalogNotifiersHBT(sOwningUser, modelObject, sHBTCatelog, sItemId, sExpiryDate, sObjectString);
							}
						}
						cExpiry.add(Calendar.MONTH, 1);
						dExpiry = cExpiry.getTime();
					}
				}
				else
				{					
					for(int i=0; i<6; i++)
					{
						for(int j=0; j<iDifference; j++) {
							cExpiry.setTime(dExpiry);
							cExpiry.add(Calendar.MONTH, -i);
							cExpiry.add(Calendar.DATE, +j);
							dExpiry2  = cExpiry.getTime();	
							sExpriyDate = formatter.format(dExpiry2);
							System.out.println("Expiry Date : "+sExpriyDate+"\t Current Date : "+dCurrentFormatted);
							if(sExpriyDate.trim().equalsIgnoreCase(dCurrentFormatted))
							{
								System.out.println("Expiry alert // Sending Notification");
								writer.write("\nExpiry alert // Sending Notification");
								loadCatalogNotifiersHBT(sOwningUser, modelObject, sHBTCatelog, sItemId, sExpiryDate, sObjectString);
								break innerloop;
							}
						}
					}
				}
			}
			else {
				System.out.println("Expiry Date or sHBTCatelog Invalid");
				writer.write("Expiry Date or sHBTCatelog Invalid");;
			}
		}catch(Exception e) {e.printStackTrace();}	
		
	}

	private void checkExpiryDate6MonthsHBT(String sItemId, String sExpiryDate , List <String> sHBTCatelog , SimpleDateFormat formatter , SimpleDateFormat formatter2 , ModelObject modelObject, int iDifference, String sObjectString)
	{
		try
		{
			if(sExpiryDate!=null && sExpiryDate.trim().length()>0) 
			{	
				innerloop:				
				if(sHBTCatelog!=null)
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
					for(int j=0; j<iDifference; j++) {
						cExpiry.setTime(dExpiry);
						cExpiry.add(Calendar.MONTH, -6);
						cExpiry.add(Calendar.DATE, +j);
						dExpiry2  = cExpiry.getTime();	
						sExpriyDate = formatter.format(dExpiry2);
						
						System.out.println("Expiry Date considering 6 month criteria : "+sExpriyDate+"\t Current Date : "+dCurrentFormatted);
								
						if(sExpriyDate.trim().equalsIgnoreCase(dCurrentFormatted))
						{
							System.out.println("6 month alert // Initiating WF");
							writer.write("\n6 month alert // Initiating WF");
							InitiateWorkflow(modelObject, writer, sItemId, sExpiryDate, sHBTCatelog, formatter, formatter2, sObjectString);
							break innerloop;
						}
					}
				}
			}
		}catch(Exception e) {e.printStackTrace();}	
	}

	private void loadCatalogNotifiersHBT(String sOwningUser, ModelObject modelObject, List <String> sHBTCatelog, String sItemId, String sExpiryDate, String sObjectString)
	{
		try {
			Set<String> hsEmaillistUser=new HashSet<>();
			String sTotalEmailToList="";
			String sCatalogList = " ";
			int iCount = 0;
			
		for (String modelObject2 : sHBTCatelog) {
			ModelObject[] catalogResponse = QueryManagement.queryObjects("HON BA Catalog", new String[] {"Catalog Name"}, new String[] {modelObject2}, writer);

		if(catalogResponse!=null)
		{
				System.out.println("Catalog response length : "+catalogResponse.length);
				writer.write("\nCatalog response length : "+catalogResponse.length);
				DataManagement.loadProperties(catalogResponse, new String[] {"object_name","h4_notifier1","h4_notifier2","h4_notifier3"}, writer);	
				
				for (int iHbt=0;iHbt<catalogResponse.length;iHbt++) {
					System.out.println("Got BA Catalog Response");
					writer.write("\nGot BA Catalog Response");
				
					String mPersonEmail1=null;
					String object_name = catalogResponse[0].getPropertyDisplayableValue("object_name");
					List <String>  h4_Notifier1 = catalogResponse[0].getPropertyDisplayableValues("h4_notifier1");
					
//					String[]  h4_Notifier1 = catalogResponse[0].getPropertyDisplayableValues("h4_notifier1");
					ModelObject[] h4_Notifier2 = catalogResponse[0].getPropertyObject("h4_notifier2").getModelObjectArrayValue();
					ModelObject[] h4_Notifier3 = catalogResponse[0].getPropertyObject("h4_notifier3").getModelObjectArrayValue();
					
					System.out.println("BA Catalog Name : "+object_name+"\n BA Notifier 1 : "+h4_Notifier1+"\n BA Notifier 2 : "+h4_Notifier2+"\n BA Notifier 3 : "+h4_Notifier3);
					writer.write("\n BA Catalog Name : "+object_name+"\n BA Notifier 1 : "+h4_Notifier1+"\n BA Notifier 2 : "+h4_Notifier2+"\n BA Notifier 3 : "+h4_Notifier3);
					
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
							hmItemIdOwningUserMap.put(sItemId, mPersonEmailOwningUser);
							hsEmaillistUser.add(mPersonEmailOwningUser);
						}
					}
					
					if(h4_Notifier1!=null)
					{
						for (String sEmailIDTemp : h4_Notifier1) {

							System.out.println("Notifier1 Distribution List : "+sEmailIDTemp);
							writer.write("\nNotifier1 Distribution List :"+sEmailIDTemp);
							if(!sEmailIDTemp.isEmpty())
							{
								if(sEmailIDTemp!=null)
								{
									hsEmaillistUser.add(sEmailIDTemp);
								}
							}
						}
					}
					else
					{
						System.out.println("Notifier1 Distribution List is not present");
						writer.write("\nNotifier1 Distribution List is not present");
					}
					
					if(h4_Notifier2!=null)
					{
						for (ModelObject modelObjectTemp : h4_Notifier2) {
							
							DataManagement.loadProperties(new ModelObject[] {modelObjectTemp}, new String[] {"User"}, writer);
							ModelObject mChangeSpecUser = modelObjectTemp.getPropertyObject("person").getModelObjectValue();
							DataManagement.loadProperties(new ModelObject[] {mChangeSpecUser}, new String[] { "PA9" }, writer);
							
							Person person = (Person) mChangeSpecUser;
							String sEmailID  = person.get_PA9();
							System.out.println("Notifier2 Email ID : "+sEmailID);
							writer.write("\nNotifier2 Email ID :"+sEmailID);
							if(!sEmailID.isEmpty())
							{
								if(sEmailID!=mPersonEmail1)
								{
									hsEmaillistUser.add(sEmailID);
								}
							}
						}
					}
					else
					{
						System.out.println("Notifier2 is not present");
						writer.write("\nNotifier2 is not present");
					}
					
					if(h4_Notifier3!=null)
					{
						for (ModelObject modelObjectTemp : h4_Notifier3) {
							
							DataManagement.loadProperties(new ModelObject[] {modelObjectTemp}, new String[] {"User"}, writer);
							ModelObject mChangeSpecUser = modelObjectTemp.getPropertyObject("person").getModelObjectValue();
							DataManagement.loadProperties(new ModelObject[] {mChangeSpecUser}, new String[] { "PA9" }, writer);
							
							Person person = (Person) mChangeSpecUser;
							String sEmailID  = person.get_PA9();
							System.out.println("Notifier3 Email ID : "+sEmailID);
							writer.write("\nNotifier3 Email ID :"+sEmailID);
							if(!sEmailID.isEmpty())
							{
								if(sEmailID!=mPersonEmail1)
								{
									hsEmaillistUser.add(sEmailID);
								}
							}
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
			hmItemIdCatalogMap.put(sItemId, sCatalogList);
			hmItemIdExpiryMap.put(sItemId,  sExpiryDate);
			hmItemIdObjectStringMap.put(sItemId, sObjectString);
			HBTEmailHashMapCreatingFunction(sItemId, sExpiryDate, sTotalEmailToList);
		}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private void HBTEmailHashMapCreatingFunction(String sItemId, String sExpiryDate, String sTotalEmailToList)
	{
		String[] sEmails = sTotalEmailToList.split(",");
		for(String sEmailsTemp : sEmails)
		{
			if(hmEmailIdItemIdMap.containsKey(sEmailsTemp))
			{
				String sOldValue = hmEmailIdItemIdMap.get(sEmailsTemp);
				String sNewValue = sOldValue + "," + sItemId;
				hmEmailIdItemIdMap.put(sEmailsTemp, sNewValue);
			}
			else
			{
				hmEmailIdItemIdMap.put(sEmailsTemp, sItemId);
			}
		}
	}
	
	private void InitiateWorkflow(ModelObject modelObject, FileWriter writer, String sItemId, String sExpiryDate, List <String> sHBTCatelog,  SimpleDateFormat formatter, SimpleDateFormat formatter2, String sObjectString) {
		try {
		WorkflowService wfService = WorkflowService.getService(AppXSession.getConnection());
		DataManagementService dmService1= DataManagementService.getService(AppXSession.getConnection());
		ContextData contextData = new ContextData();
		String observerKey = "";
		String name = "Certificate Alert";
		String subject = "createInstance";
		String description = "Certificate Expiry Alert Description";
		
		contextData.processTemplate = "HON Certificate Expiry Alert Workflow BA";
		contextData.subscribeToEvents = false;
		contextData.subscriptionEventCount = 0;
		contextData.attachmentCount = 1;
		contextData.attachments = new String[]{ modelObject.getUid() };
		contextData.attachmentTypes = new int[]{1};
		
		InstanceInfo instanceInfo = wfService.createInstance(true,observerKey, name, subject, description, contextData);
		//function to send email to DL
		Set<String> hsEmaillistUser=new HashSet<>();
		String sTotalEmailToList="";
		String sCatalogList = " ";
		int iCount = 0;
		
		for (String modelObject2 : sHBTCatelog) {
			ModelObject[] catalogResponse = QueryManagement.queryObjects("HON BA Catalog", new String[] {"Catalog Name"}, new String[] {modelObject2}, writer);

			if(catalogResponse!=null)
			{
					DataManagement.loadProperties(catalogResponse, new String[] {"object_name","h4_notifier1"}, writer);	
					for (int iHbt=0;iHbt<catalogResponse.length;iHbt++) {
						
						String mPersonEmail1=null;
						String object_name = catalogResponse[0].getPropertyDisplayableValue("object_name");
						List <String>  h4_Notifier1 = catalogResponse[0].getPropertyDisplayableValues("h4_notifier1");
//						ModelObject[] h4_Notifier2 = catalogResponse[0].getPropertyObject("h4_notifier2").getModelObjectArrayValue();
//						ModelObject[] h4_Notifier3 = catalogResponse[0].getPropertyObject("h4_notifier3").getModelObjectArrayValue();
						
						if (iCount==0) {
							sCatalogList=""+object_name;
						}
						else {
							sCatalogList+=", "+object_name;
						}
						iCount++;	
						
						if(h4_Notifier1!=null)
						{
							for (String sEmailIDTemp : h4_Notifier1) {
	
								System.out.println("Notifier1 Distribution List : "+sEmailIDTemp);
								writer.write("\nNotifier1 Distribution List :"+sEmailIDTemp);
								if(!sEmailIDTemp.isEmpty())
								{
									if(sEmailIDTemp!=null)
									{
										hsEmaillistUser.add(sEmailIDTemp);
									}
								}
							}
						}
						else
						{
							System.out.println("Notifier1 Distribution List is not present");
							writer.write("\nNotifier1 Distribution List is not present");
						}
						
//						if(h4_Notifier2!=null)
//						{
//							for (ModelObject modelObjectTemp : h4_Notifier2) {
//								
//								DataManagement.loadProperties(new ModelObject[] {modelObjectTemp}, new String[] {"User"}, writer);
//								ModelObject mChangeSpecUser = modelObjectTemp.getPropertyObject("person").getModelObjectValue();
//								DataManagement.loadProperties(new ModelObject[] {mChangeSpecUser}, new String[] { "PA9" }, writer);
//								
//								Person person = (Person) mChangeSpecUser;
//								String sEmailID  = person.get_PA9();
//								System.out.println("Notifier2 Email ID : "+sEmailID);
//								writer.write("\nNotifier2 Email ID :"+sEmailID);
//								if(!sEmailID.isEmpty())
//								{
//									if(sEmailID!=mPersonEmail1)
//									{
//										hsEmaillistUser.add(sEmailID);
//									}
//								}
//							}
//						}
//						else
//						{
//							System.out.println("Notifier2 is not present");
//							writer.write("\nNotifier2 is not present");
//						}
//						
//						if(h4_Notifier3!=null)
//						{
//							for (ModelObject modelObjectTemp : h4_Notifier3) {
//								
//								DataManagement.loadProperties(new ModelObject[] {modelObjectTemp}, new String[] {"User"}, writer);
//								ModelObject mChangeSpecUser = modelObjectTemp.getPropertyObject("person").getModelObjectValue();
//								DataManagement.loadProperties(new ModelObject[] {mChangeSpecUser}, new String[] { "PA9" }, writer);
//								
//								Person person = (Person) mChangeSpecUser;
//								String sEmailID  = person.get_PA9();
//								System.out.println("Notifier3 Email ID : "+sEmailID);
//								writer.write("\nNotifier3 Email ID :"+sEmailID);
//								if(!sEmailID.isEmpty())
//								{
//									if(sEmailID!=mPersonEmail1)
//									{
//										hsEmaillistUser.add(sEmailID);
//									}
//								}
//							}
//						}
//						else
//						{
//							System.out.println("Notifier3 is not present");
//							writer.write("\nNotifier3 is not present");
//						}
						
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
		SendNofiticationAndUpdatedocumentHBTDL(modelObject, sTotalEmailToList, sCatalogList, writer, formatter, formatter2, sObjectString);
		System.out.println("Inbox sent...");
		writer.write("\nInbox sent...");
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	private void SendNofiticationAndUpdatedocumentHBT() {
		try
		{
			for(String sEmailID: hmEmailIdItemIdMap.keySet()) {
				
				String[] sItemIDs = hmEmailIdItemIdMap.get(sEmailID).split(",");
				String MAIL_SUBJECT = "Certificate Expiry Monthly Alert";
				String sBodyContent = CertificateExpiraryUtil.getEmailBodyContentHBT(sEmailID, sItemIDs, hmItemIdCatalogMap, hmItemIdExpiryMap, hmItemIdOwningUserMap, hmItemIdObjectStringMap, properties.getProperty("MAIL_BODY_CONTENT"));
				SendNotification.sendNotifications(sEmailID, "", properties.getProperty("MAIL_FROM"),
						properties.getProperty("MAIL_HOST"), MAIL_SUBJECT, sBodyContent,
						properties.getProperty("MAIL_FOOTER_CONTENT"));
				writer.write("\nINFO: Mail is sent to above notifiers");
			}
		} catch (Exception e) {
			e.printStackTrace();
			SendNotification.sendNotifications(properties.getProperty("MAIL_SUPPORT_TEAM_EMAIL"), "",
					properties.getProperty("MAIL_FROM"), properties.getProperty("MAIL_HOST"),
					"ERROR : Task Escalation utility failed.", e.getMessage(),
					properties.getProperty("MAIL_FOOTER_CONTENT"));
		}
		return;
	}

	private void SendNofiticationAndUpdatedocumentHBTDL(ModelObject modelObject, String sTotalEmailToList, String sCatalogList, FileWriter writer, SimpleDateFormat formatter, SimpleDateFormat formatter2 , String sObjectString) {
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
			String MAIL_SUBJECT = "Certificate Expiry 6 Month Alert Notification";
			
			String sBodyContent = CertificateExpiraryUtil.getEmailBodyContentHBTDL(sExpDate[0], DocumentItemId, properties.getProperty("MAIL_BODY_CONTENT"), sCatalogList, sObjectString);
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
