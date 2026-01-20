//==================================================
//
//  Copyright 2022 Siemens Digital Industries Software
//
//==================================================

package com.teamcenter.filemanagement;

import com.teamcenter.clientx.AppXSession;


/**
 * This sample client application demonstrates some of the basic features of the
 * FileManagement service.
 *
 */
public class FMS
{

    /**
     * @param args   -help or -h will print out a Usage statement
     */
    public static void main(String[] args)
    {
        if (args.length > 0)
        {
            if (args[0].equals("-help") || args[0].equals("-h"))
            {
                System.out.println("usage: java  [-Dhost=HostAdress] [-Dsso=SsoURL  -DappID=AppID] com.teamcenter.filemanagement.FMS"); 
                System.out.println("Where:");
                System.out.println("   host:        The address of the Teacmenter server to conect to, supported protocols:");
                System.out.println("                HTTP(S):  http://localhost:8080/tc");
                System.out.println("                TCCS:     tccs://env_name  Will connect to Teamcenter using the specified environment name");
                System.out.println("                TCCS:     tccs             Will query the TCCS module for available environments");
                System.out.println("                                           TCCS options require the TCCS module to be installed (FMS_HOME environment variable set).");
                System.out.println("                                           If the given TCCS environment is configured with SSO those settings will be used.");
                System.out.println("                If this option is not provided, the client will default to http://localhost:7001/tc.");
                System.out.println("   sso:         The SSO URL, login prompt will be through SSO");
                System.out.println("   appID:       The SSO application ID.");
                System.out.println("                If the SSO arguments are not provided, the client will prompt for credentials at the console.");
                System.exit(0);
            }
        }

//        AppXSession.getConfigurationFromTCCS();
        String serverHost = AppXSession.getOptionalArg( "host",  "http://localhost:8080/tc");
        String ssoURL     = AppXSession.getOptionalArg( "sso",   "");
        String appID      = AppXSession.getOptionalArg( "appID", "");



        AppXSession session = new AppXSession(serverHost, ssoURL, appID);

        FileManagement fm = new FileManagement();

        // Establish a session with the Teamcenter Server
        session.login();

        // Upload some files
        fm.uploadFiles();

        session.logout();

    }







}
