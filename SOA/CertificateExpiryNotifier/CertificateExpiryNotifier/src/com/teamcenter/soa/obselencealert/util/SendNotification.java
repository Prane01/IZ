package com.teamcenter.soa.obselencealert.util;
import java.io.File;
import java.util.*;  
import javax.mail.*;  
import javax.mail.internet.*;  
//import javax.activation.*;  
  
public class SendNotification  
{  
	public static void sendNotifications(String toAddress,String ccAddresses,
			String fromAddress, String sHost, String sMailSubject, String sBodyContent, String sFooterContent)
	{
		try
		{
		    //Get the session object  
		      Properties properties = System.getProperties();  
		      properties.setProperty("mail.smtp.host", sHost);  
		      Session session = Session.getDefaultInstance(properties);  
		  
		     //compose the message  
		      try{  
		         MimeMessage message = new MimeMessage(session);  
		         message.setFrom(new InternetAddress(fromAddress)); 
		         if(toAddress.contains(","))
		        	 for(String sUserMailId : toAddress.split(","))
		        		 message.addRecipient(Message.RecipientType.TO,new InternetAddress(sUserMailId));  
		         else
		        	 message.addRecipient(Message.RecipientType.TO,new InternetAddress(toAddress));
		         
		         if (!ccAddresses.equals("") ) {
		        	 message.addRecipients(Message.RecipientType.CC, InternetAddress.parse(ccAddresses));
		         }
		         message.setSubject(sMailSubject);  
		         
		        // String sContent ="";
		         message.setContent(sBodyContent,"text/html; charset=utf-8");  
		  
		         // Send message  
		         Transport.send(message);  
		         System.out.println("message sent successfully....");  
		 
		}catch(Exception e) {e.printStackTrace();}
	}catch(Exception e) {e.printStackTrace();}
}
}