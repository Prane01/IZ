#include <tc/tc_startup.h>
#include<tcinit/tcinit.h>
#include<stdio.h>
#include<stdlib.h>
#include<tccore/aom_prop.h>
#include<tccore/item.h>
#include<tc/emh.h>
#include<bom/bom.h>
#include<PS/PS.h>
#include<unidefs.h>
#include<tccore/aom.h>

#define user "izn"
#define pw "izn"
#define grp "dba"

int call(int arg);
int ITK_user_main(int argc, char* argv[])
{
	int iFail = ITK_ok;
	char *cerror = NULL;

	tag_t titem = NULLTAG;
	tag_t titem2 = NULLTAG;
	tag_t titem3 = NULLTAG;
	tag_t trev = NULLTAG;
	tag_t trev1 = NULLTAG;
	tag_t trev2 = NULLTAG;
	tag_t tbom = NULLTAG;
	tag_t ttop_bom = NULLTAG;
	tag_t tfirst_bom = NULLTAG;
	tag_t tsec_bom = NULLTAG;
	tag_t tbom_view = NULLTAG;
	tag_t tbvr = NULLTAG;
	tag_t tbv = NULLTAG;
	call(iFail);
	//logical precise = true;
	call(ITK_init_module(user, pw, grp));
	printf("\nlogin successfully\n");
	iFail = BOM_create_window(&tbom);
	if (iFail == ITK_ok) {
		printf("\n bom window created successfully\n");


		iFail = ITEM_find_rev("000330", "A", &trev);
		iFail = ITEM_find_item("000330", &titem);
		iFail = ITEM_find_rev("000331", "A", &trev1);
		iFail = ITEM_find_item("000331", &titem2);
		iFail = ITEM_find_rev("000332", "A", &trev2);
		iFail = ITEM_find_item("000332", &titem3);

		call(PS_create_bv_with_bvr("view", "view-desc", NULLTAG, "view-rev", "view_rev_desc", false, trev, &tbom_view, &tbvr));
		AOM_save_without_extensions(trev);
		printf("\n bom view and bvr created successfully\n");
		call(BOM_set_window_top_line(tbom, NULLTAG, trev, tbvr, &ttop_bom));
		AOM_save_without_extensions(ttop_bom);
		printf("\ntop line set successfully\n");
		call(BOM_line_add(ttop_bom, titem2, trev1, NULLTAG, &tfirst_bom));
		AOM_save_without_extensions(tfirst_bom);

		printf("\n first bom line set successfully\n");
		call(BOM_line_add(ttop_bom, titem3, trev2, NULLTAG, &tsec_bom));
		AOM_save_without_extensions(tsec_bom);
		call(BOM_save_window(tbom));
		printf("\n second bom line set successfully\n");
	}

	return iFail;
}

int call(int arg)
{
	int iFail = ITK_ok;
	char* Error = NULL;
	iFail = arg;
	if (iFail != ITK_ok)
	{
		EMH_ask_error_text(iFail, &Error);
		printf("Error is %s:\n", Error);
		exit(0);
	}
	return ITK_ok;
}

