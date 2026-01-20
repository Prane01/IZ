package com.teamcenter.soa.EDW.AMPL.Util;

import java.util.Date;
import java.text.SimpleDateFormat;

public class EDWMessageUtil {
    
    public EDWMessageUtil() {
    }

    public static String GetDateTimeStamp() {
        
        // getting the system date
        Date tempDate = new Date();
        
        SimpleDateFormat format1 = new SimpleDateFormat("dd_MM_yyyy_HH_mm_ss");
        format1.format(tempDate);
        // getting the object of the Timestamp class

        return format1.format(tempDate);
    }

    public static String LogMessage (String strLogType, String strLogMessage) {
        
        String strNewLogMessage = "[" + GetDateTimeStamp() + "] : [" + strLogType.toUpperCase() + "]" + strLogMessage;

        return strNewLogMessage;
    }
}
