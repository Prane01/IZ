#include<iostream>
#include<tc/tc_startup.h>
#include<tcinit/tcinit.h>
#include<tc/emh.h>
#include<tccore/item.h>
#include<tc/folder.h>
#include<sa/user.h>
#include<tccore/aom.h>
#include<ae/dataset.h>
#include<tccore/aom_prop.h>
#include<tccore/traversalrule.h>
#include<res/res_itk.h>
#include<sa/tcfile.h>
#include<ae/ae_errors.h>
#include<ae/datasettype.h>
#include<ss/ss_const.h>
#include<property/prop.h>
#include<tccore/tctype.h>
#include<fstream>
#include<string>
#include<tccore/aom_prop.h>
#include<bom/bom.h>
#include<ps/ps.h>
using namespace std;
int ITK_user_main(int argc, char** argv)
{
	int ifail = ITK_ok;
	char *cerror = NULL;
	tag_t titem = NULLTAG;
	tag_t trev = NULLTAG;
	tag_t trev1 = NULLTAG;
	tag_t trev2 = NULLTAG;
	ifstream fi;
	string line;
	const char *itemId;
	int count = 0;
	tag_t twindow = NULLTAG;
	tag_t tTop_bom = NULLTAG;
	tag_t tbv = NULLTAG;
	tag_t tbvr = NULLTAG;
	tag_t tnewLine = NULLTAG;
	tag_t newLine = NULLTAG;
	ifail = ITK_init_module("infodba", "infodba", "dba");
	if (ifail == ITK_ok)
	{
		cout << "Login successfull " << endl;
		ifail = ITEM_find_rev("000322", "A", &trev);
		if (ifail == ITK_ok && trev != NULLTAG)
		{
			cout << "Parent found " << endl;
			ifail = ITEM_find_rev("000318", "A", &trev1);
			if (ifail == ITK_ok && trev1 != NULLTAG)
			{
				cout << "child1 found " << endl;
				ifail = ITEM_find_rev("000319", "A", &trev2);
				if (ifail == ITK_ok && trev2 != NULLTAG)
				{
					cout << "child2 found " << endl;
					ifail = PS_create_bv_with_bvr("bv1", NULL, NULL, "bvr1", NULL, false, trev, &tbv, &tbvr);
					if (ifail == ITK_ok && tbv != NULLTAG && tbvr != NULLTAG)
					{
						cout << "Bv and Bvr Created " << endl;
						ifail = BOM_create_window(&twindow);
						if (ifail == ITK_ok && twindow != NULLTAG)
						{
							cout << "Bom window created :" << endl;
							ifail = BOM_set_window_top_line(twindow, NULLTAG, trev, tbv, &tTop_bom);
							if (ifail == ITK_ok && tTop_bom != NULLTAG)
							{
								cout << "Top line is set :" << endl;
								ifail = BOM_line_add(tTop_bom, NULLTAG, trev1, NULLTAG, &newLine);
								if (ifail == ITK_ok && newLine != NULLTAG)
								{
									cout << "Line added as child :" << endl;
									ifail = BOM_line_add(tTop_bom, NULLTAG, trev2, NULLTAG, &newLine);
									if (ifail == ITK_ok && newLine != NULLTAG)
									{
										cout << "Line added as child :" << endl;
										cout << "structure created :" << endl;
										AOM_save_without_extensions(tbv);
										AOM_save_without_extensions(tbvr);
										AOM_save_without_extensions(twindow);

									}
									else
									{
										cout << "Line2 not added as child :" << endl;
										cout << "structure Not created :" << endl;
										EMH_ask_error_text(ifail, &cerror);
										cout << cerror << endl;
									}



								}
								else
								{
									cout << "Line1 not added as child :" << endl;
									EMH_ask_error_text(ifail, &cerror);
									cout << cerror << endl;
								}
							}
							else
							{
								cout << "Top line is Not set :" << endl;
								EMH_ask_error_text(ifail, &cerror);
								cout << cerror << endl;
							}
						}
						else
						{
							cout << "Bom window not created :" << endl;
							EMH_ask_error_text(ifail, &cerror);
							cout << cerror << endl;
						}
					}
				}
			}



		}
	}
	return ifail;
}