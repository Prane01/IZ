#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unidefs.h>
#include <base_utils/FndStringUtils.h>
#include <base_utils/Mem.h>
#include <cfm/cfm.h>
#include <epm/epm.h>
#include <fclasses/tc_string.h>
#include <itk/bmf.h>
#include <pom/enq/enq.h>
#include <qry/qry.h>
#include <sa/audit.h>
#include <sa/auditmgr_errors.h>
#include <tc/emh.h>
#include <tc/tc_errors.h>
#include <tc/tc_startup.h>                /* for NULLTAG */
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <tccore/tctype.h>
#include <tccore/workspaceobject.h>
#include <textsrv/textserver.h>
#include <user_exits/user_exit_msg.h>
#include <user_exits/user_exits.h>

#define CHECK_FAIL if (ifail != POM_ok) { TC_write_syslog ("ifail %d in line %d of user_query.c\n", ifail, __LINE__); return 0; }
#ifdef WNT
#define SLASH '\\'
#else
#define SLASH '/'
#endif

#define DASH_STRING    "---"
#define OBJ_TYPE_LABEL "gk_object_type_label"
#define NAME_LABEL     "gk_name_label"

	extern int USER_execute_saved_query(const char *name, int num_args, char **names, char **values,
		int *num_found, tag_t **found)
	{
		int i, ifail;
		tag_t cfm_rule;
		int decision = NO_CUSTOMIZATION;

		BMF_EXECUTE_USER_EXIT_EXTENSIONS(("ImanQuery", "01_TC_set_query_where_run", QRY_custom_execute_msg, name, num_args, names, values, num_found, found));

		ifail = CUSTOM_execute_callbacks(&decision, "USER_execute_saved_query", name, num_args, names, values, num_found, found);

		if (ifail != ITK_ok || decision != NO_CUSTOMIZATION)
		{
			printf ( "if (ifail != ITK_ok || decision != NO_CUSTOMIZATION)\n");
			return ifail;
		}

		/* first find a bunch of Items */
		if (values != NULL)
		{
			printf("if (values != NULL)\n");
			ifail = ITEM_find(values[0], num_found, found);
			if (ifail != ITK_ok)
			{
				return ifail;
			}
		}

		ifail = CFM_find("Latest Working", &cfm_rule);
		printf("CFM_find(Latest Working, &cfm_rule)\n");
		if (ifail != ITK_ok)
		{
			return ifail;
		}
		/* and then configure them to get their equivalent revisions */
		for (i = 0; i < *num_found; i++)
		{
			printf("num_found : ",num_found);
			char *how;
			ifail = CFM_item_ask_configured(cfm_rule, (*found)[i], (*found) + i, &how);
			if (ifail != ITK_ok)
			{
				return ifail;
			}
			MEM_free(how);
		}
		return ITK_ok;
}