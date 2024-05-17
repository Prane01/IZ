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
using namespace std;

void display();
int checkNullTag(tag_t tag);
int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	tag_t tCretaeInput = NULLTAG;
	tag_t* bulkCreIn = NULL;
	tag_t* n_objects = NULLTAG;
	int* statuses = 0;
	char** error_message = NULL;
	tag_t tCretadObj = NULLTAG;


	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char *object01 = ITK_ask_cli_argument("-o1="); // API takes user input as group
	char *object02 = ITK_ask_cli_argument("-o2="); // API takes user input as group
	char *object03 = ITK_ask_cli_argument("-o3="); // API takes user input as group
	char *object04 = ITK_ask_cli_argument("-o4="); // API takes user input as group
	char *object05 = ITK_ask_cli_argument("-o5="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			for (int i = 1; i <= 5; i++) {
				TCTYPE_ask_type("Item", &tItem);
				checkNullTag(tItem);
				TCTYPE_construct_create_input(tItem, &tCretaeInput);
				checkNullTag(tCretaeInput);
				TCTYPE_create_object(tCretaeInput, &tCretadObj);
				switch (i)
				{
				case 1:
					AOM_set_value_string(tCretadObj, "object_name", object01);
					AOM_save_with_extensions(tCretadObj);
					cout << i << " - Item created" << endl;
					break;
				case 2:
					AOM_set_value_string(tCretadObj, "object_name", object02);
					AOM_save_with_extensions(tCretadObj);
					cout << i << " - Item created" << endl;
					break;
				case 3:
					AOM_set_value_string(tCretadObj, "object_name", object03);
					AOM_save_with_extensions(tCretadObj);
					cout << i << " - Item created" << endl;
					break;
				case 4:
					AOM_set_value_string(tCretadObj, "object_name", object04);
					AOM_save_with_extensions(tCretadObj);
					cout << i << " - Item created" << endl;
					break;
				case 5:
					AOM_set_value_string(tCretadObj, "object_name", object05);
					AOM_save_with_extensions(tCretadObj);
					cout << i << " - Item created" << endl;
					break;
				default:
					break;
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
		exit(0);
	}
	else
	{
		cout << "tag ok\n";
		return 0;
	}
	return ITK_ok;
}