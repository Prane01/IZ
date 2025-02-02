#include<iostream>
#include<tccore/tctype.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
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


	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			tag_t tItem = NULLTAG;
			tag_t prop = NULLTAG;
			char* proptype_n = NULL;
			int num_ele = 0;
			ifail = ITEM_find_item("000789", &tItem);
			checkifail();
			ifail = AOM_ask_descriptor(tItem, "c3List", &prop);
			checkifail();
			ifail = PROPDESC_ask_max_num_elements(prop, &num_ele);
			checkifail();
			cout << "Max no.of Elements: " << num_ele << endl;
			const char* result = (num_ele != 1) ? "Property is an Array type" : "Property is not an Array type";
			cout << result << endl;
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