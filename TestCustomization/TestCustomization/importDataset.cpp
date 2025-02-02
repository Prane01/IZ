#include <iostream>
#include<stdio.h>
#include<tccore/aom.h>
#include<tcinit/tcinit.h>
#include<tc/tc_startup.h>
#include<ae/dataset.h>
#include<ae/datasettype.h>
#include <tc/emh.h>

#define user "izn"
#define pwd "izn"
#define grp "dba"
#define SS_TEXT 010000
#define SS_BINARY 020000

using namespace std;

int ITK_user_main(int argc, char** argv)
{
	int iFail = 0;
	char* cError = NULL;
	tag_t tdataset = NULLTAG;
	iFail = ITK_init_module(user, pwd, grp);
	if (iFail == ITK_ok)
	{
		printf("TC Login Successful!");
		AE_find_dataset2("000274/A", &tdataset);
		if (tdataset != NULLTAG)
		{
			printf("\n\ndataset found!\n\n");
			AOM_refresh(tdataset, true);
			iFail = AE_import_named_ref(tdataset, "PDF_Reference", "D:\\my_teamcenter.pdf", "qwertyu.pdf", SS_BINARY);
			//cout << iFail << '\n';
			iFail = AOM_save_without_extensions(tdataset);
			//cout << iFail << '\n';
			iFail = AOM_refresh(tdataset, false);
			//cout << iFail << '\n';
			
			printf("\n\n NamedReference Added Succesfully!\n\n");
		}
		else
		{
			EMH_ask_error_text(iFail, &cError);
			cout << "Error: "<< cError;
		}
	}
	else
	{
		EMH_ask_error_text(iFail, &cError);
		cout << "Error: " << cError;
	}
	return iFail;
}