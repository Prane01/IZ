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
#include<tccore/tctype.h>
#include<tc/folder.h>
#include<sa/user.h>
#include<tc/preferences.h>
using namespace std;

void display();
int checkNullTag(tag_t);
int ifail = 0;
char* cError = NULL;
tag_t problemReportType = NULLTAG;
tag_t createInput = NULLTAG;
tag_t opPR = NULLTAG;
char* typeP = NULL;
tag_t tUser = NULLTAG;
tag_t tHome = NULLTAG;

int ITK_user_main(int argc, char* argv[])
{
	
	string aa = "Custom View";
	char* pse = new char[aa.length()+1];
	strcpy(pse, aa.c_str());

	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			ITK_set_bypass(true);
			SA_find_user2("izn", &tUser);
			SA_ask_user_home_folder(tUser, &tHome);
			TCTYPE_ask_type("ProblemReport", &problemReportType);
			checkNullTag(problemReportType);
			AOM_ask_value_string(problemReportType, "object_type", &typeP);
			cout << "Type: " << typeP << endl;
			TCTYPE_construct_create_input(problemReportType, &createInput);
			checkNullTag(createInput);
			cout << "createInput done" << endl << endl;
			AOM_set_value_string(createInput, "object_name", "TestingFITK");
			cout << "object name set" << endl << endl;
			AOM_set_value_string(createInput, "object_desc", "TestingFITK");
			cout << "object desc set" << endl << endl;
			TCTYPE_create_object(createInput, &opPR);
			checkNullTag(opPR);
			cout << "... object created ..." << endl << endl;
			
			//PREF_set_char_values("ProblemReportRevision_PseudoFolder", 1, &pse);

			AOM_save_without_extensions(opPR);
			FL_insert(tHome, opPR, 999);
			AOM_save(tHome);
			cout << "... created in Home ...";
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