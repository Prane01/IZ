#include<iostream>
#include<fstream>
#include<tccore/tctype.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<tc/tc_startup.h>
#include<tcinit\tcinit.h>
#include<tccore/item.h>
#include<tc\emh.h>
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);

int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t type_t = NULLTAG;
	tag_t* createInput_t = NULL;
	tag_t* created_objects_t = NULL;
	int count = 0;
	const char* item_id;

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
			const char* objectTypes[] = { "Item","PDF","Text" };
			int length = sizeof(objectTypes);
			int no_of_created_objects = 0;
			cout << "Length : " << length << endl;
			for (int i = 0; i < length; i++)
			{
				cout << "Type : " << objectTypes[i] << endl;
				TCTYPE_ask_type(objectTypes[i], &type_t);
				checkNullTag(type_t);
				TCTYPE_construct_create_input(type_t, &createInput_t[i]);
				checkNullTag(createInput_t[i]);
			}

			TCTYPE_create_objects(sizeof(createInput_t), &length, createInput_t, &no_of_created_objects,&created_objects_t);
			//checkNullTag(created_objects_t);
			//AOM_set_value_string(createdObj1, "object_name", "Hii2");
			AOM_save_without_extensions(createdObj1);
			cout << "Object created successfully...";

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