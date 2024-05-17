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
#include<tc/envelope.h>
#include<sa/user.h>
#include<tc/folder.h>
#define date "11-Sep-2023 00:00"
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t tEnvelope = NULLTAG;
	tag_t tReceiver = NULLTAG;
	tag_t tCCReceiver = NULLTAG;
	tag_t tQuery = NULLTAG;
	tag_t opPR = NULLTAG;
	int u = 1;
	int n_entries = 0;
	char** n_UserEntry = NULL;
	char** n_UserValue = NULL;
	int n_found = 0;
	tag_t* result = NULLTAG;
	char* typeE = NULLTAG;
	tag_t createInput = NULLTAG;

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

			TCTYPE_ask_type("Envelope", &tEnvelope);
			checkNullTag(tEnvelope);
			AOM_ask_value_string(tEnvelope, "object_type", &typeE);
			cout << "Type: " << typeE << endl;
			TCTYPE_construct_create_input(tEnvelope, &createInput);
			checkNullTag(createInput);
			cout << "createInput done" << endl << endl;
			AOM_set_value_string(createInput, "object_name", "TestingFITK");
			cout << "object name set" << endl << endl;
			AOM_set_value_string(createInput, "object_desc", "TestingFITK");
			cout << "object desc set" << endl << endl;
			TCTYPE_create_object(createInput, &opPR);
			checkNullTag(opPR);

			MAIL_create_envelope("Today's Items", "list of today created items", &tEnvelope);
			SA_find_user2("izn", &tReceiver);
			checkNullTag(tReceiver);
			SA_find_user2("infodba", &tCCReceiver);
			checkNullTag(tCCReceiver);
			MAIL_add_envelope_receiver(tEnvelope, tReceiver);
			cout << "Receiver added to envelope\n\n";
			MAIL_add_envelope_cc_receiver(tEnvelope, tCCReceiver);
			cout << "CC Receiver added to envelope\n\n";
			QRY_find2("ITK_TodayItems", &tQuery);
			QRY_find_user_entries(tQuery, &n_entries, &n_UserEntry, &n_UserValue);

			for (int i = 0; i < n_entries; i++)
			{
				cout << n_UserEntry[i] << endl;
				strcpy(n_UserValue[i], date);
			}
			QRY_execute(tQuery, u, n_UserEntry, n_UserValue, &n_found, &result);

			for (int j = 0; j < n_found; j++)
			{
				FL_insert(tEnvelope, result[j], 999);
			}
			MAIL_send_envelope(tEnvelope);
			cout << "Envelope sent successfully...\n\n";
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
}

int checkifail()
{
	if (ifail != ITK_ok)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "Error: " << cError;
		exit(0);
	}
	else
	{
		return 0;
	}
}