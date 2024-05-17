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
#include<sstream>
#include<tccore/tctype.h>
using namespace std;

void display();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t tItem = NULLTAG;
	tag_t tCretaeInput = NULLTAG;
	tag_t* bulkCreIn = NULL;
	tag_t* n_objects = NULLTAG;
	int* statuses = 0;
	char** error_message = NULL;
	tag_t tCretadObj = NULLTAG;
	int count = 0;

	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group
	fstream fin;

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}
	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			fin.open("C:\\Users\\T50372\\Desktop\\test.txt", ios::in);
			if (fin.is_open)
			{
				string line;
				while (getline(fin, line))
				{
					char* char_array = new char[line.length() + 1];
					strcpy(char_array, line.c_str());
					TCTYPE_ask_type("Item", &tItem);
					checkNullTag(tItem);
					TCTYPE_construct_create_input(tItem, &tCretaeInput);
					checkNullTag(tCretaeInput);
					TCTYPE_create_object(tCretaeInput, &tCretadObj);

					AOM_set_value_string(tCretadObj, "object_name", char_array);
					AOM_save_with_extensions(tCretadObj);
					count++;
				}
				fin.close();
				cout << count << " - Item's created" << endl;
			}
			else {
				cout << "cannot open file..\n";
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