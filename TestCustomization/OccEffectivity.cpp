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
#include<ps/ps.h>
#include<cfm/cfm.h>
#include<fclasses/tc_date.h>
#include<bom/bom.h>
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
	tag_t* tbvrs = NULLTAG;
	tag_t tbvr = NULLTAG;
	tag_t tocc = NULLTAG;
	tag_t teff = NULLTAG;
	tag_t trev = NULLTAG;
	tag_t trevP = NULLTAG;
	tag_t tItem = NULLTAG;
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	tag_t* childs = NULLTAG;
	int rCount = 0;
	int child = 0;
	string fromS = "01-Aug-2023 00:00";
	char* from = new char[fromS.length()+1];
	string toS = "31-Aug-2023 00:00";
	char* to = new char[toS.length() + 1];
	date_t* fromD = new date_t[1];
	date_t* toD = new date_t[1];;
	bool f;
	bool t;
	tag_t childss = NULLTAG;
	char* id = NULL;
	strcpy(from, fromS.c_str());
	strcpy(to, toS.c_str());

	DATE_string_to_date_t(from, &f, &fromD[0]);
	DATE_string_to_date_t(to, &t, &toD[0]);

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

			ifail = ITEM_find_item("000374", &tItem);
			checkifail();
			ifail = ITEM_find_rev("000374", "A", &trevP);
			checkNullTag(tItem);
			checkifail();
			cout << "Found Item\n\n";
			ifail = ITEM_find_rev("000376", "A", &trev);
			checkNullTag(trev);
			checkifail();
			cout << "Found ItemRevision to add occ eff\n\n";

			ifail = ITEM_rev_list_bom_view_revs(trevP, &rCount, &tbvrs);
			checkifail();
			tbvr = tbvrs[0];
			cout << rCount << endl;

			ifail = BOM_create_window(&tWindow);
			checkifail();
			checkNullTag(tWindow);
			ifail = BOM_set_window_top_line(tWindow, tItem, NULLTAG, NULLTAG, &tTopLine);
			checkifail();
			checkNullTag(tTopLine);
			ifail = BOM_line_ask_all_child_lines(tTopLine, &child, &childs);
			checkifail();

			for (int i = 0; i < child; i++)
			{
				AOM_ask_value_string(childs[i], "bl_item_item_id", &id);
				if (strcmp(id,"000376") == 0)
				{
					childss = childs[i];
				
				}
			}			
			AOM_ask_value_tag(childss, "bl_occurrence", &tocc);
			checkNullTag(tocc);
			ifail = PS_occ_eff_create(tbvr, tocc, &teff);
			checkifail();
			checkNullTag(teff);
			AOM_save_without_extensions(teff);
			AOM_save_without_extensions(tocc);
			AOM_save_without_extensions(tbvr);
			cout << "Created eff\n\n";

			ifail = PS_occ_eff_set_enditemrev(tbvr, tocc, teff, trevP);
			//ifail = CFM_set_eff_mapping_enditem(teff, tItem);
			checkifail();
			cout << "End Item has been set\n\n";

			ifail = PS_occ_eff_set_date_range(tbvr, tocc, teff, "01-Aug-2023 00:00 to 31-Aug-2023 00:00", false);
			//ifail = CFM_effectivity_set_date_ranges(teff, 1, fromD, toD);
			checkifail();
			cout << "date ranges has been set\n\n";

			AOM_save_without_extensions(tocc);
			AOM_save_without_extensions(tbvr);
			AOM_save_without_extensions(teff);
			AOM_save_without_extensions(trevP);
			AOM_save_without_extensions(trev);
			cout << "Saved occ eff\n\n";
			BOM_save_window(tWindow);
			for (int i = 0; i < child; i++)
			{
				AOM_ask_value_string(childs[i], "bl_has_date_effectivity", &id);
				cout << id << endl;
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