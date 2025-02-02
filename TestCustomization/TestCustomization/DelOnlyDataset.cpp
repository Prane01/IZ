#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/workspaceobject.h>
#include <tccore/grm.h>
#include <tccore/aom.h>
#include <ae/dataset.h>
#include<ae/ae.h>
#include<fclasses/tc_string.h>
#include <Cls0classification/cls_itk.h>
#include<pom/pom/pom.h>
using namespace std;

int iFail = 0;
char* cError = NULL;

int checkITK_OK() {
	if (iFail != ITK_ok)
	{
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : " << cError;
		exit(0);
	}
	else {
		cout << "\n\n Execution success\n";
		return iFail;
	}
}

int ITK_user_main(int arg, char* args[]) {

	tag_t trev = NULLTAG; // revision tag
	tag_t* tRelationTypeList = NULLTAG; // relation type tag
	tag_t  tRelation = NULLTAG; // relation tag
	int count = 0;
	int hits = 0;
	char* s1 = NULL;
	char* s2 = NULL;
	tag_t* list = NULLTAG;
	char* test = NULL;
	tag_t dataset = NULLTAG;
	tag_t* secondary_list = NULLTAG;
	tag_t parent = NULLTAG;
	char* sss = NULL;
	tag_t dType = NULLTAG;
	tag_t class_id = NULLTAG;
	char* class_name = NULL;
	int count1 = 0;

	iFail = ITK_init_module("izn", "izn", "dba");
	checkITK_OK();
		iFail = ITEM_find_rev("000278", "A", &trev); // finding item revision 
		if (trev != NULLTAG)
		{
			cout << "\n\n Item Revision Found..\n";
			iFail = GRM_list_secondary_objects_only(trev, NULLTAG, &count, &secondary_list); // getting all secondaries from trev
			cout << count << '\n';
			if (secondary_list != NULLTAG) 
			{
				for (int i = 0; i < count; i++) // Iterating through list of secondaries
				{
					POM_class_of_instance(secondary_list[i], &class_id); // getting class for the particular object
					POM_name_of_class(class_id, &class_name);  // getting name of the class of particular class
					if (tc_strcmp(class_name,"Dataset") == 0 ) // if the class is Dataset making into if block
					{
						iFail = GRM_list_relation_types(&count1, &tRelationTypeList); // getting list of all relation types
						checkITK_OK(); // checking API return ITK_ok
						if (tRelationTypeList != NULLTAG) {
							for (int j = 0; j < count1; j++) { // Iterating through list of relations types
								GRM_find_relation(trev, secondary_list[i], tRelationTypeList[j], &tRelation); // getting relation to tRelation as to delete that relation
								GRM_delete_relation(tRelation); // deleting the relation of tRelation
								AOM_save_with_extensions(trev); // saving the changes to trev
							}
						}
						AOM_ask_value_string(secondary_list[i], "object_name", &test); // deleted object name
						cout << "Deleted : " << test << '\n';
					}
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
	return iFail;
}