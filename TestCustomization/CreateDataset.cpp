#include <iostream>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <ae/dataset.h>
#include <ae/datasettype.h>
#include <tccore/aom.h>
#include <tccore/grm.h>
using namespace std;


int ITK_user_main(int arg, char* args[]) {

	int iFail = 0;
	char* cError = NULL;
	tag_t trev = NULLTAG;
	tag_t tdata = NULLTAG;
	tag_t tdataset = NULLTAG;
	tag_t tRelation_type = NULLTAG;
	tag_t tRelation = NULLTAG;
	iFail = ITK_init_module("infodba", "infodba", "dba");
	if (iFail == ITK_ok) {
		cout << "\n\n Login Successful..\n";
		iFail = ITEM_find_rev("152415","A", &trev);
		if (iFail == ITK_ok) {
			cout << "\n\n Found Revision to attach..\n";
			iFail = AE_find_datasettype2("PDF",&tdataset);
			if (iFail == ITK_ok) {
				cout << "\n\n Found dataset to attach..\n";
				iFail = AE_create_dataset_with_id(tdataset,"ITKPDF","Nothing","123123","A",&tdata);
				if (iFail == ITK_ok) {
					cout << "\n\n dataset created..\n";
					iFail = AOM_save(tdata);
					if (iFail == ITK_ok) {
						cout << "\n\n dataset saved in DB..\n";
						iFail = GRM_find_relation_type("IMAN_specification", &tRelation_type);
						if (iFail == ITK_ok) {
							cout << "\n\n Took Relation type..\n";
							iFail = GRM_create_relation(trev, tdata, tRelation_type, NULLTAG, &tRelation);
							if (iFail == ITK_ok) {
								cout << "\n\n Created Relation..\n";
								iFail = GRM_save_relation(tRelation);
								if (iFail == ITK_ok) {
									cout << "\n\n GRM Relation Saved..\n";
								}
								else {				
									cout << "\n\n GRM Relation Not Saved";
								}
							}
							else {
								cout << "\n\n Not Created Relation";
							}
						}
						else {
							cout << "\n\n Not Took Relation";
						}
					}
					else {
						cout << "\n\n dataset not saved in DB";
					}
				}
				else {
					cout << "\n\n dataset not created";
				}
			}
			else {
				cout << "\n\n Can't Find dataset to attach";
			}
		}
		else {
			cout << "\n\n Can't Found Revision to attach";
		}
	}
	else {
		EMH_ask_error_text(iFail, &cError);
		cout << "\n\n The error is : %s\n", cError;
	}
	return iFail;
}