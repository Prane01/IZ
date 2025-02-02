package com.teamcenter.soa.datasetexport.util;

public class NotificationBody {
	
	public static String getEmailBodyContent (String sItemIdValue,String UserZipFileName, String strMSG) {

		strMSG="Overview:";
		StringBuffer sbfBuffer =  new StringBuffer();

			String Current_Task="Current Task";
			String Process_Name="Process Name";
			//String Due_date=Date;
			String Email="goldplmsupport@Honeywell.com";
			//String Comment="Please find the attached zip file location for exported bom datasets. ";
			String instruction="Please download the dataset files by accessing the below shared location ";
			String ZipFileLocation=UserZipFileName;


			sbfBuffer.append("<html>");
			sbfBuffer.append("<head>");
			sbfBuffer.append("<style> <!-- @font-face{font-family:'Cambria Math';panose-1:2 4 5 3 5 4 6 3 2 4;} @font-face {font-family:Calibri;panose-1:2 15 5 2 2 2 4 3 2 4;} @font-face {font-family:Consolas; panose-1:2 11 6 9 2 2 4 3 2 4;} p.MsoNormal, li.MsoNormal, div.MsoNormal {margin:0in; margin-bottom:.0001pt; font-size:12.0pt; font-family:'Times New Roman','serif';} a:link, span.MsoHyperlink {color:blue;text-decoration:underline;} a:visited, span.MsoHyperlinkFollowed{color:purple;text-decoration:underline;} p.MsoPlainText, li.MsoPlainText, div.MsoPlainText{mso-style-link:'Plain Text Char';margin-right:0in;margin-left:0in;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraph, li.MsoListParagraph, div.MsoListParagraph{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpFirst, li.MsoListParagraphCxSpFirst, div.MsoListParagraphCxSpFirst{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpMiddle, li.MsoListParagraphCxSpMiddle, div.MsoListParagraphCxSpMiddle {margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} p.MsoListParagraphCxSpLast, li.MsoListParagraphCxSpLast, div.MsoListParagraphCxSpLast{margin-top:0in;margin-right:0in;margin-bottom:0in;margin-left:.5in;margin-bottom:.0001pt;font-size:12.0pt;font-family:'Times New Roman','serif';} span.PlainTextChar {mso-style-name:'Plain Text Char';mso-style-link:'Plain Text';font-family:Consolas;} span.gwd-span-5eid{mso-style-name:gwd-span-5eid;} span.apple-converted-space{mso-style-name:apple-converted-space;} .MsoChpDefault{font-size:10.0pt;} @page Section1{size:8.5in 11.0in;margin:1.0in 1.0in 1.0in 1.0in;} div.Section1{page:Section1;} ol{margin-bottom:0in;} ul{margin-bottom:0in;} --></style>");

			sbfBuffer.append("</head> <body lang=EN-US link=blue vlink=purple> <div>");
			sbfBuffer.append("<p class=MsoNormal><b><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif';color:red'>ECN Dataset Download Notification - </span></b><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif';color:red'> </span></p> <div class=MsoNormal align=center style='text-align:center'> <span style='font-size:10.0pt;font-family:'Helvetica','sans-serif''> <hr size=1 width='100%' noshade style='color:red' align=center> </span></div> <p class=MsoNormal align=center style='text-align:center'><span style='font-size:10.0pt;font-family:'Helvetica','sans-serif''>&nbsp;</span></p> <p class=MsoNormal><span style='font-size:10.0pt;font-family:'Courier New''>&nbsp;</span></p> <p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''><b>");
			sbfBuffer.append(strMSG+"</span></p>");
			sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
			sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");
			/*	sbfBuffer.append("<p class=MsoNormal><u><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>Escalation Details</span></u></p>");*/
			sbfBuffer.append("<p class=MsoNormal><span style='font-size:11.0pt;font-family:'Arial','sans-serif''>&nbsp;</span></p>");


			sbfBuffer.append("<form>");
			sbfBuffer.append("<table>");


			sbfBuffer.append("<tr>");
			sbfBuffer.append("<td><label for='curren_task'>ECN Id:-</label></td>");
			sbfBuffer.append("<td><label for='curren_task'>"+sItemIdValue+"</label></td>");
			sbfBuffer.append("</tr>");
			/*
			sbfBuffer.append("<tr>");
			sbfBuffer.append("<td><label for='curren_task'>Catalog List:-</label></td>");
			sbfBuffer.append("<td><label for='curren_task'>"+sCatalogList+"</label></td>");
			sbfBuffer.append("</tr>");
			
			sbfBuffer.append("<tr>");
			sbfBuffer.append("<td><label for='curren_task'>Due date:-</label></td>");
			sbfBuffer.append("<td><label for='curren_task'>"+Due_date+"</label></td>");
			sbfBuffer.append("</tr>");
			*/
			sbfBuffer.append("<tr>");
			sbfBuffer.append("<td><label for='curren_task'>Email From:-</label></td>");
			sbfBuffer.append("<td><label for='curren_task'>"+Email+"</label></td>");
			sbfBuffer.append("</tr>");

			sbfBuffer.append("<tr>");
			sbfBuffer.append("<td><label for='curren_task'>Instructions:-</label></td>");
			sbfBuffer.append("<td><label for='curren_task'>"+instruction+"</label></td>");
			sbfBuffer.append("</tr>");

			sbfBuffer.append("<tr>");
			sbfBuffer.append("<td><label for='curren_task'>File Location:-</label></td>");
			sbfBuffer.append("<td><label for='curren_task'>"+ZipFileLocation+"</label></td>");
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
}
