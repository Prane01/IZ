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
int check_ITK_Error(int iFail);

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
		//const char* select_attr[] = {"puid","object_name", "object_type" };
		const char* select_attr[] = {"puid","object_name"};
		//const char *select_attrs_Item[] = { "item_id" };
		const char* for_left_join = "puid";
		//exObj = POM_enquiry_add_select_attrs(searchQueryName, searchObjectType, 3, select_attr);
		//exObj = POM_enquiry_add_select_attrs(searchQueryName, subSearchObjectType, 1, select_attrs_Item);
		const char* ItemID[] = { "001444" };

		/*exObj = POM_enquiry_set_string_value(searchQueryName, "bindExp", 1, ItemID, POM_enquiry_bind_value);
		exObj = POM_enquiry_set_join_expr(searchQueryName, "joinExp", searchObjectType, for_left_join, POM_enquiry_equal, subSearchObjectType, for_left_join);
		exObj = POM_enquiry_set_attr_expr(searchQueryName, "itemIdExp", subSearchObjectType, "item_id", POM_enquiry_equal, "bindExp");
		exObj = POM_enquiry_set_expr(searchQueryName, "combine_two", "joinExp", POM_enquiry_and, "itemIdExp");
		exObj = POM_enquiry_set_where_expr(searchQueryName, "combine_two");
		exObj = POM_enquiry_execute(searchQueryName, &n_rows, &n_cols, &values);
		exObj = POM_enquiry_delete(searchQueryName);*/

		const char* enqName1 = "GetAMPLDataByPOMQuery";

		const char* className = "Item";

		const char* dStratEnqExp = "ExprId1";
		const char* dStartValueExp = "val1";


		const char* dEndEnqExp = "ExprId2";
		const char* dEndValueExp = "val2";

		const char* dStartEndEnqVar = "dStartEndEnqVar";
		const char* exp3 = "expo3";

		const char* cpstartDate = "01-Mar-2024 00:00";
		const char* cpendDate = "02-Feb-2025 00:00";
		const char* searchCriteria = "creation_date";


		date_t startDate, endDate;


		/* create two date value objects using local time zone dates */
		check_ITK_Error(ITK_string_to_date(cpstartDate, &startDate));
		check_ITK_Error(ITK_string_to_date(cpendDate, &endDate));


		check_ITK_Error(POM_enquiry_create(enqName1));


		//MPN Revsion attributes divided into Storage property and AOM properties.

		string mpnRevTypePropertiesAOMapi[] = { "item_id|String", "h4_vendor_name|String","project_list|StringTemp","h4_design_authority|String","h4_vendor_id|String","h4_comment|String","owning_project|Reference" };

		//const char * mpnRevClassAttributes[] = { "gov_classification","object_type", "item_revision_id", "object_name", "object_desc", "h4_strategic_business_grp", "h4_strategic_business_unit", "h4_gold_business_enterprise", "h4_line_of_business", "h4_status_rating", "h4_mpn_type","puid" };

		//string mpnRevClassAttributesWithType[] = { "gov_classification|String","object_type|String","item_revision_id|String", "object_name|String", "object_desc|String", "h4_strategic_business_grp|String", "h4_strategic_business_unit|String", "h4_gold_business_enterprise|String", "h4_line_of_business|String", "h4_status_rating|String", "h4_mpn_type|String","puid|Uid" };

		const char * mpnRevClassAttributes[] = {"object_type","item_id"};

		string mpnRevClassAttributesWithType[] = { "object_type|String","item_revision_id|String", "object_name|String", "object_desc|String", "h4_strategic_business_grp|String", "h4_strategic_business_unit|String", "h4_gold_business_enterprise|String", "h4_line_of_business|String", "h4_status_rating|String", "h4_mpn_type|String","puid|Uid" };



		int sizeOfSelAtt = sizeof(mpnRevClassAttributes) / sizeof(mpnRevClassAttributes[0]);
		//printf("\n  mpnRevClassAttributes sizeOfSelAtt =%d", sizeOfSelAtt);//11

		int sizeOfSelAtt1 = sizeof(mpnRevClassAttributesWithType) / sizeof(mpnRevClassAttributesWithType[0]);
		//printf("\n  mpnRevClassAttributesWithType sizeOfSelAtt =%d : %d", sizeOfSelAtt, sizeOfSelAtt1);//11

		int mpnRevTypePropertiesAOMapiSIZE = sizeof(mpnRevTypePropertiesAOMapi) / sizeof(mpnRevTypePropertiesAOMapi[0]);
		//printf("\n  mpnRevTypePropertiesAOMapi sizeOfSelAtt =%d : %d", sizeOfSelAtt, sizeOfSelAtt1);//11

		check_ITK_Error(POM_enquiry_add_select_attrs(enqName1, className, 2, mpnRevClassAttributes));
		check_ITK_Error(POM_enquiry_set_date_value(enqName1, dStartValueExp, 1, &startDate, POM_enquiry_bind_value));
		check_ITK_Error(POM_enquiry_set_date_value(enqName1, dEndValueExp, 1, &endDate, POM_enquiry_bind_value));
		check_ITK_Error(POM_enquiry_set_attr_expr(enqName1, dStratEnqExp, className, searchCriteria, POM_enquiry_greater_than_or_eq, dStartValueExp));
		check_ITK_Error(POM_enquiry_set_attr_expr(enqName1, dEndEnqExp, className, searchCriteria, POM_enquiry_less_than_or_eq, dEndValueExp));
		check_ITK_Error(POM_enquiry_set_expr(enqName1, dStartEndEnqVar, dStratEnqExp, POM_enquiry_and, dEndEnqExp));
		check_ITK_Error(POM_enquiry_set_where_expr(enqName1, dStartEndEnqVar));

		int rows = 0;
		int cols = 0;
		void*** results;
		int count = 0;
		check_ITK_Error(POM_enquiry_execute(enqName1, &rows, &cols, &results));

		for (int i = 0; i < rows; i++) {
			void ** column = results[i];

			for (int j = 0; j < cols; j++) {	
					printf("%s	", column[j]);
					count++;
			}	
			cout << "\n";
		}
		
			cout << "count :" << count << endl;
	}
	catch (MyException exObj) {
		exObj.error();
	}
	return ifail;
}

int check_ITK_Error(int iFail) {

	if (iFail != 0) {
		char* err_msg = NULL;
		EMH_ask_error_text(iFail, &err_msg);
		printf("\n ERROR : ITK API Failed : Error = %s \n", err_msg);
	}
	return iFail;
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