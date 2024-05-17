#include<iostream>
#include<stdio.h>
#include<conio.h>
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
#include<qry/qry.h>
#define qName "General..."
using namespace std;

void display();

int checkNullTag(tag_t tag);

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	tag_t tQuery = NULLTAG;
	int entries = 0;
	
	char** actEntries = NULL;
	char* userEntries[4];

	char** actValues[14];
	char* userValue[4];
	char* op = NULL;

	int foundObj = 0;
	tag_t* results = NULLTAG;

	char* uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char* pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char* grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char* type = ITK_ask_cli_argument("-ty=");

	char* owning_user = ITK_ask_cli_argument("-ou=");

	char* owning_grp = ITK_ask_cli_argument("-grp=");

	char* rel = ITK_ask_cli_argument("-rel=");

	

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			QRY_find2(qName, &tQuery); // finding query 
			checkNullTag(tQuery);
			QRY_find_user_entries(tQuery, &entries, &actEntries, actValues);
			if (entries != 0 && actEntries != NULLTAG && actValues != NULLTAG)
			{
				Item_fin
				cout << entries << endl;
				for (int i = 0; i < entries; i++)
				{
					//cout << actEntries[i] << "  ";
					if (i == 2)
					{
						actValues[i] = &type;
						//cout << "\ntype successful...\n\n";
						//cout << *actValues << endl;
						userValue[0] = *actValues[i];
						userEntries[0] = actEntries[i];

					}
					else if (i == 3)
					{
						actValues[i] = &owning_user;
						//cout << "\nowning_user successful...\n\n";
						//cout << *actValues << endl;
						userValue[1] = *actValues[i];
						userEntries[1] = actEntries[i];
					}
					else if (i == 4)
					{
						actValues[i] = &owning_grp;
						//cout << "\nowning_grp successful...\n\n";
						//cout << *actValues << endl;
						userValue[2] = *actValues[i];
						userEntries[2] = actEntries[i];
					}
					else if (i == 11)
					{
						actValues[i] = &rel;
						//cout << "\nrel successful...\n\n";
						//cout << *actValues << endl;
						userValue[3] = *actValues[i];
						userEntries[3] = actEntries[i];
					}
					else
					{
						//cout << endl;
						continue;
					}
					/*cout << *actValues[i];
					cout << '\n';*/
				}
				/*for (int i=0; i<4; i++)
				{
					cout << userEntries[i] << endl;
					cout << userValue[i] << endl;
				}*/
				QRY_execute(tQuery, 4, userEntries, userValue, &foundObj, &results);
				cout << "Found objects: " << foundObj << endl;
				for (int i = 0; i < foundObj; i++)
				{
					AOM_ask_value_string(results[i], "object_string", &op);
					cout << op << endl;
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

int checkNullTag(tag_t tag) {
	if (tag == NULLTAG)
	{
		exit(0);
	}
	else
	{
		cout << "tag ok\n";
		return 0;
	}
}