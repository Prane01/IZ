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
		const char* subSearchObjectType = "Item";
		int n_rows, n_cols = 0;
		void ***values;
		 
		exObj = POM_enquiry_create(searchQueryName);
		const char* select_attr[] = {"puid","object_name", "object_type" };
		//const char *select_attrs_Item[] = { "item_id" };
		const char* for_left_join = "puid";
		exObj = POM_enquiry_add_select_attrs(searchQueryName, searchObjectType, 3, select_attr);
		//exObj = POM_enquiry_add_select_attrs(searchQueryName, subSearchObjectType, 1, select_attrs_Item);
		const char* ItemID[] = { "001444" };
		exObj = POM_enquiry_set_string_value(searchQueryName, "bindExp", 1, ItemID, POM_enquiry_bind_value);
		exObj = POM_enquiry_set_join_expr(searchQueryName, "joinExp", searchObjectType, for_left_join, POM_enquiry_equal, subSearchObjectType, for_left_join);
		exObj = POM_enquiry_set_attr_expr(searchQueryName, "itemIdExp", subSearchObjectType, "item_id", POM_enquiry_equal, "bindExp");
		exObj = POM_enquiry_set_expr(searchQueryName, "combine_two", "joinExp", POM_enquiry_and, "itemIdExp");
		exObj = POM_enquiry_set_where_expr(searchQueryName, "combine_two");
		exObj = POM_enquiry_execute(searchQueryName, &n_rows, &n_cols, &values);
		exObj = POM_enquiry_delete(searchQueryName);
		for (int i = 0; i < n_rows; i++) {
			void ** column = values[i];
			for (int j = 0; j < n_cols; j++) {
				if (j == 0)
				{
					printf("%d	", column[j]);
				}
				else {
					printf("%s	", column[j]);
				}
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