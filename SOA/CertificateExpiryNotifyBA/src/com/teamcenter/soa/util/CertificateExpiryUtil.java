package com.teamcenter.soa.util;

import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.administration.PreferenceManagementService;
import com.teamcenter.services.strong.administration._2012_09.PreferenceManagement.CompletePreference;
import com.teamcenter.services.strong.administration._2012_09.PreferenceManagement.GetPreferencesResponse;
import com.teamcenter.services.strong.administration._2012_09.PreferenceManagement.PreferenceValue;

public class CertificateExpiryUtil {

    /**
     * Build the full HTML mail body for the given email and status maps.
     * Extracts a friendly recipient name from the email automatically.
     */
    public static String buildMailBody(
            String email,
            Map<String, Set<String>> expired,
            Map<String, Set<String>> expiringIn3months,
            Map<String, Set<String>> expiringIn6months,
            Map<String, Set<String>> expiringIn1Year,
            Map<String, Set<String>> renewed3monthsago) {

        StringBuilder sb = new StringBuilder();
        String activeWorkspaceUrl = getAWCurl();

        sb.append("<html><head><style>")
          .append("table {border-collapse: collapse; width: 100%; font-family: Arial, sans-serif; font-size: 14px;}")
          .append("th, td {border: 1px solid #000; padding: 5px; text-align: left;}")
          .append("th {background-color: #19BDFF; color: black; font-weight: normal;}")
          .append("tr:nth-child(even) {background-color: #f4f6f8;}")
          .append("</style></head><body>");

        // Extract recipient name
        String nameToUse = extractRecipientNameFromEmail(email);

        sb.append("<p>Dear ").append(escapeHtml(nameToUse)).append(",</br></p>");
        sb.append("<p>Please find the status of Certification schemes/Regulatory Specifications that needs your attention. Please ignore if the action is already taken.</p></br>");

        // Always build all five tables in required order, passing rows (can be empty)

     // Expired
        Set<String> expiredSet = expired.get(email) != null ? expired.get(email) : Collections.emptySet();
        sb.append(buildTableAlwaysShow(expiredSet, "Certificates Expired", true));

        // Certificates Expiring in 0-3 Months
        Set<String> exp3Set = expiringIn3months.get(email) != null ? expiringIn3months.get(email) : Collections.emptySet();
        sb.append(buildTableAlwaysShow(exp3Set, "Certificates Expiring in 0–3 Months", false));

        // Certificates Expiring in 3-6 Months
        Set<String> exp6Set = expiringIn6months.get(email) != null ? expiringIn6months.get(email) : Collections.emptySet();
        sb.append(buildTableAlwaysShow(exp6Set, "Certificates Expiring in 3–6 Months", false));

        // Certificates Expiring in 6-12 Months
        Set<String> exp1YSet = expiringIn1Year.get(email) != null ? expiringIn1Year.get(email) : Collections.emptySet();
        sb.append(buildTableAlwaysShow(exp1YSet, "Certificates Expiring in 6–12 Months", false));

        // Renewed in last 3 Months
        Set<String> renewedSet = renewed3monthsago.get(email) != null ? renewed3monthsago.get(email) : Collections.emptySet();
        sb.append(buildTableAlwaysShow(renewedSet, "Certificates / Specifications Renewed Last 3 Months", false));



//		Pattern pattern = Pattern.compile("gplm(.*?)\\.honeywell");
//        Matcher matcher = pattern.matcher(activeWorkspaceUrl);
//        String value = "";
//        if (matcher.find()) {
//            value = matcher.group(1);
//            System.out.println("Extracted value: '" + value + "'");
//        } else {
//            System.out.println("No match for: " + activeWorkspaceUrl);
//        }
//
//     // Instruction and Workspace link
//        sb.append("</br>").append(value.toUpperCase()).append(" : ")
//          .append("<a href=\"").append(activeWorkspaceUrl)
//          .append("\" style=\"font-size:15px; font-family: Arial, sans-serif; color: #0000EE; text-decoration: underline;\" target=\"_blank\">Active Workspace</a><br>");

        // Footer
        sb.append("<br>Thanks,</br>Gold PLM Support<br><br>")
        .append("<div style=\"color: #999999; font-size: 14px; font-family: Arial, sans-serif; margin-top: 20px; font-style: italic;\">This e-mail, and any attachments thereto, are intended only for use by the addressee(s) named herein and contain Honeywell confidential information. If you are not the intended recipient of this e-mail, you are hereby notified that any dissemination, distribution or copying which amounts to misappropriation of this e-mail and any attachments thereto, is strictly prohibited. If you have received this e-mail in error, please immediately notify GOLD PLM Team and permanently delete the original and any copy of any e-mail and any printout thereof\r\n"
                + "</div>");
        
        sb.append("</body></html>"); 
        return sb.toString();
    }

    /**
     * Builds an HTML table showing 9 columns:
     * - The first column's text is linked to a URL provided as the 10th tab-separated value in each row (if present),
     * - If no URL is present, the first column is plain text.
     * - If rows empty, shows "No records found".
     */
    private static String buildTableAlwaysShow(Set<String> expiredSet, String tableTitle, boolean isRedText) {
        final int visibleCols = 9;  // Number of visible columns in table
        final int totalCols = visibleCols + 1; // 10 columns in data including hyperlink URL

        StringBuilder sb = new StringBuilder();
        sb.append("<div style='text-align:center;'>");
        sb.append("<table style='margin-left:auto; margin-right:auto; border-collapse: collapse; width: 100%; font-family: Arial, sans-serif; font-size: 14px;'>");

        // Title row
        if (tableTitle != null && !tableTitle.trim().isEmpty()) {
            sb.append("<tr>")
              .append("<td colspan='").append(visibleCols).append("' ")
              .append("style='text-align:center; font-size:14px; padding:10px 0; background-color: #05014a; color: white;'>")
              .append(escapeHtml(tableTitle))
              .append("</td></tr>");
        }

        // Header row
        sb.append("<tr style='background-color: #1976d2; color: black; font-size:16px'>")
          .append("<th>Certification Scheme/Regulatory Spec</th>")
          .append("<th>Document Type</th>")
          .append("<th>Affected SKUs</th>")
          .append("<th>Document number<br>(for Certification Scheme)</th>")
          .append("<th>Affected Regulatory Spec/Certification Scheme</th>")
          .append("<th>Timeline</th>")
          .append("<th>Action Needed by</th>")
          .append("<th>Regulatory Owner</th>")
          .append("<th>Expiry Date/ Date of withdrawal</th>")
          .append("</tr>");

        if (expiredSet == null || expiredSet.isEmpty()) {
            sb.append("<tr><td colspan='").append(visibleCols)
              .append("' style='text-align:center; color:#999;'>No records found</td></tr>");
        } else {
            boolean even = false;
            for (String row : expiredSet) {
                String[] cols = row.split("\t", totalCols);
                String rowStyle = even ? " style='background-color: #f4f6f8;" : " style='";
                if (isRedText) {
                    rowStyle += "color: red;";
                }
                rowStyle += "'";
                sb.append("<tr").append(rowStyle).append(">");

//                for (int i = 0; i < visibleCols; i++) {
//                    String value = (i < cols.length) ? cols[i].trim() : "";
//                    if (i == 0) {
//                        // Hyperlink for first column if URL present in 10th column (index 9)
//                        String hyperlink = (cols.length > visibleCols) ? cols[visibleCols].trim() : "";
//                        if (!hyperlink.isEmpty()) {
//                            sb.append("<td>")
//                              .append("<a href=\"").append(escapeHtml(hyperlink)).append("\" style=\"color: #0072ce; text-decoration: underline;\" target=\"_blank\">")
//                              .append(escapeHtml(value))
//                              .append("</a></td>");
//                        } else {
//                            sb.append("<td>").append(escapeHtml(value)).append("</td>");
//                        }
//                    } else {
//                        sb.append("<td>").append(escapeHtml(value)).append("</td>");
//                    }
//                }
                for (int i = 0; i < visibleCols; i++) {
                    String value = (i < cols.length) ? cols[i].trim() : "";

                    if (i == 4) { // 5th column, index 4, where multiline data appears
                        // Replace new lines with <br> for HTML formatting
                        value = escapeHtml(value).replace("\n", "<br>");
                    } else {
                        value = escapeHtml(value);
                    }

                    if (i == 0) {
                        // Existing hyperlink logic (unchanged)
                        String hyperlink = (cols.length > visibleCols) ? cols[visibleCols].trim() : "";
                        if (!hyperlink.isEmpty()) {
                            sb.append("<td>")
                              .append("<a href=\"").append(escapeHtml(hyperlink)).append("\" style=\"color: #0072ce; text-decoration: underline;\" target=\"_blank\">")
                              .append(value)
                              .append("</a></td>");
                        } else {
                            sb.append("<td>").append(value).append("</td>");
                        }
                    } else {
                        sb.append("<td>").append(value).append("</td>");
                    }
                }
                sb.append("</tr>");
                even = !even;
            }
        }

        sb.append("</table>");
        sb.append("</div>");
        return sb.toString();
    }

    /** Extract user-friendly name from email address */
    private static String extractRecipientNameFromEmail(String email) {
        if (email == null || !email.contains("@")) {
            return "PLM User";
        }
     
        String userPart = email.substring(0, email.indexOf('@'));
        String[] parts = userPart.split("[._-]"); // split by dot OR underscore
        StringBuilder nameBuilder = new StringBuilder();
     
        for (int i = 0; i < parts.length; i++) {
            String part = parts[i].trim();
            if (!part.isEmpty()) {
                //Smart acronym handling: if all caps and short (≤4 chars), keep as-is
                if (part.equals(part.toUpperCase()) && part.length() <= 4) {
                    nameBuilder.append(part);
                } else {
                    nameBuilder.append(Character.toUpperCase(part.charAt(0)));
                    if (part.length() > 1) {
                        nameBuilder.append(part.substring(1).toLowerCase());
                    }
                }
     
                if (i < parts.length - 1) {
                    nameBuilder.append(" ");
                }
            }
        }
     
        return nameBuilder.toString().trim();
    }

    /** Escape HTML special characters */
    private static String escapeHtml(String s) {
        if (s == null) {
            return "";
        }
        return s.replace("&", "&amp;")
                .replace("<", "&lt;")
                .replace(">", "&gt;")
                .replace("\"", "&quot;")
                .replace("'", "&#39;");
    }

    /** Retrieve Active Workspace URL from preferences */
    public static String getAWCurl() {
        String[] actualPrefValues = {};
        PreferenceManagementService prefMngmtSvc = PreferenceManagementService.getService(AppXSession.getConnection());
        GetPreferencesResponse prefResponse = prefMngmtSvc.getPreferences(new String[] {"ActiveWorkspaceHosting.WorkflowEmail.URL"}, false);

        if(prefResponse.data.sizeOfPartialErrors() > 0) {
            // handle errors if required; left as-is for brevity
        } else {
            CompletePreference[] compPrefs = prefResponse.response;
            for(CompletePreference cPref : compPrefs) {
                PreferenceValue prefValue = cPref.values;
                actualPrefValues = prefValue.values;
            }
        }
        return actualPrefValues[0];
    }
}
