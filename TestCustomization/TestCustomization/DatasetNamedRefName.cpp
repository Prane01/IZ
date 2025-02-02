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
#include<ae/dataset.h>
#include<ae/ae.h>
//#define uname "izn"
//#define pass "izn"
//#define grp "dba"
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char *name = ITK_ask_cli_argument("-n="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			tag_t tdataset = NULLTAG;
			int n = 0;
			tag_t* nam_refs = NULLTAG;
			const char* dataset_name = "QWER";
			char* ref_name = NULL;
			char name1[100];

			ifail = AE_find_dataset2(dataset_name, &tdataset);
			checkifail();
			ifail = AE_ask_all_dataset_named_refs2(tdataset, "PDF_Reference", &n, &nam_refs);
			checkifail();
			cout << n << endl;
			for (int i = 0; i < n; i++)
			{
				char num[100];
				sprintf(num, "%d", i+1);
				tc_strcpy(name1, name);
				tc_strcat(name1, num);
				AOM_ask_value_string(nam_refs[i], "original_file_name", &ref_name);
				cout << ref_name << endl;
				AOM_refresh(nam_refs[i], true);
				AOM_set_value_string(nam_refs[i], "original_file_name", name1);
				AOM_save_without_extensions(nam_refs[i]);
				AOM_refresh(nam_refs[i], false);
				AOM_ask_value_string(nam_refs[i], "original_file_name", &ref_name);
				cout << ref_name << endl;
				empty(name1);
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
		cout << "The Error is: " << cError;
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