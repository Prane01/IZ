#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/workspaceobject.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t trev = NULLTAG;
	char* value = NULL;
	tag_t tRelationType = NULLTAG;
	int count = 0;
	char* value1 = NULL;
	char* test = NULL;
	tag_t * secondary_list = NULL;
	tag_t* stored = NULLTAG;
	date_t tdate = NULLDATE;
	char* dtostr = NULL;
	iFail = ITK_init_module("infodba", "infodba", "dba");
	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		/*iFail = ITEM_find_item("000048", &trev);
		iFail = AOM_ask_value_date(trev, "creation_date", &tdate);
		ITK_date_to_string(tdate, &dtostr);
		cout << dtostr << endl;
		int num = 0;
		tag_t* list = NULL;
		iFail = AOM_ask_value_tags(trev, "revision_list", &num, &list);
		for (int i = 0; i < num; i++) {
			AOM_ask_value_string(list[i], "item_revision_id", &test);
			cout << test << endl;
		}*/
		iFail = ITEM_find_rev("000048","A", &trev);
		if (trev != NULLTAG)
		{
			cout << "\n\n Item Revision Found..\n";
			iFail = GRM_find_relation_type("IMAN_specification", &tRelationType);
			if (tRelationType != NULLTAG)
			{
				cout << "\n\n Fetching secondary objects..\n";
				iFail = GRM_list_secondary_objects_only(trev, tRelationType, &count, &secondary_list);
				if (secondary_list != NULL)
				{
					for (int i = 0; i < count; i++)
					{
						
						//cout << count;
						AOM_ask_value_string(secondary_list[i],"object_name", &test);
						cout <<  test << '\n';
					}
				}
				else {
					cout << "\n\n The error is : Cannot list secondaries";
				}
			}
			else {
				cout << "\n\n The error is : Unable to fetch secondaries";
			}	  
		}		  
		else {	  
			cout << "\n\n The error is : Cannot find Ite Revision";
		}	  
	}			  
	else {		  
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : %s\n", cError;
	}
	return iFail;

}