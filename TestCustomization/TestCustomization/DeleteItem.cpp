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
int ifail = 0;
char* cError = NULL;

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
			tag_t tItem = NULLTAG;
			tag_t tFolder = NULLTAG;
			int n = 0;
			int nClasses = 0;
			int* levels = 0;
			int* Class_levels = 0;
			int* where_found = 0;
			int* Class_where_found = 0;
			tag_t* references = NULLTAG;
			tag_t* ref_classes = NULLTAG;
			char** relations = NULL;
			int s = 0;
			char** sss = NULL;
			int hits = 0;
			tag_t* list = NULLTAG;
			char* value = NULL;
			int relCount = 0;
			tag_t* relList = NULLTAG;
			tag_t relation = NULLTAG;
			tag_t relation2 = NULLTAG;
			int psn_Levels = -1;
			int psn_parents = 0;
			int* pslevels = 0;
			tag_t* parents = NULLTAG;
			char* parentsStr = NULLTAG;
			int revCount = 0;
			tag_t* allrevs = NULLTAG;
			int revC = 0;
			tag_t* Allrevs = NULLTAG;
			ifail = ITEM_find_item(item, &tItem);
			checkifail();
			checkNullTag(tItem);
			cout << "Item found...\n\n";

			ifail = ITEM_list_all_revs(tItem, &revC, &allrevs);

			ITK_set_bypass(true);

			ifail = WSOM_where_referenced2(tItem, WSO_where_ref_any_depth, &n, &levels, &references, &relations); // Getting all references
			cout << n << endl;
			for (int i = 0; i < n; i++)
			{
				AOM_ask_displayable_values(references[i], "object_name", &s, &sss);
				cout << sss[0] << endl; // Prining reference object name
				WSOM_find2(sss[0], &hits, &list); // Finding reference object name
				if (hits != 0)
				{
					for (int j = 0; j < hits; j++)
					{
						AOM_ask_value_string(list[j], "object_type", &value);
						if (tc_strcmp(value, "Folder") == 0) // Validating as the reference is only Folder
						{
							FL_remove(list[j], tItem); // Removing the object from folder
							AOM_save_without_extensions(list[j]); // Saving the folder
						}
						else // If reference is with other than folder coming to this block to delete it's referenced relation
						{
							GRM_list_relation_types(&relCount, &relList);
							for (int r = 0; r < revC; r++)
							{
								for (int z = 0; z < relCount; z++)
								{
									GRM_find_relation(tItem, NULLTAG, relList[z], &relation); // objects secondaries
									GRM_delete_relation(relation);
									GRM_find_relation(list[j], tItem, relList[z], &relation2); // object as secondary
									GRM_delete_relation(relation2);

								}
							}
						}
					}
				}
			}
			cout << ">>>Object removed from folders\n\n";
			cout << ">>>Objects relation with all other objects is deleted\n\n";
			cout << "Checking object in Product Structure to remove it's occurrence\n\n";

			tag_t tWindow = NULLTAG;
			tag_t tTopLine = NULLTAG;
			tag_t occ = NULLTAG;
			tag_t new_process = NULLTAG;
			tag_t process_template = NULLTAG;
			int attListC = EPM_target_attachment;
			
			ifail = ITEM_list_all_revs(tItem, &revCount, &allrevs); // getting all ItemRevisions for Item as they are only involved in PS
			checkifail();
			for (int j = 0; j < revCount; j++) // Iterating to 1 by 1 ItemRevision of Item
			{
				ifail = PS_where_used_all(allrevs[j], PS_where_used_all_levels, &psn_parents, &pslevels, &parents); // Fining Object where it is used
				checkifail();
				/*cout << psn_parents << endl;
				cout << pslevels << endl;*/
				for (int i = 0; i < psn_parents; i++) // Iterating to all the parents of the Item to remove occurrence
				{
					int bvrC = 0;
					int childCount = 0;
					tag_t* bvrs = NULLTAG;
					tag_t* children = NULLTAG;

					AOM_ask_value_string(parents[i], "object_string", &parentsStr); // Prining parent
					cout << parentsStr << endl;
					ifail = BOM_create_window(&tWindow); // creating window to get the occ in our process(mainly)
					ifail = BOM_set_window_top_line(tWindow, NULLTAG, parents[i], NULLTAG, &tTopLine); // Setting parent to top line
					//cout << "test 1\n\n";
					checkifail();
					ifail = BOM_line_ask_all_child_lines(tTopLine, &childCount, &children); // Finding and getting all child lines
					//cout << "test 2\n\n";
					checkifail();
					ifail = ITEM_rev_list_bom_view_revs(parents[i], &bvrC, &bvrs); // Getting all bvr's of parent as to use it to delete occurrence
					//cout << "test 3\n\n";
					checkifail();
					for (int bomC = 0; bomC < childCount; bomC++) // Iterating to childs
					{
						char* id = NULL;
						AOM_ask_value_string(children[bomC], "bl_item_item_id", &id); // getting child id to validate our Item in PS
						if (tc_strcmp(id, item) == 0) // validating and entering if our Item found
						{
							ifail = AOM_ask_value_tag(children[bomC], "bl_occurrence", &occ); // getting occurrence for our Item
							checkifail();
							cout << "Found Item in PS\n\n";
							//cout << bvrC << endl;
							for (int z = 0; z < bvrC; z++) // Iterating to all bvr's of parent to delete our Item in every parent bvr in which it is present
							{
								AOM_ask_value_string(parents[i], "bl_item_item_id", &id);
								if (tc_strcmp(id, item) == 0)
								{
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