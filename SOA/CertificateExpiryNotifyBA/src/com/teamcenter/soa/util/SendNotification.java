package com.teamcenter.soa.util;

import java.io.File;
import java.util.*;
import javax.mail.*;
import javax.mail.internet.*;

public class SendNotification {
	public static void sendNotifications(String toAddress, String ccAddresses, String fromAddress, String sHost,
			String sMailSubject, String sBodyContent, String sFooterContent) {
		try {
			Properties properties = System.getProperties();
			properties.setProperty("mail.smtp.host", sHost);
			Session session = Session.getDefaultInstance(properties);

			try {
				MimeMessage message = new MimeMessage(session);
				message.setFrom(new InternetAddress(fromAddress));

// TO recipient (only one)
				if (toAddress != null && !toAddress.trim().isEmpty()) {
					message.addRecipient(Message.RecipientType.TO, new InternetAddress(toAddress.trim()));
				}

// CC recipients
				if (ccAddresses != null && !ccAddresses.trim().isEmpty()) {
					message.addRecipients(Message.RecipientType.CC, InternetAddress.parse(ccAddresses));
				}

				message.setSubject(sMailSubject);
				message.setContent(sBodyContent, "text/html; charset=utf-8");

				Transport.send(message);
				System.out.println("message sent successfully....");

			} catch (Exception e) {
				e.printStackTrace();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}