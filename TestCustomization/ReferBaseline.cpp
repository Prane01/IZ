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
#include<epm/epm.h>
#include<epm/epm_task_template_itk.h>
#include<tccore/workspaceobject.h>
#include<tccore/grm.h>
#include<tc/folder.h>
#define EPM_target_attachment               1 
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
void createBaseline(tag_t tRevision);
void attach_to_references(tag_t tRevision, tag_t newRev);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{


	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char *item_id = ITK_ask_cli_argument("-t="); // API takes user input as group

	char *rev_id = ITK_ask_cli_argument("-rid="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			tag_t tRevision = NULLTAG;
			int release_val = 0;
			int status_count = 0;
			char** release_status = NULL;
			tag_t delete_status = NULLTAG;
			tag_t new_process = NULLTAG;
			int att_types = EPM_target_attachment;
			ifail = ITEM_find_rev(item_id, rev_id, &tRevision);
			checkifail();
			ifail = EPM_ask_if_released(tRevision, &release_val);
			checkifail();
			if (release_val == 1)
			{
				ifail = AOM_ask_displayable_values(tRevision, "release_status_list", &status_count, &release_status);
				ifail = EPM_find_process_template("Delete status", &delete_status);
				ifail = EPM_create_process("Delete status", NULL, delete_status, 1, &tRevision, &att_types, &new_process);
				cout << "* Deleted status\n";
				createBaseline(tRevision);
			}
			else {
				createBaseline(tRevision);
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

void createBaseline(tag_t tRevision) {
	tag_t newRev = NULLTAG;
	int deep_n = 0;
	tag_t* deep_objects = NULL;
	ifail = ITEM_baseline_rev(tRevision, NULL, "Baseline created from ITK", "BaselineTest01", NULL, NULL, &newRev, &deep_n, &deep_objects);
	checkifail();
	AOM_save_without_extensions(newRev);
	cout << "Done\n\n";
	attach_to_references(tRevision, newRev);
}

void attach_to_references(tag_t tRevision, tag_t newRev) {
	int depth = -1;
	int n_references = 0;
	int* n_levels = 0;
	tag_t* references = NULL;
	char** relations = NULL;
	tag_t relation_type = NULLTAG;
	tag_t relation = NULLTAG;
	tag_t claaId = NULLTAG;
	tag_t claaId1 = NULLTAG;
	char* className = NULL;
	char* className1 = NULL;
	int ids = 0;
	tag_t* super = NULL;
	ifail = WSOM_where_referenced2(tRevision, depth, &n_references, &n_levels, &references, &relations);
	checkifail();
	for (int i = 0; i < n_references; i++)
	{
		POM_class_of_instance(references[i], &claaId);
		POM_name_of_class(claaId, &className);
		POM_superclasses_of_class(claaId, &ids, &super);
		POM_name_of_class(super[0], &className1);
		if (tc_strcmp(className, "Folder") == 0 || tc_strcmp(className1, "Folder") == 0)
		{
			FL_insert(references[i], newRev, 999);
			AOM_save_without_extensions(references[i]);
		}
		//cout << relations << endl;
		GRM_find_relation_type(relations[i], &relation_type);
		GRM_create_relation(references[i], newRev, relation_type, NULLTAG, &relation);
		GRM_save_relation(relation);
	}
		cout << "attached\n";
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