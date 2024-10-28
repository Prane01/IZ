#include<iostream>
#include<fstream>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<tc/tc_startup.h>
#include<tcinit\tcinit.h>
#include<tc\emh.h>
#include<string.h>
#include<bom/bom.h>
#include<tc/folder.h>
#include<direct.h>
#include<ae/ae.h>
#include<fclasses/tc_string.h>
#include<tc/tc_macros.h>
using namespace std;

int ifail = ITK_ok;
char* cError = NULL;
void display();
int checkNullTag(tag_t);
void findInsideObjects(tag_t tChildObject, string sParentFolder);
void namedReferencesDownload(tag_t tPdfDataset, string sParentFolder);

tag_t* tInsideObjects = NULLTAG;
tag_t tParentFolder = NULLTAG;
tag_t* results = NULL;
tag_t* tchildFolders = NULL;

char* cpObjectString = NULL;
char* cpObjectType = NULL;

int iChildCount = 0;
int foundObj = 0;

ofstream outFile;
string sParentFolder = "D:\\Honeywell\\ExportDatasets\\";
string sChildFolder;

int ITK_user_main(int argc, char* argv[])
{
	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char* cpFolderName = ITK_ask_cli_argument("-ty=");

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function
		display();
		return ifail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\n*** Login successful ***\n\n";
			WSO_search_criteria_t folderName;
			WSOM_clear_search_criteria(&folderName);

			tc_strcpy(folderName.name, "Export Dataset from ItemRevision");
			WSOM_search(folderName, &iChildCount, &tchildFolders);

			ITKCALL(AOM_ask_value_string(tchildFolders[0], "object_type", &cpObjectType));
			cout << "cpObjectType " << cpObjectType << endl;
			if (tc_strcmp("Folder", cpObjectType) == 0)
			{
				ITKCALL(AOM_ask_value_tags(tchildFolders[0], "contents", &foundObj, &results));
				cout << "Found objects for Parent folder: " << foundObj << endl;
				ITKCALL(AOM_ask_value_string(tchildFolders[0], "object_name", &cpObjectString));

				sParentFolder += cpFolderName;
				if (_mkdir(sParentFolder.c_str()) == 0) {
					cout << "Folder created successfully at: " << sParentFolder.c_str() << endl;
				}

				for (int i = 0; i < foundObj; i++)
				{
					ITKCALL(AOM_ask_value_string(results[i], "object_type", &cpObjectType));
					cout << "cpObjectType " << cpObjectType << endl;
					if (tc_strcmp("Folder", cpObjectType) == 0)
					{
						findInsideObjects(results[i], sParentFolder);
					}
					else if (tc_strcmp("PDF", cpObjectType) == 0) {
						namedReferencesDownload(results[i], sParentFolder);
					}
				}
			}
		}
		else
		{
			EMH_ask_error_text(ifail, &cError);
			cout << "\n\n The error is : " << cError;
		}
	}
	else
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "\n\n The error is : " << cError;
	}
	return ifail;
}
void display() {
	cout << "\n -u\t ---> Enter username";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
}

int checkNullTag(tag_t tag)
{
	if (tag == NULLTAG)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "The Error is: ", cError;
		exit(0);
	}
	else
	{
		return 0;
	}
	return ITK_ok;
}
void findInsideObjects(tag_t tChildObject, string sParentFolder) {

	int insideFoundObj = 0;
	tag_t* tInsideResults = NULL;
	ITKCALL(AOM_ask_value_tags(tChildObject, "contents", &insideFoundObj, &tInsideResults));
	ITKCALL(AOM_ask_value_string(tChildObject, "object_name", &cpObjectString));
	cout << insideFoundObj << endl;
	cout << "object_string : " << cpObjectString << endl;
	sParentFolder += "\\";
	sParentFolder += cpObjectString;

	if (_mkdir(sParentFolder.c_str()) == 0) {
		cout << "Folder created successfully at: " << sParentFolder.c_str() << endl;
	}

	for (int i = 0; i < insideFoundObj; i++)
	{
		ITKCALL(AOM_ask_value_string(tInsideResults[i], "object_type", &cpObjectType));
		cout << "object_type : " << cpObjectType << endl;
		
		if (tc_strcmp("Folder", cpObjectType) == 0)
		{
			findInsideObjects(tInsideResults[i], sParentFolder);
		}
		else if(tc_strcmp("PDF", cpObjectType)==0){
			namedReferencesDownload(tInsideResults[i], sParentFolder);
		}
	}

}
void namedReferencesDownload(tag_t tPdfDataset, string sParentFolder) {

	string namedReferenceFullPath = sParentFolder;
	ITKCALL(AOM_ask_value_string(tPdfDataset, "object_name", &cpObjectString));
	namedReferenceFullPath += "\\";
	namedReferenceFullPath += cpObjectString;
	namedReferenceFullPath +=".pdf";
	ITKCALL(AE_export_named_ref(tPdfDataset, "PDF_Reference", namedReferenceFullPath.c_str()));
	cout << cpObjectString << " Downloaded." << endl;

}