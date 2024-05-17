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
#include<qry/qry.h>
#define qName "ITK_Item_byGroup"
using namespace std;

void display();

int checkNullTag(tag_t tag);

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	tag_t tQuery = NULLTAG;
	char* test = NULL;
	int entries = 0;
	char** actEntries = NULL;
	
	int foundObj = 0;
	tag_t* results = NULLTAG;
	char* p = NULL;

	char* uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char* pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char* grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char* Sgrp = ITK_ask_cli_argument("-group=");

	char** actValues = NULL;

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			QRY_find2(qName, &tQuery); // finding query 
			checkNullTag(tQuery);
			QRY_find_user_entries(tQuery, &entries, &actEntries, &actValues); 
			if (entries != 0 && actEntries != NULLTAG && actValues != NULLTAG)
			{
				cout << entries << endl;
				for (int i = 0; i < entries; i++)
				{
					actValues = &Sgrp;
					cout << actEntries[i] << endl;
					cout << *actValues << endl << '\n';
				}
				QRY_execute(tQuery, entries, actEntries, actValues, &foundObj, &results);
				for (int j = 0; j < foundObj; j++) {
					AOM_ask_value_string(results[j], "object_string", &p);
					cout << p << endl;
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