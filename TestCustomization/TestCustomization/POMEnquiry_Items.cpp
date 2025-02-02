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
#include<pom/enq/enq.h>
#include<base_utils/ScopedSmPtr.hxx>
#include<vector>
#include"MyException.h"
using namespace std;
void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;
MyException exObj;

int ITK_user_main(int argc, char* argv[])
{
	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username
	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password
	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group
	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function
		display();
	}
	try {
		vector<tag_t> listOfObjects;
		exObj = ITK_init_module(uname, pass, grp);
		const char* searchQueryName = "find_object_names_by_item_id";
		const char* searchObjectType = "WorkspaceObject";
		exObj = POM_enquiry_create(searchQueryName);
		const char* select_attr[] = { "object_name", "object_type" };
		exObj = POM_enquiry_add_select_attrs(searchQueryName, searchObjectType, 2, select_attr);
		const char* ItemID[] = { "001444" };
		date_t afterDate = NULLDATE;
		exObj = ITK_string_to_date("05-Jun-2024 23:59", &afterDate);
		exObj = POM_enquiry_set_date_value(searchQueryName, "bindExp", 1, &afterDate, POM_enquiry_bind_value);
		exObj = POM_enquiry_set_attr_expr(searchQueryName, "ItemIDExp", searchObjectType, "creation_date", POM_enquiry_greater_than, "bindExp");
		exObj = POM_enquiry_set_where_expr(searchQueryName, "ItemIDExp");
		int n_rows, n_cols = 0;
		void ***values;
		exObj = POM_enquiry_execute(searchQueryName, &n_rows, &n_cols, &values);
		exObj = POM_enquiry_delete(searchQueryName);
		for (int i = 0; i < n_rows; i++) {
			void ** column = values[i];
			for (int j = 0; j < n_cols; j++) {
				printf("%s ", column[j]);
			}
			cout << "\n";
		}
	}
	catch (MyException exObj) {
		exObj.error();
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