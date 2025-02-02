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
#include<tccore/workspaceobject.h>
#include<tc/folder.h>
#include<fclasses/tc_string.h>
#include<tccore/grm.h>
#include<ps/ps.h>
#include<bom/bom.h>
#include<epm/epm.h>
#include<epm/epm_task_template_itk.h>
//#define uname "izn"
//#define pass "izn"
//#define grp "dba"
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
void check_WhereReferenced(tag_t rev);
void check_WhereUsed(tag_t rev, char* item_id);
int ifail = 0;
char* cError = NULL;
tag_t tItem = NULLTAG;

int ITK_user_main(int argc, char* argv[])
{


	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	char *item = ITK_ask_cli_argument("-t="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			int revC = 0;
			tag_t* allrevs = NULLTAG;

			ifail = ITEM_find_item(item, &tItem);
			checkifail();
			checkNullTag(tItem);
			cout << "Item found...\n\n";

			ifail = ITEM_list_all_revs(tItem, &revC, &allrevs);
			ITK_set_bypass(true);

			for (int i = 0; i < revC; i++)
			{
				check_WhereReferenced(allrevs[i]);
				check_WhereUsed(allrevs[i], item);
			}
			ifail = ITEM_delete_item(tItem);
			checkifail();
			cout << "Item Deleted successfully...\n\n";
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

void check_WhereReferenced(tag_t rev) {
	tag_t* references = NULLTAG;
	char** relations = NULL;
	tag_t relType = NULLTAG;
	tag_t relation = NULLTAG;
	tag_t class_id = NULLTAG;
	int n = 0;
	int* levels = 0;
	char* type = NULL;
	char* className = NULL;
	char* className1 = NULL;

	ifail = WSOM_where_referenced2(rev, WSO_where_ref_any_depth, &n, &levels, &references, &relations); // Getting all references
	cout << n << endl;
	for (int i = 0; i < n; i++)
	{
		int ids = 0;
		tag_t* supIds = NULLTAG;
		POM_class_of_instance(references[i], &class_id);
		POM_name_of_class(class_id, &className);
		POM_superclasses_of_class(class_id, &ids, &supIds);
		POM_name_of_class(supIds[0], &className1);
		if (tc_strcmp(className, "Folder") == 0 || tc_strcmp(className1, "Folder") == 0) // Validating as the reference is only Folder
		{
			FL_remove(references[i], tItem); // Removing the object from folder
			AOM_save_without_extensions(references[i]); // Saving the folder
		}
		else // If reference is with other than folder coming to this block to delete it's referenced relation
		{
			GRM_find_relation_type(relations[i], &relType);
			GRM_find_relation(references[i], rev, relType, &relation); // object as secondary
			GRM_delete_relation(relation);
		}
	}
}

void check_WhereUsed(tag_t rev, char* item_id) {
	int psn_parents = 0;
	int* pslevels = 0;
	tag_t* parents = NULLTAG;
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	tag_t occ = NULLTAG;
	
	ifail = PS_where_used_all(rev, PS_where_used_all_levels, &psn_parents, &pslevels, &parents); // Fining Object where it is used
	checkifail();
	for (int i = 0; i < psn_parents; i++) // Iterating to all the parents of the Item to remove occurrence
	{
		int bvrC = 0;
		int childCount = 0;
		tag_t* bvrs = NULLTAG;
		tag_t* children = NULLTAG;

		ifail = BOM_create_window(&tWindow); // creating window to get the occ in our process(mainly)
		checkifail();
		ifail = BOM_set_window_top_line(tWindow, NULLTAG, parents[i], NULLTAG, &tTopLine); // Setting parent to top line
		checkifail();
		ifail = BOM_line_ask_all_child_lines(tTopLine, &childCount, &children); // Finding and getting all child lines
		checkifail();
		ifail = ITEM_rev_list_bom_view_revs(parents[i], &bvrC, &bvrs); // Getting all bvr's of parent as to use it to delete occurrence
		checkifail();
		for (int bomC = 0; bomC < childCount; bomC++) {  // Iterating to childs
			char* id = NULL;
			AOM_ask_value_string(children[bomC], "bl_item_item_id", &id); // getting child id to validate our Item in PS
			if (tc_strcmp(id, item_id) == 0) { // validating and entering if our Item found
				ifail = AOM_ask_value_tag(children[bomC], "bl_occurrence", &occ); // getting occurrence for our Item
				checkifail();
				for (int z = 0; z < bvrC; z++) { // Iterating to all bvr's of parent to delete our Item in every parent bvr in which it is present
					AOM_ask_value_string(parents[i], "bl_item_item_id", &id);
					if (tc_strcmp(id, item_id) == 0){
						ifail = PS_delete_bvr(bvrs[z]);
						checkifail();
					}
					else {
						ifail = PS_delete_occurrence(bvrs[z], occ); // Deleting occurrence of Item
						checkifail();
						cout << "Deleted occurrence\n\n";
						AOM_save_without_extensions(bvrs[z]); // Saving bvr
						BOM_save_window(tWindow); // Saving window
					}
				}
			}
		}
		BOM_close_window(tWindow); // Closing the window to open for next parent to repeat the process
	}
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