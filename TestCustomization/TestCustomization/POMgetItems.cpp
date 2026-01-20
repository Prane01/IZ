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
#include<base_utils/IFail.hxx>
#include<base_utils/TcResultStatus.hxx>
#include<base_utils/ScopedPtr.hxx>
#include<base_utils/ScopedSmPtr.hxx>
using namespace std;
using namespace Teamcenter;
void display();
int checkifail();
int checkNullTag(tag_t tag);

int ITK_user_main(int argc, char* argv[])
{
	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username
	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password
	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group
	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function
		display();
	}

	int ifail = 0;
	ResultStatus status(0);
	try {
		

		vector<tag_t> listOfObjects;
		status = ITK_init_module(uname, pass, grp);
		const char* searchQueryName = "find_objecs_by_item_id";
		const char* searchObjectType = "WorkspaceObject";

		status = POM_enquiry_create(searchQueryName);
		const char* select_attr[] = { "object_name", "puid" };

		status = POM_enquiry_add_select_attrs(searchQueryName, searchObjectType, 2, select_attr);
		const char* ItemID[] = { "000048" };
		date_t afterDate = NULLDATE;
		/*status = ITK_string_to_date("05-Jun-2024 23:59", &afterDate);
		status = POM_enquiry_set_date_value(searchQueryName, "bindExp", 1, &afterDate, POM_enquiry_bind_value);*/
		status = POM_enquiry_set_char_value(searchQueryName, "bindExp", 1, ItemID[0], POM_enquiry_bind_value);
		status = POM_enquiry_set_attr_expr(searchQueryName, "ItemIDExp", searchObjectType, ItemID[0], POM_enquiry_equal, "bindExp");
		status = POM_enquiry_set_where_expr(searchQueryName, "ItemIDExp");
		int n_rows, n_cols = 0;
		void ***values;
		status = POM_enquiry_execute(searchQueryName, &n_rows, &n_cols, &values);
		status = POM_enquiry_delete(searchQueryName);
		for (int i = 0; i < n_rows; i++) {
			void ** column = values[i];
			for (int j = 0; j < n_cols; j++) {
				printf("%s ", column[j]);
			}
			cout << "\n";
		}
	}
	catch (IFail &ex) {
		ifail = ex.ifail();
		ITKCALL(ifail);
		scoped_smptr <char> message;
		EMH_ask_error_text(ifail, &message);
		//writeToFile(message.getString());
		TC_write_syslog("\nThe error is %s", message.getString());
		cout << "\nThe error is " << message.getString();
	}
	return ifail;
}
void display() {
	cout << "\n -u\t ---> Enter username";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
}
