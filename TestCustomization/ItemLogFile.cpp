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
using namespace std;

int iFail = 0;
char* cError = NULL;

void display();

int checkITK_OK() {
	if (iFail != ITK_ok)
	{
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : " << cError;
		exit(0);
	}
	else {
		cout << "\n\n Execution success\n";
		return iFail;
	}
}

int ITK_user_main(int argc, char* argv[])
{
	FILE* fp = NULL;
	tag_t tItem = NULLTAG;
	tag_t tRevision = NULLTAG;

	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return iFail;
	}

	fp = TC_fopen("C:\\Users\\T50372\\Desktop\\Customsyslog\\logfileitem.log", "w");

	TC_fprintf(fp, "\n **********************************************************");

	TC_fprintf(fp, "\n ------------------- ITK log file -------------------");

	TC_fprintf(fp, "\n **********************************************************\n");

	if (argc == 4) { // Syslog.c is fourth argument or file name

		if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
			if (ITK_init_module(uname, pass, grp) == ITK_ok) {

				cout << "\nLogin successful...\n\n";
				TC_fprintf(fp, "\n\n Teamcenter Login Successful...");

				iFail = ITEM_create_item("152447", "ITK Test", "Item", "A", &tItem, &tRevision);
				if (iFail == ITK_ok) {
					cout << "\n\n Item Created Successfully..\n";
					TC_fprintf(fp, "\n\n Item Created Successfully...");

					iFail = ITEM_save_item(tItem);
					if (iFail == ITK_ok) {
						cout << "\n\n Item Saved Successfully..\n";
						TC_fprintf(fp, "\n\n Item Saved Successfully...");
					}
					else {
						EMH_ask_error_text(iFail, &cError);
						cout << "\n\n The error is : " << cError;
						TC_fprintf(fp, "\n\n The error is : %s", cError);
					}
				}
				else {
					EMH_ask_error_text(iFail, &cError);
					cout << "\n\n The error is : " << cError;
					TC_fprintf(fp, "\n\n The error is : %s", cError);
				}

			}
			else
			{
				EMH_ask_error_text(iFail, &cError);
				cout << "\n\n The error is : " << cError;
				TC_fprintf(fp, "\n\n The error is : %s", cError);
			}
		}
		else
		{
			EMH_ask_error_text(iFail, &cError);
			cout << "\n\n The error is : " << cError;
			TC_fprintf(fp, "\n\n The error is : %s", cError);
		}
	}
	else
	{
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : " << cError;
		TC_fprintf(fp, "\n\n The error is : %s", cError);
	}
	return iFail;
}
void display() {
	cout << "\n -u\t ---> Enter username";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
}