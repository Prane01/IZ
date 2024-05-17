#include<conio.h>
#include<iostream>
#include<stdio.h>
#include<tccore/tctype.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<tc/tc_startup.h>
#include<tcinit\tcinit.h>
#include<tccore/item.h>
#include<tc\emh.h>
#include<stdlib.h>
#include<string.h> 
#include<ae/dataset.h>
using namespace std;

int iFail = 0;
char* cError = NULL;

void display();

int ITK_user_main(int argc, char** argv)
{
	
	tag_t tdataset = NULLTAG;
	
	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return iFail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			AE_find_dataset2("Testing", &tdataset); // finding dataset by name
			if (tdataset != NULLTAG)
			{
				printf("\n\ndataset found!\n\n");
				//AOM_refresh(tdataset, true);
				iFail = AE_export_named_ref(tdataset, "PDF_Reference", "D:\\check\\new1.pdf"); // exporting named reference
				//cout << iFail << '\n';
				//iFail = AOM_save_without_extensions(tdataset);
				//cout << iFail << '\n';
				//iFail = AOM_refresh(tdataset, false);
				//cout << iFail << '\n';

				printf("\n\n NamedReference Added Succesfully!\n\n");
			}
			else
			{
				EMH_ask_error_text(iFail, &cError);
				cout << "Error: " << cError;
			}
		}
		else
		{
			EMH_ask_error_text(iFail, &cError);
			cout << "Error: " << cError;
		}
	}
	else
	{
		EMH_ask_error_text(iFail, &cError);
		cout << "Error: " << cError;
	}
	return iFail;
}

void display() {
	cout << "\n -u\t ---> Enter username";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
}