#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/workspaceobject.h>
#include <tccore/grm.h>
#include <tccore/aom.h>
using namespace std;

int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL; // error message
	tag_t trev = NULLTAG; // revision tag
	tag_t tRelationType = NULLTAG; // relation type tag
	tag_t  tRelation = NULLTAG; // relation tag
	int count = 0;
	char* test = NULL;
	tag_t * secondary_list = NULL;
	iFail = ITK_init_module("izn", "izn", "dba");
	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		iFail = ITEM_find_rev("000278", "A", &trev); // finding item revision 
		if (trev != NULLTAG)
		{
			cout << "\n\n Item Revision Found..\n";
			iFail = GRM_find_relation_type("IMAN_specification", &tRelationType); // finding relation type of specific
			if (tRelationType != NULLTAG)
			{
				cout << "\n\n Fetching secondary objects..\n";
				iFail = GRM_list_secondary_objects_only(trev, tRelationType, &count, &secondary_list); // getting all secondaries from trev
				cout << count << '\n';
				if (secondary_list != NULLTAG)
				{
					for (int i = 0; i < count; i++)
					{
						GRM_find_relation(trev, secondary_list[i], tRelationType, &tRelation); // getting relation to tRelation as to delete that relation
						GRM_delete_relation(tRelation); // deleting the delation of tRelation
						AOM_save_with_extensions(trev); // saving the changes to trev
						AOM_ask_value_string(secondary_list[i], "object_name", &test); // deleted object name
						cout << "Deleted : " << test << '\n';
					}
				}
				else if (count == 0) { // if searched Item revision has no sencondaries with that relation type this executes
					cout << "\n\n The error is : No secondary objects with that relation";
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
		cout << "\n\n The error is : " << cError;
	}
	return iFail;
}