#include<stdio.h>

#include<conio.h>

#include<iostream>

#include<fstream>

#include<tccore/tctype.h>

#include<tccore/aom_prop.h>

#include<tccore/aom.h>

#include<tc/tc_startup.h>

#include<tcinit\tcinit.h>

#include<tccore/item.h>

#include<tc\emh.h>

#include<stdlib.h>

#include<string.h>

#include<bom/bom.h>

#include<tc/folder.h>

#include<direct.h>

#include<ae/ae.h>

#include<tc/tc_macros.h>

#include<tccore/grm.h>

#include<tccore/workspaceobject.h>

using namespace std;

void display();

int checkNullTag(tag_t tag);

void findInsideObjects(tag_t tChildObject, string sParentFolder);

void namedReferencesDownload(tag_t tExcelDataset, string sParentFolder);

int ifail = 0;

char* cError = NULL;

tag_t* tchildrenInner = NULLTAG;

tag_t* tInsideObjects = NULLTAG;

tag_t tParentFolder = NULLTAG;

int foundObj = 0;

tag_t* results = NULL;

char* cpObjectString = NULL;

char* cpObjectType = NULL;

ofstream outFile;

string sParentFolder = "D:\\Honeywell\\";

string sChildFolder;

int iChildCount = 0;

tag_t* tchildFolders = NULL;

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

					else if (tc_strcmp("Item", cpObjectType) == 0)

					{

						tag_t item_tag = NULLTAG;

						tag_t tLatestRev = NULLTAG;

						//ITKCALL(ITEM_ask_item_of_rev(results[i], &item_tag));

						ITKCALL(ITEM_ask_latest_rev(results[i], &tLatestRev));

						tag_t rel_type = NULLTAG;

						tag_t* dataset = NULL;

						int Count = 0;

						ITKCALL(GRM_find_relation_type("IMAN_specification", &rel_type));

						ITKCALL(GRM_list_secondary_objects_only(tLatestRev, rel_type, &Count, &dataset));

						if (Count == 0)continue;

						for (int j = 0; j < Count; j++)

						{

							char *Sec_obj_type = NULL;

							ITKCALL(WSOM_ask_object_type2(dataset[j], &Sec_obj_type));
								
							if (tc_strcmp(Sec_obj_type, "MSExcel") == 0 || tc_strcmp(Sec_obj_type, "MSExcelX") == 0)

							{

								//tag_t *ref_object=NULL;

								//int Count1=0;

								//ITKCALL(iFail, AE_ask_all_dataset_named_ref2(dataset[j],"excel",&Count1,&ref_object);

								namedReferencesDownload(dataset[j], sParentFolder);

							}

							//if(Sec_obj_type!=NULL)MEM_free(Sec_obj_type);

						}


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

		else if (tc_strcmp("Item", cpObjectType) == 0)

		{

			tag_t item_tag = NULLTAG;

			tag_t tLatestRev = NULLTAG;

			//ITKCALL(ITEM_ask_item_of_rev(results[i], &item_tag));

			ITKCALL(ITEM_ask_latest_rev(tInsideResults[i], &tLatestRev));

			tag_t rel_type = NULLTAG;

			tag_t* dataset = NULL;

			int Count = 0;

			ITKCALL(GRM_find_relation_type("IMAN_specification", &rel_type));

			ITKCALL(GRM_list_secondary_objects_only(tLatestRev, rel_type, &Count, &dataset));

			if (Count == 0)continue;

			for (int j = 0; j < Count; j++)

			{

				char *Sec_obj_type = NULL;

				ITKCALL(WSOM_ask_object_type2(dataset[j], &Sec_obj_type));

				cout << "Sec_obj_type " << Sec_obj_type << endl;

				if (tc_strcmp(Sec_obj_type, "MSExcel") == 0 || tc_strcmp(Sec_obj_type, "MSExcelX") == 0)

				{

					//tag_t *ref_object=NULL;

					//int Count1=0;

					//ITKCALL(iFail, AE_ask_all_dataset_named_ref2(dataset[j],"excel",&Count1,&ref_object);

					namedReferencesDownload(dataset[j], sParentFolder);

				}

				//if(Sec_obj_type!=NULL)MEM_free(Sec_obj_type);

			}	

		}

	}

}

void namedReferencesDownload(tag_t tExcelDataset, string sParentFolder) {

	string namedReferenceFullPath = sParentFolder;

	ITKCALL(AOM_ask_value_string(tExcelDataset, "object_name", &cpObjectString));

	cout << cpObjectString << " cpObjectString." << endl;

	namedReferenceFullPath += "\\";

	namedReferenceFullPath += cpObjectString;

	namedReferenceFullPath += ".xlsx";

	ITKCALL(AE_export_named_ref(tExcelDataset, "excel", namedReferenceFullPath.c_str()));

	cout << cpObjectString << " Downloaded." << endl;

}