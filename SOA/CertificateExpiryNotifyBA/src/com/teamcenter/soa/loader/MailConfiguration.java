package com.teamcenter.soa.loader;

import com.teamcenter.soa.util.CertificateExpiryUtil;
import com.teamcenter.soa.util.SendNotification;

import java.io.IOException;
import java.time.LocalDate;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.stream.Collectors;

import com.teamcenter.soa.loader.Connect;

public class MailConfiguration {

	public static Properties properties;

	public MailConfiguration(Properties properties) {
		this.properties = properties;
	}

	public void configMail(Map<String, Map<String, Set<String>>> allCategories) throws IOException {

		// Collect all email keys from all 5 maps
		List<String> userEmails = allCategories.values().stream().flatMap(map -> map.keySet().stream()).distinct() // Optional:
																													// remove
																													// duplicates
				.collect(Collectors.toList());

		Map<String, Set<String>> expired = allCategories.getOrDefault("Expired", Map.of());
		Map<String, Set<String>> expiringIn3months = allCategories.getOrDefault("Expiring in 0-3 months", Map.of());
		Map<String, Set<String>> expiringIn6months = allCategories.getOrDefault("Expiring in 3-6 months", Map.of());
		Map<String, Set<String>> expiringIn1Year = allCategories.getOrDefault("Expiring in 6-12 months", Map.of());
		Map<String, Set<String>> renewed3monthsago = allCategories.getOrDefault("Renewed 3 months ago", Map.of());

		for (String email : userEmails) {
			System.out.println("***email : " + email);
			LocalDate todayDate = LocalDate.now();
			String MAIL_SUBJECT_DATE = properties.getProperty("MAIL_SUBJECT") + " - " + todayDate;
			String sBodyContent = CertificateExpiryUtil.buildMailBody(email, expired, expiringIn3months,
					expiringIn6months, expiringIn1Year, renewed3monthsago);
			try {
				SendNotification.sendNotifications(email, "", properties.getProperty("MAIL_FROM"),
						properties.getProperty("MAIL_HOST"), MAIL_SUBJECT_DATE, sBodyContent,
						properties.getProperty("MAIL_FOOTER_CONTENT"));
				System.out.println("INFO: Mail is sent to " + email + " notifier");
				Connect.writer.write("INFO: Mail is sent to " + email + " notifier");
			} catch (Exception e) {
				System.err.println("ERROR: Failed to send mail to " + email + " - " + e.getMessage());
				Connect.writer.write("ERROR: Failed to send mail to " + email + " - " + e.getMessage());
			}
		}
	}
}
