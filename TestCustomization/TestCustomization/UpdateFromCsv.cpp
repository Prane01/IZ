#include<iostream>
#include<fstream>
#include<sstream>
#include<tccore/tctype.h>
#include<tccore/aom.h>
#include<tccore/aom_prop.h>
#include<tc/tc_startup.h>
#include<tcinit/tcinit.h>
#include<tccore/item.h>
#include<tc/emh.h>
#include<string.h> 
#include<pom/pom/pom_tokens.h>
#include<base_utils/ScopedSmPtr.hxx>
#include<base_utils/ScopedPtr.hxx>
#include<base_utils/IFail.hxx>
#include<base_utils/ResultCheck.hxx>
#include<base_utils/TcResultStatus.hxx>
#include<vector>

#define POM_bypass_attr_update              3011
using namespace std;
using namespace Teamcenter;
void display();

int ifail = ITK_ok;
ResultStatus status(0);
const char* cpGOVvalues = "ESO2,ESO2.0,ESO3,ESO4,ESO4.1,ESO7";

int searchAndUpdateObjects(string szItemId, string szItemRevisionId, string szObjectType);

int ITK_user_main(int argc, char* argv[])
{
	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username
	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password
	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group
	char *filepath = ITK_ask_cli_argument("-path=");
	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function
		display();
	}
	
	int rowNumber = 0;
	string szItemId;
	string szItemRevisionId;
	string szObjectType;

	try {
		// Login to Teamcenter
		status = ITK_init_to_login();
		status = ITK_init_module(uname, pass, grp);
		string filename = filepath;
		ifstream file(filename.c_str());

		if (!file.is_open()) {
			cerr << "Error: Could not open the file!" << std::endl;
			return 1;
		}

		string line;
		while (getline(file, line)) {
			vector<string> row;
			stringstream lineStream(line);
			string cell;

			while (getline(lineStream, cell, ',')) {
				row.push_back(cell);
			}

			if (rowNumber == 0) {
				rowNumber++;
				continue;
			}

			for (size_t i = 0; i < row.size(); ++i) {
				
				if (i == 0) {
					szItemId = row[i];
					TC_write_syslog("szItemId: %s\n", szItemId.c_str());
				}
				if (i == 1) {
					szItemRevisionId = row[i];
					TC_write_syslog("szItemRevisionId: %s\n", szItemRevisionId.c_str());
				}
				if (i == 2) {
					szObjectType = row[i];
					TC_write_syslog("szObjectType: %s\n", szObjectType.c_str());
				}
				if (!szItemId.empty() && !szItemRevisionId.empty() && !szObjectType.empty()) {
					searchAndUpdateObjects(szItemId, szItemRevisionId, szObjectType);
				}
			}
			cout << endl;
			rowNumber++;
		}
		file.close();

		/*tag_t item;
		char* name;

		status = ITEM_find_item("345678789", &item);
		status = AOM_ask_value_string(item, "object_name", &name);
		cout << "GHJKJ";*/
	}
	catch (IFail &ex)
	{
		ifail = ex.ifail();
		ITKCALL(ifail);
		TC_write_syslog("Error: %s\n", ex.getMessage().c_str());
	}
	
	return 0;
}
int searchAndUpdateObjects(string szItemId, string szItemRevisionId, string szObjectType) 
{

	TC_write_syslog("* * * searchAndUpdateObjects * * *\n\n");
	tag_t tItemObject = NULLTAG;
	tag_t tRevisionObject = NULLTAG;
	char* cpItemProperty = NULL;
	char* cpRevisionProperty = NULL;

	status = ITEM_find_rev(szItemId.c_str(), szItemRevisionId.c_str(), &tRevisionObject);
	TC_write_syslog("tRevisionObject: %d\n", tRevisionObject);
	if (tRevisionObject != NULLTAG) 
	{
		status = ITEM_ask_item_of_rev(tRevisionObject, &tItemObject);
		TC_write_syslog("tItemObject: %d\n", tItemObject);
		status = AOM_ask_value_string(tItemObject, "object_desc", &cpItemProperty);
		status = AOM_ask_value_string(tRevisionObject, "object_desc", &cpRevisionProperty);
		TC_write_syslog("Item Gov Classification: %s\n", cpItemProperty);
		TC_write_syslog("Item Revision Gov Classification: %s\n", cpRevisionProperty);
		if (tc_strcmp(cpItemProperty, cpRevisionProperty) != 0) {
			status = POM_set_env_info(POM_bypass_attr_update, false, 0, 0, NULLTAG, NULL);
			if ((cpItemProperty == NULL || tc_strcmp(cpItemProperty,"") == 0) && (cpRevisionProperty != NULL || tc_strcmp(cpRevisionProperty, "") != 0)) 
			{
				status = AOM_refresh(tItemObject, true);
				status = AOM_set_value_string(tItemObject, "object_desc", cpRevisionProperty);
				status = AOM_save_without_extensions(tItemObject);
				status = AOM_refresh(tItemObject, false);
				TC_write_syslog("Property Updated on Item\n");
			}
			else if ((cpItemProperty != NULL || tc_strcmp(cpItemProperty, "") != 0) && (cpRevisionProperty == NULL || tc_strcmp(cpRevisionProperty, "") == 0)) 
			{
				status = AOM_refresh(tRevisionObject, true);
				status = AOM_set_value_string(tRevisionObject, "object_desc", cpItemProperty);
				status = AOM_save_without_extensions(tRevisionObject);
				status = AOM_refresh(tRevisionObject, false);
				TC_write_syslog("Property Updated on ItemRevision\n");
			}
			else if ((cpItemProperty != NULL || tc_strcmp(cpItemProperty, "") != 0) && (cpRevisionProperty != NULL || tc_strcmp(cpRevisionProperty, "") != 0)) 
			{
				if (tc_strstr(cpGOVvalues, cpRevisionProperty) != 0) {
					status = AOM_refresh(tItemObject, true);
					status = AOM_set_value_string(tItemObject, "object_desc", cpRevisionProperty);
					status = AOM_save_without_extensions(tItemObject);
					status = AOM_refresh(tItemObject, false);
					TC_write_syslog("Property Updated on Item\n");
				}
			}
		}
	}
	MEM_free(cpItemProperty);
	MEM_free(cpRevisionProperty);
	return 0;
}
void display() 
{

	cout << "\n -u\t ---> Enter username";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
	cout << "\n -path\t ---> Enter path of the csv file";
}