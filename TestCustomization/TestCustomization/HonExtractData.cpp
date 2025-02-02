#pragma once

#include <tccore/aom_prop.h>
#include <base_utils/mem.h>
#include <tccore/tctype.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <ae/dataset.h>
#include <tcinit/tcinit.h>
#include <tccore/aom.h>
#include <tccore/grm.h>
#include <tccore/tctype.h>
#include <user_exits/epm_toolkit_utils.h>
#include <qry\qry.h>
#include<iostream>
#include<sstream>
#include <base_utils/IFail.hxx>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tc/preferences.h>
#include <sa/user.h>
#include <sa/person.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <strstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <math.h>
#include <sys/stat.h>
#include <ae/ae.h>
#include <property/nr.h>
#include <vector>
#include <nls/nls.h>
#include <locale>
#include <codecvt>
#define FILE_SEP "\\"
#define BUFFER_SIZE 1000
using namespace std;

#define EXIT_FAILURE 1
#define ITK_CALL(x) {           \
    int stat;                     \
    char *err_string = NULL;             \
    if( (stat = (x)) != ITK_ok)   \
    {                             \
		EMH_ask_error_text(stat, &err_string);                 \
		TC_write_syslog ("ERROR: %d ERROR MSG: %s.\n", stat, err_string);           \
		TC_write_syslog ("FUNCTION: %s\nFILE: %s LINE: %d\n",#x, __FILE__, __LINE__); \
		if(err_string) MEM_free(err_string);                                \
		return stat;                                                   \
    }                                                                    \
}

int String_GetEncoding(char* string)
{
	unsigned c, i = 0, flags = 0;
	while (string[i] | string[i + 1] | string[i + 2] | string[i + 3])
		flags = (c = string[i++]) ? flags | ((!(flags % 4) &&
			c > 0x7F) << 3) : flags | 1 | (!(i & 1) << 1)
		| ((string[i] == 0) << 2);
	return (flags & 1) + ((flags & 2) != 0) +
		((flags & 4) != 0) + ((flags & 8) != 0);
}

int ITK_user_main(int argc, char* argv[])
{
	int ifail = ITK_ok;
	int ITK_set_bypass(true);
	const char* sUser = ITK_ask_cli_argument("-u=");
	const char* sPassword = ITK_ask_cli_argument("-p=");
	const char* sGroup = ITK_ask_cli_argument("-g=");
	const char* sItemID = ITK_ask_cli_argument("-itemId=");
	const char* sRevID = ITK_ask_cli_argument("-revId=");
	
	time_t rawtime;
	struct tm* timeinfo;
	char timeStamp[80];
	time(&rawtime);
	timeinfo = gmtime(&rawtime);
	strftime(timeStamp, sizeof(timeStamp), "%d_%m_%Y_%H_%M_%S", timeinfo);
	char* StrTempDirPath = NULL;
	StrTempDirPath = (char*)MEM_alloc(sizeof(char) * 1024);
	char* LogFilePath = (char*)MEM_alloc(sizeof(char) * 1024);
	tc_strcpy(StrTempDirPath, "");
	StrTempDirPath = getenv("TEMP");

	if (StrTempDirPath == NULL || strlen(StrTempDirPath) == 0)
	{
		strcpy(StrTempDirPath, "//tmp");
	}

	tc_strcpy(LogFilePath, StrTempDirPath);
	tc_strcat(LogFilePath, FILE_SEP);
	tc_strcat(LogFilePath, "Data_Extract_");
	tc_strcat(LogFilePath, timeStamp);
	tc_strcat(LogFilePath, "_log.csv");

	TC_write_syslog("Log filePath is %s\n", LogFilePath);
	printf("Log filePath is %s\n", LogFilePath);

	
	ifail = ITK_init_module(sUser, sPassword, sGroup);

	if (ifail == 0)
	{
		printf("\n Login successful");

		tag_t tQuery1 = NULLTAG;

		printf("\n Finding query");
		ITK_CALL(QRY_find2("Item Revision...", &tQuery1));
		
		printf("\n Query Found");

		tag_t RevisionObj = NULLTAG;
		try
		{
			if (tQuery1 != NULLTAG)
			{
				printf("\n Query Found");
				char** entries = NULL;
				char** values = NULL;

				entries = (char**)MEM_alloc(sizeof(char*));
				entries[0] = (char*)MEM_alloc(strlen("Item ID") + 1);
				tc_strcpy(entries[0], "Item ID");
				TC_write_syslog("\n Entry : %s \n", entries[0]);

				entries[1] = (char*)MEM_alloc(strlen("Revision") + 1);
				tc_strcpy(entries[1], "Revision");
				TC_write_syslog("\n Entry : %s \n", entries[1]);

				values = (char**)MEM_alloc(sizeof(char*));
				values[0] = (char*)MEM_alloc(strlen(sItemID) + 1);
				strcpy(values[0], sItemID);
				TC_write_syslog("\n Item ID : %s \n", values[0]);
				printf("\n Item ID : %s \n", values[0]);

				values[1] = (char*)MEM_alloc(strlen(sRevID) + 1);
				strcpy(values[1], sRevID);
				TC_write_syslog("\n Rev ID : %s \n", values[1]);
				printf("\n Rev ID : %s \n", values[1]);
				
				int iResFound = 0;
				tag_t* RevTags = NULL;
				printf("\nFinding Objects...");
				ITK_CALL(QRY_execute(tQuery1, 2, entries, values, &iResFound, &RevTags))
				TC_write_syslog("\n Revision Found: %d \n", iResFound);
				printf("\n Revision Found: %d \n", iResFound);
				
				if (iResFound > 0)
				{
					printf("Result = %d \n", iResFound);

					char* StrObjType = NULL;
					char* StrObjectName = NULL;
					char* StrObjectDesc = NULL;
					StrObjectDesc = (char*)malloc(sizeof(char) * 128);
					ITK_CALL(WSOM_ask_object_type2(RevTags[0], &StrObjType));
					ITK_CALL(AOM_ask_value_string(RevTags[0], "object_name", &StrObjectName));
					ITK_CALL(AOM_ask_value_string(RevTags[0], "object_desc", &StrObjectDesc));

					//ITK_CALL(AOM_UIF_ask_value(RevTags[0], "object_desc", &StrObjectDesc));


					TC_write_syslog("\n Object Type : %s \n", StrObjType);
					TC_write_syslog("\n Object Name : %s \n", StrObjectName);
					TC_write_syslog("\n Object Desc : %s \n", StrObjectDesc);
					printf("\n Object Type : %s \n", StrObjType);
					printf("\n Object Name : %s \n", StrObjectName);
					printf("\n Object Desc : %s \n", StrObjectDesc);
					
					// Create an object of ofstream to write to a file in binary mode
					std::ofstream outFile(LogFilePath, std::ios::binary);

					if (outFile.is_open()) {
						
						std::string text = "";

						text.append(StrObjectName).append(",").append(StrObjectDesc).append(",").append(StrObjType);
						outFile.write(text.c_str(), text.size());

						// Close the file
						outFile.close();

						
					}
					else {
						std::cerr << "Error: Unable to open the file!" << std::endl;
					}
					
					
					
					/*
					bool IsConfigured = NLS_is_server_utf8_configured();
					
					printf("IsConfigured = %d \n", IsConfigured);

					char* str_ext = NULL;
					str_ext = (char*)malloc(sizeof(char) * 128);

					char** argv_utf8 = NULL;
					char** argv = NULL;
					argv = (char**)MEM_alloc(sizeof(char*));
					argv[0] = (char*)MEM_alloc(strlen(StrObjectDesc) + 1);
					strcpy(argv[0],StrObjectDesc);


					NLS_init_utf8(1, (const char**)argv, &argv_utf8);
					
					printf("\n Conversion Completed");
					printf("\n After Conversion : %s \n", argv_utf8);

					NLS_internal_to_external(StrObjectDesc, &str_ext);

					printf("\n After NLS_internal_to_external : %s \n", str_ext);

					int encode = String_GetEncoding(StrObjectDesc);

					printf("Encoding Format = %d \n", encode);*/
				}
				else
				{
					printf("Revision is not found");
				}
				
				
			}
			else
			{

			}
		}
		catch (...)
		{

			printf("Received error to fetched data from object... \n");
			TC_write_syslog("Received error to fetched data from object... \n");
			
		}
	}
	else
	{
		printf("\n Login to Teamcenter Failed.");
	}
	return 0;
}

