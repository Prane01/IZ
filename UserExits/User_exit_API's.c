#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <property/prop_errors.h>
#include <qry/qry.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <user_exits/user_exits.h>
#include<fclasses/tc_string.h>
#include<tccore/item.h>

	extern DLLAPI int DLLProject_register_callbacks();
	extern DLLAPI int user_exit_query_results(int *decision, va_list args);
	//static void get_object_list(char *id, int *n_objects, tag_t** objects);

	extern DLLAPI int DLLProject_register_callbacks()
	{
		printf("\n\n DLLProject is Registered in Callback \n\n");

		/* tc_set_query_where_run -query=<query name> -run=query */
		ITKCALL(CUSTOM_register_exit("DLLProject", "USER_execute_saved_query", user_exit_query_results));
		return ITK_ok;
	}

	extern DLLAPI int user_exit_query_results(int *decision, va_list args)
	{
		printf("\n\n User-exit query results are \n\n");
		*decision = ALL_CUSTOMIZATIONS;

		va_list largs;
		va_copy(largs, args);

		char  *qName = va_arg(largs, char*);
		int   num_args = va_arg(largs, int);
		char  **names = va_arg(largs, char **);
		char  **values = va_arg(largs, char **);
		int   *num_found = va_arg(largs, int*);
		tag_t **found = va_arg(largs, tag_t **);

		va_end(largs);
		printf("\n qName : %s", qName);
		printf("\n num_args : %d", num_args);
		printf("\n names[0] : %s", names[0]);
		printf("\n values[0] : %s", values[0]);
		if (tc_strcmp(qName, "000001.TC_Query") == 0)
		{
			int n_objects = 0;
			char* cText = NULL;
			tag_t* tobjects = NULL;
			tag_t tcfm_rule = NULLTAG;
			//get_object_list(values[0], &n_objects, &tobjects);

			ITKCALL(ITEM_find(values[0], &n_objects, &tobjects));//Either use the query



			*num_found = n_objects;
			*found = (tag_t *)MEM_alloc(n_objects * sizeof(tag_t));
			ITKCALL(CFM_find("Latest Working", &tcfm_rule));
			for (int i = 0; i < n_objects; i++)
			{
				(*found)[i] = tobjects[i];
				char *CRevRule = NULL;
				ITKCALL(CFM_item_ask_configured(tcfm_rule, (*found)[i], (*found) + i, &CRevRule));
				printf("\n configured : %s", CRevRule);
				MEM_free(CRevRule);
				AOM_ask_value_string((*found)[i], "object_string", &cText);
				printf("\n Object String : %s", cText);
			}
			MEM_free(tobjects);
			return 0;
		}
	}

	//static void get_object_list(char *user_input, int *n_objects, tag_t** objects)
	//{
	//	/* add logic to find the desired objects */

	//	tag_t tquery = NULLTAG;
	//	ITKCALL(QRY_find("TC_Query_local", &tquery));
	//	char *entries[] = { "ID" };
	//	char **values = NULL;

	//	values = (char **)MEM_alloc(sizeof(char *) * 1);
	//	values[0] = (char *)MEM_alloc(strlen(user_input) + 1);
	//	strcpy(values[0], user_input);

	//	ITKCALL(QRY_execute(tquery, 1, entries, values, n_objects, objects));
	//	MEM_free(values[0]);
	//	MEM_free(values);
	//}

#ifdef __cplusplus
}
#endif
