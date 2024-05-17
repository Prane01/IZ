#include<iostream>
#include<string.h> 
#include<string>
#include<fstream>
#include<tccore/tctype.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<tc/tc_startup.h>
#include<tcinit\tcinit.h>
#include<tccore/item.h>
#include<tc\emh.h>
#include<tc/folder.h>
using namespace std;

int ifail = 0;
char* cError = NULL;
void display();
int checkifail();
int checkNullTag(tag_t tag);

int ITK_user_main(int argc, char* argv[])
{
	tag_t tObject = NULLTAG;
	tag_t createdObj = NULLTAG;
	tag_t createdObj1 = NULLTAG;
	tag_t tRelationType = NULLTAG;
	tag_t* tItemMasterForm = NULLTAG;
	int hits = 0;
	tag_t* list = NULL;
	char* object_name = NULL;
	char* object_type = NULL;
	int count = 0;

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
			int create = 1;
			while (create == 1) {
				cout << "Type to create: ";
				string createTypeS;
				cin >> createTypeS;
				const char* createTypeC = createTypeS.c_str();
				ifail = TCTYPE_ask_type(createTypeC, &tObject);
				checkifail();
				checkNullTag(tObject);
				ifail = TCTYPE_construct_create_input(tObject, &createdObj);
				checkifail();
				checkNullTag(createdObj);
				ifail = TCTYPE_create_object(createdObj, &createdObj1);
				checkifail();
				checkNullTag(createdObj1);
				cout << "Enter Item Id: ";
				string item_idS;
				cin >> item_idS;
				const char*  item_idC = item_idS.c_str();
				ifail = AOM_set_value_string(createdObj1, "item_id", item_idC);
				checkifail();
				cout << "Enter Name: ";
				string object_nameS;
				cin >> object_nameS;
				const char*  object_nameC = object_nameS.c_str();
				ifail = AOM_set_value_string(createdObj1, "object_name", object_nameC);
				checkifail();
				cout << "Enter Description: ";
				string object_descS;
				cin >> object_descS;
				const char*  object_descC = object_descS.c_str();
				ifail = AOM_set_value_string(createdObj1, "object_desc", object_descC);
				checkifail();
				ifail = AOM_save_without_extensions(createdObj1);
				checkifail();
				ifail = WSOM_find2("ITK_USE_izn", &hits, &list);
				checkifail();
				for (int i = 0; i < hits; i++) {
					AOM_ask_value_string(list[i], "object_name", &object_name);
					cout << "Object name: " << object_name << '\n';

					AOM_ask_value_string(list[i], "object_type", &object_type);
					cout << "Type: " << object_type << '\n';

					if (strcmp(object_type, "Folder") == 0)
					{
						ifail = FL_insert(*list, createdObj1, 0);
						AOM_save_without_extensions(*list);
						cout << "\n\n Item Saved Successfully..\n";
					}
					else {
						EMH_ask_error_text(ifail, &cError);
						cout << "\n\n The error is : " << cError;
					}
				}
				cout << "Object created successfully...";
				cout << "Press 1 to create, other to stop: ";
				cin >> create;
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