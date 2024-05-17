#include<iostream>
#include <tcinit/tcinit.h>
#include<tc/tc_startup.h>
#include<string.h>
#include<bom/bom.h>
#include<fclasses/tc_string.h>
#include<pie/pie.h>
#include<pie/ClosureRule.hxx>
#include<tccore/aom.h>
#include<tccore/aom_prop.h>
#include<ss/ss_functions.h>
#include"MyException.h"
using namespace std;
void display();
int checkNullTag(tag_t tag);
int ITK_user_main(int arg, char* args[]) {
	MyException ex;
	tag_t tRevision = NULLTAG;
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	tag_t tSession = NULLTAG;
	int n_modes = 0;
	tag_t* tTransferMode = NULLTAG;
	tag_t tClosureRule = NULLTAG;
	tag_t tClosureRuleClass = NULLTAG;
	PIE_type_t type = PIE_class;
	PIE_connection_type_t connection_type = PIE_property;
	PIE_rule_type_t rule_type = PIE_process;
	int n_obj = 0;
	tag_t* objects = NULL;
	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username
	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password
	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group
	char *level = ITK_ask_cli_argument("-l="); // API takes user input as level
	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function
		display();
	}
	try {
		ex = ITK_init_module(uname, pass, grp);
		ex = ITEM_find_rev("000159", "A", &tRevision);
		checkNullTag(tRevision);
		cout << "Revision Found\n";
		ex = BOM_create_window(&tWindow);
		checkNullTag(tWindow);
		cout << "BOM Window Created\n";
		ex = BOM_set_window_top_line(tWindow, NULLTAG, tRevision, NULLTAG, &tTopLine);
		checkNullTag(tTopLine);
		cout << "BOM Top line has been set\n";
		ex = PIE_create_session(&tSession);
		checkNullTag(tSession);
		ex = PIE_find_transfer_mode2("Custom_BOM_Export_Level1", "Custom_BOM_Export_Level1", &n_modes, &tTransferMode);
		ex = PIE_transfer_mode_get_closures(tTransferMode[0], &tClosureRule);
		int value;
		sscanf_s(level, "%d", &value);
		value+=1;
		string rule = "CLASS.ImanItemLine:TYPE.*:PROPERTY.bl_all_child_lines:TRAVERSE_AND_PROCESS:SECONDARY.bl_level_starting_0!=";
		rule += '"';
		rule.append(to_string(value));
		rule += '"';
		const char* condition_rule = rule.c_str();
		ex = AOM_refresh(tClosureRule, true);
		ex = AOM_set_value_string_at(tClosureRule, "clauses", 0,condition_rule);
		ex = AOM_save_without_extensions(tClosureRule);
		ex = AOM_refresh(tClosureRule, false);
		ex = POM_class_of_instance(tClosureRule, &tClosureRuleClass);
		ex = POM_load_instances(1, &tClosureRule, tClosureRuleClass, POM_no_lock);
		char* values = NULL;
		ex = AOM_ask_value_string_at(tClosureRule, "clauses", 0, &values);
		cout << values << endl;
		ex = PIE_session_set_transfer_mode(tSession, tTransferMode[0]);
		ex = PIE_session_set_file(tSession, "D:\\export\\BOMExport\\BOM_specific_level_export.xml");
		ex = PIE_session_export_objects(tSession, 1, &tTopLine);
		cout << "BOM Exported Successful" << endl;
	}
	catch (MyException err) {
		err.error();
	}
	return 0;
}

void display() {
	cout << "\n -u\t ---> Enter username";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
	cout << "\n -l\t ---> Enter level";
}
int checkNullTag(tag_t tag)
{
	int ifail = 0;
	char* cError = NULL;
	if (tag == NULLTAG)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "The Error is: " << cError;
		return ifail;
	}
	else
	{
		return ITK_ok;
	}
}