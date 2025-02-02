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
#include<bom/bom.h>
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t tTop = NULLTAG;
	tag_t tRevision = NULLTAG;
	tag_t tLevel = NULLTAG;
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	tag_t ve = NULLTAG;
	tag_t veb = NULLTAG;
	int cc = 0;
	tag_t* children = NULL;
	tag_t clause_list = NULLTAG;
	char* item_id = NULL;
	tag_t toption = NULLTAG;
	tag_t toptionRev = NULLTAG;
	tag_t condition_ve = NULLTAG;
	tag_t load_if_ve = NULLTAG;
	tag_t block = NULLTAG;

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
			int index = 0;
			ITEM_find_rev("000355", "A", &tTop);
			checkNullTag(tTop);
			cout << "Found Top line...\n\n";
			BOM_create_window(&tWindow);
			checkNullTag(tTop);
			cout << "Created Window...\n\n";
			BOM_set_window_top_line(tWindow, NULLTAG, tTop, NULLTAG, &tTopLine);
			checkNullTag(tTop);
			cout << "Created Top line on window...\n\n";
			BOM_line_ask_all_child_lines(tTopLine, &cc, &children);
			const char* option_values[] = { "test_01", "test_02" };
			if (cc > 0)
			{
				cout << "Childs found: " << cc << endl;
				ifail = BOM_new_option(tTop, "CHOOSE01_for_786", "CHOOSE01_for_786", BOM_option_mode_enum, &toption, &toptionRev, &ve, &veb);
				checkifail();
				for (int i = 0; i < cc; i++)
				{
					BOM_add_option_rev_value(toptionRev, option_values[i], &index);
				}
				AOM_save_without_extensions(toptionRev);
				AOM_save_without_extensions(toption);
				AOM_save_without_extensions(tTop);
				AOM_save_without_extensions(ve);
				AOM_save_without_extensions(veb);
				for (int i = 0; i < cc; i++)
				{
					ifail = BOM_variant_new_clause_list(tWindow, &clause_list);
					checkifail();
					ifail = BOM_variant_clause_append(clause_list, BOM_variant_operator_and, toption, BOM_variant_operator_is_equal, option_values[i]);
					checkifail();
					ifail = BOM_variant_join_clause_list(clause_list, &condition_ve);
					checkifail();
					ifail = BOM_variant_expr_load_if(condition_ve, &load_if_ve);
					checkifail();
					ifail = BOM_variant_delete_clause_list(clause_list);
					checkifail();
					ifail = BOM_new_variant_e_block(&block);
					checkifail(); 
					ifail = BOM_variant_e_block_add(block, load_if_ve);
					checkifail();
					ifail = BOM_line_set_variant_e_block(children[i], block);
					checkifail();
					AOM_save_without_extensions(load_if_ve);
					AOM_save_without_extensions(block);
				}
				AOM_save_without_extensions(toptionRev);
				AOM_save_without_extensions(toption);
				AOM_save_without_extensions(tTop);
				BOM_save_window(tWindow);
				AOM_unlock(tTopLine);
				AOM_unlock(tTop);
			}
			else
			{
				cout << "No childs found...\n\n";
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