package com.teamcenter.soa.certcxpalert.loader;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;

public class CertificateExpiraryUtil {
	
	public static String getEmailBodyContentPMT(String Date, String sObjectString, String strMSG, String sCatalogList) 
	{
		strMSG="Overview:";
		StringBuffer sbfBuffer =  new StringBuffer();
		String Current_Task="Current Task";
		String Process_Name="Process Name";
		String Due_date=Date;
		String Email="https://gplm.honeywell.com/awc/";//goldplmsupport@Honeywell.com
		String Comment="The Document will be expiring on "+Date;
		String instruction="Please complete the necessary tasks before the Expiration Date";


		sbfBuffer.append("<html>");
		sbfBuffer.append("<head>");
		sbfBuffer.append("<style> <!-- @font-face{font-family:'Cambria Math';panose-1:2 4 5 3 5 4 6 3 2 4;} @font-face {font-family:Calibri;panose-1:2 15 5 2 2 2 4 3 2 4;} @font-face {font-family:Consolas; panose-1:2 11 6 9 2 2 4 3 2 4;} p.MsoNormal, li.MsoNormal, div.MsoNormal {margin:0in; margin-bottom:.0001pt; font-size:12.0pt; font-family:'Times New Roman','serif';} a:link, span.MsoHyperlink {color:blue;text-decoration:underline;} a:visited, span.MsoHyperlinkFollowed{color:purple;text-decoration:underline;} p.MsoPlainText, li.MsoPlainText, div.MsoPlainText{mso-style-link:'Plain Text Char';margin-right:0in;margin-left:0in;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraph, li.MsoListParagraph, div.MsoListParagraph{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpFirst, li.MsoListParagraphCxSpFirst, div.MsoListParagraphCxSpFirst{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpMiddle, li.MsoListParagraphCxSpMiddle, div.MsoListParagraphCxSpMiddle {margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpLast, li.MsoListParagraphCxSpLast, div.MsoListParagraphCxSpLast{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} span.PlainTextChar {mso-style-name:'Plain Text Char';mso-style-link:'Plain Text';font-family:Consolas;} span.gwd-span-5eid{mso-style-name:gwd-span-5eid;} span.apple-converted-space{mso-style-name:apple-converted-space;} .MsoChpDefault{font-size:10.0pt;} @page Section1{size:8.5in 11.0in;margin:1.0in 1.0in 1.0in 1.0in;} div.Section1{page:Section1;} ol{margin-bottom:0in;} ul{margin-bottom:0in;}--></style>");
		//sbfBuffer.append("<style> <!-- @font-face{font-family:'Cambria Math';panose-1:2 4 5 3 5 4 6 3 2 4;} @font-face {font-family:Calibri;panose-1:2 15 5 2 2 2 4 3 2 4;} @font-face {font-family:Consolas; panose-1:2 11 6 9 2 2 4 3 2 4;} p.MsoNormal, li.MsoNormal, div.MsoNormal {margin:0in; margin-bottom:.0001pt; font-size:12.0pt; font-family:'Times New Roman','serif';} a:link, span.MsoHyperlink {color:blue;text-decoration:underline;} a:visited, span.MsoHyperlinkFollowed{color:purple;text-decoration:underline;} p.MsoPlainText, li.MsoPlainText, div.MsoPlainText{mso-style-link:'Plain Text Char';margin-right:0in;margin-left:0in;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraph, li.MsoListParagraph, div.MsoListParagraph{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpFirst, li.MsoListParagraphCxSpFirst, div.MsoListParagraphCxSpFirst{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpMiddle, li.MsoListParagraphCxSpMiddle, div.MsoListParagraphCxSpMiddle {margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpLast, li.MsoListParagraphCxSpLast, div.MsoListParagraphCxSpLast{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} span.PlainTextChar {mso-style-name:'Plain Text Char';mso-style-link:'Plain Text';font-family:Consolas;} span.gwd-span-5eid{mso-style-name:gwd-span-5eid;} span.apple-converted-space{mso-style-name:apple-converted-space;} .MsoChpDefault{font-size:10.0pt;} @page Section1{size:8.5in 11.0in;margin:1.0in 1.0in 1.0in 1.0in;} div.Section1{page:Section1;} ol{margin-bottom:0in;} ul{margin-bottom:0in;} table, th, td{border:1px solid black;}--></style>");
		sbfBuffer.append("</head> <body lang=EN-US link=blue vlink=purple> <div>");
		sbfBuffer.append("<p class=MsoNormal><b><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif';color:red'>HON Compliance Document Expiry Notification - </span></b><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif';color:red'> </span></p>");// <div class=MsoNormal align=center style='text-align:center'> <span style='font-size:10.0pt;font-family:'Helvetica','sans-serif''> <hr size=1 width='100%' noshade style='color:red' align=center> </span></div> <p class=MsoNormal align=center style='text-align:center'><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif''>&nbsp;</span></p> <p class=MsoNormal><span style='font-size:10.0pt;font-family:'Courier New''>&nbsp;</span></p> <p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''><b>");
		sbfBuffer.append(strMSG+"</span></p>");
		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
//		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
//		/*	sbfBuffer.append("<p class=MsoNormal><u><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>Escalation Details</span></u></p>");*/
//		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");


		sbfBuffer.append("<form>");
		sbfBuffer.append("<table>");


		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td><label for='curren_task'>Document Id:-</label></td>");
		sbfBuffer.append("<td><label for='curren_task'>"+sObjectString+"</label></td>");
		sbfBuffer.append("</tr>");
		
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td><label for='curren_task'>Catalog List:-</label></td>");
		sbfBuffer.append("<td><label for='curren_task'>"+sCatalogList+"</label></td>");
		sbfBuffer.append("</tr>");
		
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td><label for='curren_task'>Expiry date:-</label></td>");
		sbfBuffer.append("<td><label for='curren_task'>"+Due_date+"</label></td>");
		sbfBuffer.append("</tr>");

		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td><label for='curren_task'>Environment Link:-</label></td>");
		sbfBuffer.append("<td><label for='curren_task'>"+Email+"</label></td>");
		sbfBuffer.append("</tr>");

		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td><label for='curren_task'>Comments:-</label></td>");
		sbfBuffer.append("<td><label for='curren_task'>"+Comment+"</label></td>");
		sbfBuffer.append("</tr>");

		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td><label for='curren_task'>Instructions:-</label></td>");
		sbfBuffer.append("<td><label for='curren_task'>"+instruction+"</label></td>");
		sbfBuffer.append("</tr>");

		sbfBuffer.append("</table>");
		sbfBuffer.append("</form>");

		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p></div>");
		sbfBuffer.append("");
		sbfBuffer.append("<pre><br><span style = 'text-align: left; font-family: Helvetica, Arial, Sans-Serif; font-size: 8pt;color:#C0C0C0 ;'> <i>This e-mail, and any attachments thereto, are intended only for use by the addressee(s) named herein and contain Honeywell confidential information. If you are not the intended recipient of this e-mail, you are hereby notified that any dissemination, distribution or copying which amounts to misappropriation of this e-mail and any attachments thereto, is strictly prohibited. If you have received this e-mail in error, please immediately notify GOLD PLM Team and permanently delete the original and any copy of any e-mail and any printout thereof.</i></span></pre>");
		sbfBuffer.append("</body>");
		sbfBuffer.append("</html>");

		return sbfBuffer.toString();
	}
	
	public static String getEmailBodyContentPMTcustom(String Date, String sObjectString, String strMSG, String sCatalogList) 
	{
		strMSG="Overview:";
		StringBuffer sbfBuffer =  new StringBuffer();
		String Current_Task="Current Task";
		String Process_Name="Process Name";
		String Due_date=Date;
		String Email="https://gplm.honeywell.com/awc/";//goldplmsupport@Honeywell.com
		String Comment="The Document will be expiring on "+Date;
		String instruction="Please complete the necessary tasks before the Expiration Date";


		sbfBuffer.append("<html>");
		sbfBuffer.append("<head>");
		sbfBuffer.append("<style> <!-- @font-face{font-family:'Cambria Math';panose-1:2 4 5 3 5 4 6 3 2 4;} @font-face {font-family:Calibri;panose-1:2 15 5 2 2 2 4 3 2 4;} @font-face {font-family:Consolas; panose-1:2 11 6 9 2 2 4 3 2 4;} p.MsoNormal, li.MsoNormal, div.MsoNormal {margin:0in; margin-bottom:.0001pt; font-size:12.0pt; font-family:'Times New Roman','serif';} a:link, span.MsoHyperlink {color:blue;text-decoration:underline;} a:visited, span.MsoHyperlinkFollowed{color:purple;text-decoration:underline;} p.MsoPlainText, li.MsoPlainText, div.MsoPlainText{mso-style-link:'Plain Text Char';margin-right:0in;margin-left:0in;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraph, li.MsoListParagraph, div.MsoListParagraph{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpFirst, li.MsoListParagraphCxSpFirst, div.MsoListParagraphCxSpFirst{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpMiddle, li.MsoListParagraphCxSpMiddle, div.MsoListParagraphCxSpMiddle {margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpLast, li.MsoListParagraphCxSpLast, div.MsoListParagraphCxSpLast{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} span.PlainTextChar {mso-style-name:'Plain Text Char';mso-style-link:'Plain Text';font-family:Consolas;} span.gwd-span-5eid{mso-style-name:gwd-span-5eid;} span.apple-converted-space{mso-style-name:apple-converted-space;} .MsoChpDefault{font-size:10.0pt;} @page Section1{size:8.5in 11.0in;margin:1.0in 1.0in 1.0in 1.0in;} div.Section1{page:Section1;} ol{margin-bottom:0in;} ul{margin-bottom:0in;}--></style>");
		//sbfBuffer.append("<style> <!-- @font-face{font-family:'Cambria Math';panose-1:2 4 5 3 5 4 6 3 2 4;} @font-face {font-family:Calibri;panose-1:2 15 5 2 2 2 4 3 2 4;} @font-face {font-family:Consolas; panose-1:2 11 6 9 2 2 4 3 2 4;} p.MsoNormal, li.MsoNormal, div.MsoNormal {margin:0in; margin-bottom:.0001pt; font-size:12.0pt; font-family:'Times New Roman','serif';} a:link, span.MsoHyperlink {color:blue;text-decoration:underline;} a:visited, span.MsoHyperlinkFollowed{color:purple;text-decoration:underline;} p.MsoPlainText, li.MsoPlainText, div.MsoPlainText{mso-style-link:'Plain Text Char';margin-right:0in;margin-left:0in;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraph, li.MsoListParagraph, div.MsoListParagraph{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpFirst, li.MsoListParagraphCxSpFirst, div.MsoListParagraphCxSpFirst{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpMiddle, li.MsoListParagraphCxSpMiddle, div.MsoListParagraphCxSpMiddle {margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpLast, li.MsoListParagraphCxSpLast, div.MsoListParagraphCxSpLast{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} span.PlainTextChar {mso-style-name:'Plain Text Char';mso-style-link:'Plain Text';font-family:Consolas;} span.gwd-span-5eid{mso-style-name:gwd-span-5eid;} span.apple-converted-space{mso-style-name:apple-converted-space;} .MsoChpDefault{font-size:10.0pt;} @page Section1{size:8.5in 11.0in;margin:1.0in 1.0in 1.0in 1.0in;} div.Section1{page:Section1;} ol{margin-bottom:0in;} ul{margin-bottom:0in;} table, th, td{border:1px solid black;}--></style>");
		sbfBuffer.append("</head> <body lang=EN-US link=blue vlink=purple> <div>");
		sbfBuffer.append("<p class=MsoNormal><b><span style='color:#448da6; font-size:13.0pt; font-weight:bold; margin-bottom:3px'>"+strMSG+"</span></b><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif';color:red'> </span></p>");// <div class=MsoNormal align=center style='text-align:center'> <span style='font-size:10.0pt;font-family:'Helvetica','sans-serif''> <hr size=1 width='100%' noshade style='color:red' align=center> </span></div> <p class=MsoNormal align=center style='text-align:center'><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif''>&nbsp;</span></p> <p class=MsoNormal><span style='font-size:10.0pt;font-family:'Courier New''>&nbsp;</span></p> <p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''><b>");
		sbfBuffer.append("</span></p>");
//		sbfBuffer.append("<p><span style='color:#448da6; font-weight:bold; margin-bottom:3px"+strMSG+"</span></p>");
//		sbfBuffer.append("<p>"+strMSG+"</p>");
//		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
//		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
//		/*	sbfBuffer.append("<p class=MsoNormal><u><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>Escalation Details</span></u></p>");*/
//		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");


		sbfBuffer.append("<form>");
		sbfBuffer.append("<table style='font-family:arial'>");
		sbfBuffer.append("<tbody>");
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left; color:#808080'>Current Task: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>Certificate Expiry Notification</td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left; color:#808080'>Process Name: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>HON Certificate Expiry Alert Workflow IA : "+sObjectString+"</td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left; color:#808080'>Document ID: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>"+sObjectString+"</td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left; color:#808080'>Catalog List: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>"+sCatalogList+"</td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left; color:#808080'>Expiry Date: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>"+Due_date+"</td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("<tr><td style='vertical-align:top; text-align:left; color:#808080'>Comments: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>The Document will be Expiring soon. Please complete the necessary tasks before the Expiration Date </td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("</tbody>");
		sbfBuffer.append("</table>");
		sbfBuffer.append("</form>");

		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
		sbfBuffer.append("<p class=MsoNormal><b><span style='color:#448da6; font-size:13.0pt; font-weight:bold; margin-bottom:3px'>Select the preferred client to view the task:</span></b></p>");
		sbfBuffer.append("<p>"+Email+"</p>");
		
		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p></div>");
		sbfBuffer.append("");
		sbfBuffer.append("<pre><br><span style = 'text-align: left; font-family: Helvetica, Arial, Sans-Serif; font-size: 8pt;color:#C0C0C0 ;'> <i>This e-mail, and any attachments thereto, are intended only for use by the addressee(s) named herein and contain Honeywell confidential information. If you are not the intended recipient of this e-mail, you are hereby notified that any dissemination, distribution or copying which amounts to misappropriation of this e-mail and any attachments thereto, is strictly prohibited. If you have received this e-mail in error, please immediately notify GOLD PLM Team and permanently delete the original and any copy of any e-mail and any printout thereof.</i></span></pre>");
		sbfBuffer.append("</body>");
		sbfBuffer.append("</html>");

		return sbfBuffer.toString();
	}
	
	public static String getEmailBodyContentHBT (String sEmailID, String[] sItemIDs, HashMap<String, String> hmItemIdCatalogMap, HashMap<String, String> hmItemIdExpiryMap, HashMap<String, String> hmItemIdOwningUserMap, HashMap<String, String> hmItemIdObjectStringMap, String strMSG) 
	{
		try {
			strMSG="Overview:";
			StringBuffer sbfBuffer =  new StringBuffer();
			String Current_Task="Current Task";
			String Process_Name="Process Name";
			String Email="https://gplm.honeywell.com/awc/";//goldplmsupport@Honeywell.com
			String instruction="Please complete the necessary tasks before the Expiration Date ";
			Date dCurrent = new Date();
			SimpleDateFormat formatter=new SimpleDateFormat("dd-MMM-yyyy");
			String dCurrentFormatted = formatter.format(dCurrent);
		
		
			sbfBuffer.append("<html>");
			sbfBuffer.append("<head>");
			sbfBuffer.append("<style> <!-- @font-face{font-family:'Cambria Math';panose-1:2 4 5 3 5 4 6 3 2 4;} @font-face {font-family:Calibri;panose-1:2 15 5 2 2 2 4 3 2 4;} @font-face {font-family:Consolas; panose-1:2 11 6 9 2 2 4 3 2 4;} p.MsoNormal, li.MsoNormal, div.MsoNormal {margin:0in; margin-bottom:.0001pt; font-size:12.0pt; font-family:'Times New Roman','serif';} a:link, span.MsoHyperlink {color:blue;text-decoration:underline;} a:visited, span.MsoHyperlinkFollowed{color:purple;text-decoration:underline;} p.MsoPlainText, li.MsoPlainText, div.MsoPlainText{mso-style-link:'Plain Text Char';margin-right:0in;margin-left:0in;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraph, li.MsoListParagraph, div.MsoListParagraph{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpFirst, li.MsoListParagraphCxSpFirst, div.MsoListParagraphCxSpFirst{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpMiddle, li.MsoListParagraphCxSpMiddle, div.MsoListParagraphCxSpMiddle {margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpLast, li.MsoListParagraphCxSpLast, div.MsoListParagraphCxSpLast{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} span.PlainTextChar {mso-style-name:'Plain Text Char';mso-style-link:'Plain Text';font-family:Consolas;} span.gwd-span-5eid{mso-style-name:gwd-span-5eid;} span.apple-converted-space{mso-style-name:apple-converted-space;} .MsoChpDefault{font-size:10.0pt;} @page Section1{size:8.5in 11.0in;margin:1.0in 1.0in 1.0in 1.0in;} div.Section1{page:Section1;} ol{margin-bottom:0in;} ul{margin-bottom:0in;}  table { border-collapse: collapse; width: 100%;} --></style>");
		
			sbfBuffer.append("</head> <body lang=EN-US link=blue vlink=purple> <div>");
			sbfBuffer.append("<p class=MsoNormal><b><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif';color:red'>HON Compliance Document Expiry Notification - </span></b><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif';color:red'> </span></p> <div class=MsoNormal align=center style='text-align:center'> <span style='font-size:10.0pt;font-family:'Helvetica','sans-serif''> <hr size=1 width='100%' noshade style='color:red' align=center> </span></div> <p class=MsoNormal align=center style='text-align:center'><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif''>&nbsp;</span></p> <p class=MsoNormal><span style='font-size:10.0pt;font-family:'Courier New''>&nbsp;</span></p> <p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''><b>");
			sbfBuffer.append("<style>\r\n" + "table, th, td {\r\n" + "  border:1px solid black;\r\n" + "}\r\n" + "td { text-align: center;}" + "</style>");
			sbfBuffer.append(strMSG+"</span></p>");
			sbfBuffer.append("<p>"+instruction+"</p>");
			sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
			sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
			sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>Environment Link:- </span>"+Email+"</p>");
			sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
		
		
			sbfBuffer.append("<form>");
			sbfBuffer.append("<table style='width:100%' border-collapse: collapse>");
		
		
			sbfBuffer.append("<tr>");
			sbfBuffer.append("<th><label for='curren_task'>Object ID</label></th>");
			sbfBuffer.append("<th><label for='curren_task'>Catalog(s)</label></th>");
			sbfBuffer.append("<th><label for='curren_task'>Expiry Date</label></th>");
			sbfBuffer.append("<th><label for='curren_task'>Notes</label></th>");
			sbfBuffer.append("</tr>");
		
			for(String sItemID: sItemIDs) {
				sbfBuffer.append("<tr>");
				//sbfBuffer.append("<td><label for='curren_task'>"+sItemID+"</label></td>");
				sbfBuffer.append("<td><label for='curren_task'>"+hmItemIdObjectStringMap.get(sItemID)+"</label></td>");
				sbfBuffer.append("<td><label for='curren_task'>"+hmItemIdCatalogMap.get(sItemID)+"</label></td>");
				Date dExpiry = formatter.parse(hmItemIdExpiryMap.get(sItemID));
				String sExpiryDate = formatter.format(dExpiry);
				if(dExpiry.after(dCurrent) || (sExpiryDate.equals(dCurrentFormatted)))
				{
					sbfBuffer.append("<td><label for='curren_task'>"+hmItemIdExpiryMap.get(sItemID)+"</label></td>");
				}
				else
				{
					sbfBuffer.append("<td><FONT COLOR=red><label for='curren_task'>"+hmItemIdExpiryMap.get(sItemID)+"</label></td>");
				}
				if(!sEmailID.equalsIgnoreCase(hmItemIdOwningUserMap.get(sItemID)))
				{
					sbfBuffer.append("<td><label for='curren_task'>You are a Notifier</label></td>");
				}
				else 
				{
					sbfBuffer.append("<td><label for='curren_task'><b>You are the Owner</b></label></td>");
				}
				sbfBuffer.append("</tr>");
			}
		
			sbfBuffer.append("</table>");
			sbfBuffer.append("</form>");
		
			sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p></div>");
			sbfBuffer.append("");
			sbfBuffer.append("<pre><br><span style = 'text-align: left; font-family: Helvetica, Arial, Sans-Serif; font-size: 8pt;color:#C0C0C0 ;'> <i>This e-mail, and any attachments thereto, are intended only for use by the addressee(s) named herein and contain Honeywell confidential information. If you are not the intended recipient of this e-mail, you are hereby notified that any dissemination, distribution or copying which amounts to misappropriation of this e-mail and any attachments thereto, is strictly prohibited. If you have received this e-mail in error, please immediately notify GOLD PLM Team and permanently delete the original and any copy of any e-mail and any printout thereof.</i></span></pre>");
			sbfBuffer.append("</body>");
			sbfBuffer.append("</html>");
		
			return sbfBuffer.toString();
		}catch(Exception e){e.printStackTrace();return "";}
	}
	public static String getEmailBodyContentHBTDL(String Date, String sItemIdValue, String strMSG, String sCatalogList, String sObjectString) 
	{
		strMSG="Overview:";
		StringBuffer sbfBuffer =  new StringBuffer();
		String Current_Task="Current Task";
		String Process_Name="Process Name";
		String Due_date=Date;
		String Email="https://gplm.honeywell.com/awc/";//goldplmsupport@Honeywell.com
		String Comment="The Document will be expiring on "+Date;
		String instruction="Please complete the necessary tasks before the Expiration Date";


		sbfBuffer.append("<html>");
		sbfBuffer.append("<head>");
		sbfBuffer.append("<style> <!-- @font-face{font-family:'Cambria Math';panose-1:2 4 5 3 5 4 6 3 2 4;} @font-face {font-family:Calibri;panose-1:2 15 5 2 2 2 4 3 2 4;} @font-face {font-family:Consolas; panose-1:2 11 6 9 2 2 4 3 2 4;} p.MsoNormal, li.MsoNormal, div.MsoNormal {margin:0in; margin-bottom:.0001pt; font-size:12.0pt; font-family:'Times New Roman','serif';} a:link, span.MsoHyperlink {color:blue;text-decoration:underline;} a:visited, span.MsoHyperlinkFollowed{color:purple;text-decoration:underline;} p.MsoPlainText, li.MsoPlainText, div.MsoPlainText{mso-style-link:'Plain Text Char';margin-right:0in;margin-left:0in;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraph, li.MsoListParagraph, div.MsoListParagraph{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpFirst, li.MsoListParagraphCxSpFirst, div.MsoListParagraphCxSpFirst{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpMiddle, li.MsoListParagraphCxSpMiddle, div.MsoListParagraphCxSpMiddle {margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpLast, li.MsoListParagraphCxSpLast, div.MsoListParagraphCxSpLast{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} span.PlainTextChar {mso-style-name:'Plain Text Char';mso-style-link:'Plain Text';font-family:Consolas;} span.gwd-span-5eid{mso-style-name:gwd-span-5eid;} span.apple-converted-space{mso-style-name:apple-converted-space;} .MsoChpDefault{font-size:10.0pt;} @page Section1{size:8.5in 11.0in;margin:1.0in 1.0in 1.0in 1.0in;} div.Section1{page:Section1;} ol{margin-bottom:0in;} ul{margin-bottom:0in;}--></style>");
		//sbfBuffer.append("<style> <!-- @font-face{font-family:'Cambria Math';panose-1:2 4 5 3 5 4 6 3 2 4;} @font-face {font-family:Calibri;panose-1:2 15 5 2 2 2 4 3 2 4;} @font-face {font-family:Consolas; panose-1:2 11 6 9 2 2 4 3 2 4;} p.MsoNormal, li.MsoNormal, div.MsoNormal {margin:0in; margin-bottom:.0001pt; font-size:12.0pt; font-family:'Times New Roman','serif';} a:link, span.MsoHyperlink {color:blue;text-decoration:underline;} a:visited, span.MsoHyperlinkFollowed{color:purple;text-decoration:underline;} p.MsoPlainText, li.MsoPlainText, div.MsoPlainText{mso-style-link:'Plain Text Char';margin-right:0in;margin-left:0in;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraph, li.MsoListParagraph, div.MsoListParagraph{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpFirst, li.MsoListParagraphCxSpFirst, div.MsoListParagraphCxSpFirst{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpMiddle, li.MsoListParagraphCxSpMiddle, div.MsoListParagraphCxSpMiddle {margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpLast, li.MsoListParagraphCxSpLast, div.MsoListParagraphCxSpLast{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} span.PlainTextChar {mso-style-name:'Plain Text Char';mso-style-link:'Plain Text';font-family:Consolas;} span.gwd-span-5eid{mso-style-name:gwd-span-5eid;} span.apple-converted-space{mso-style-name:apple-converted-space;} .MsoChpDefault{font-size:10.0pt;} @page Section1{size:8.5in 11.0in;margin:1.0in 1.0in 1.0in 1.0in;} div.Section1{page:Section1;} ol{margin-bottom:0in;} ul{margin-bottom:0in;} table, th, td{border:1px solid black;}--></style>");
		sbfBuffer.append("</head> <body lang=EN-US link=blue vlink=purple> <div>");
		sbfBuffer.append("<p class=MsoNormal><b><span style='color:#448da6; font-size:13.0pt; font-weight:bold; margin-bottom:3px'>"+strMSG+"</span></b><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif';color:red'> </span></p>");// <div class=MsoNormal align=center style='text-align:center'> <span style='font-size:10.0pt;font-family:'Helvetica','sans-serif''> <hr size=1 width='100%' noshade style='color:red' align=center> </span></div> <p class=MsoNormal align=center style='text-align:center'><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif''>&nbsp;</span></p> <p class=MsoNormal><span style='font-size:10.0pt;font-family:'Courier New''>&nbsp;</span></p> <p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''><b>");
		sbfBuffer.append("</span></p>");
//		sbfBuffer.append("<p><span style='color:#448da6; font-weight:bold; margin-bottom:3px"+strMSG+"</span></p>");
//		sbfBuffer.append("<p>"+strMSG+"</p>");
//		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
//		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
//		/*	sbfBuffer.append("<p class=MsoNormal><u><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>Escalation Details</span></u></p>");*/
//		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");


		sbfBuffer.append("<form>");
		sbfBuffer.append("<table style='font-family:arial'>");
		sbfBuffer.append("<tbody>");
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left; color:#808080'>Current Task: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>Certificate Expiry Notification</td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left; color:#808080'>Process Name: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>HON Certificate Expiry Alert Workflow HBT : "+sObjectString+"</td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left; color:#808080'>Document ID: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>"+sObjectString+"</td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left; color:#808080'>Catalog List: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>"+sCatalogList+"</td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("<tr>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left; color:#808080'>Expiry Date: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>"+Due_date+"</td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("<tr><td style='vertical-align:top; text-align:left; color:#808080'>Comments: </td>");
		sbfBuffer.append("<td style='vertical-align:top; text-align:left'>The Document will be Expiring soon. Please complete the necessary tasks before the Expiration Date </td>");
		sbfBuffer.append("</tr>");
		sbfBuffer.append("</tbody>");
		sbfBuffer.append("</table>");
		sbfBuffer.append("</form>");

		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
		sbfBuffer.append("<p class=MsoNormal><b><span style='color:#448da6; font-size:13.0pt; font-weight:bold; margin-bottom:3px'>Select the preferred client to view the task:</span></b></p>");
		sbfBuffer.append("<p>"+Email+"</p>");
		
		sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p></div>");
		sbfBuffer.append("");
		sbfBuffer.append("<pre><br><span style = 'text-align: left; font-family: Helvetica, Arial, Sans-Serif; font-size: 8pt;color:#C0C0C0 ;'> <i>This e-mail, and any attachments thereto, are intended only for use by the addressee(s) named herein and contain Honeywell confidential information. If you are not the intended recipient of this e-mail, you are hereby notified that any dissemination, distribution or copying which amounts to misappropriation of this e-mail and any attachments thereto, is strictly prohibited. If you have received this e-mail in error, please immediately notify GOLD PLM Team and permanently delete the original and any copy of any e-mail and any printout thereof.</i></span></pre>");
		sbfBuffer.append("</body>");
		sbfBuffer.append("</html>");

		return sbfBuffer.toString();
	}
	
	
	
	
}
