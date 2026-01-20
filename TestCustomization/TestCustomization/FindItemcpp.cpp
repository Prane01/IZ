#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include<fstream>
#include <tccore/item.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include<tc/tc_macros.h>
using namespace std;

const char* safeVal(char** arr, int size) {
	if (size == 0) return "";
	return "HII";
}

static void sanitize_folder_name(char *name)
{
	const char *invalid = "<>:\"/\\|?*";
	char *p = name;

	while (*p != '\0')
	{
		if (strchr(invalid, *p) != NULL)
			*p = '_';
		p++;
	}
}

void create_child_export_folder(const char *pathStamp, const char *cpItemID, const char *cpItemRevID)
{
	/* Step 1: Make safe copies (never modify cpItemID / cpItemRevID directly) */
	char safeItemID[256];
	char safeItemRevID[256];

	tc_strcpy(safeItemID, cpItemID);
	tc_strcpy(safeItemRevID, cpItemRevID);

	/* Step 2: Sanitize item ID and revision */
	sanitize_folder_name(safeItemID);
	sanitize_folder_name(safeItemRevID);

	/* Step 3: Build the folderStamp */
	char folderStamp[1000];
	folderStamp[0] = '\0';

	tc_strcpy(folderStamp, pathStamp);   // base path (/tmp/ExportedFiles…)
	tc_strcat(folderStamp, "/");
	tc_strcat(folderStamp, safeItemID);
	tc_strcat(folderStamp, "_");
	tc_strcat(folderStamp, safeItemRevID);

	TC_write_syslog("\n Child folder path: %s", folderStamp);

	/* Step 4: Build mkdir command */
	char cmd[1200];
	cmd[0] = '\0';
	tc_strcpy(cmd, "mkdir \"");
	tc_strcat(cmd, folderStamp);
	tc_strcat(cmd, "\"");

	TC_write_syslog("\n mkdir command: %s", cmd);

	/* Step 5: Execute mkdir */
	const int rc = system(cmd);
	if (rc == -1)
	{
		TC_write_syslog("\n ERROR: Could not create folder!");
		return;
	}

	TC_write_syslog("\n Folder created successfully.");
}

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	bool flag = false;
	//string desc = "ITK description set";
	//char* char_array;
	//strcpy_s(char_array, 10, desc.c_str());
	iFail = ITK_init_module("izn", "izn", "dba");
	char** cpType = NULL;
	int disCount = 0;
	std::ofstream csv("D:\\Honeywell\\Scripts\\Scripts\\output.csv");
	if (!csv.is_open()) {
		std::cout << "Unable to open file!" << std::endl;
		return 1;
	}
	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		iFail = ITEM_find_rev("000295","A", &tItem);
		if (tItem != NULLTAG)
		{
			cout << "\n\n Object Present in the DB..\n";
			ITKCALL(AOM_ask_displayable_values(tItem, "object_type", &disCount, &cpType));
			cout << " object_type : " << disCount << endl;
			cout << " object_type : " << cpType[0] << endl;
			TC_write_syslog("test %s", cpType[0]);


			// Write header
			//csv << "\"" << "01" << "\",";
			//// Write rows
			//csv << safeVal(cpType, disCount) <<",";
			//csv << cpType[0];
			//csv.close();

			char safeItemID[256];
			char safeItemRevID[256];

			create_child_export_folder("D:\\Siemens\\Teamcenter2406\\Export\\", "parent/object\child object", " hi hello");


		}
		else {
			EMH_ask_error_text(iFail, &cError);
			cout << "\n\n The error is : Object not found in DB\n", cError;
		}
	}
	else {
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : %s\n", cError;
	}
	return iFail;
}