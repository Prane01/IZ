/*
*    Teamcenter Ouery Builder Definition
*        Name: Find Object by UID
*        Search Type: WorkspaceObject
*        Modifiable Query Types: Local Query
*
*        Search Critrea
*           Attribute: object_name
*           User Entry L10N Key: Name
*           User Entry Name: Name
*           operator =
*        Create
*
*    Execute utility to convert "Local Query" to "User Query"
*
*       %TC_BIN%\tc_set_query_where_run -query="Find Object by Name" -run=query
*/

#include<iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sa/am.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <pom/pom/pom.h>
#include <property/prop_errors.h>
#include <qry/qry.h>
#include <tc/emh.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/workspaceobject.h>
#include <user_exits/user_exits.h>
#include <ug_va_copy.h>
using namespace std;
#define DLLAPI _declspec(dllexport)

extern "C" {
extern DLLAPI int DLLProject_register_callbacks();
extern DLLAPI int user_exit_query_results_displayed_in_ui(int *decision, va_list args);
static void get_object_list(char *id, int *n_objects, tag_t** objects);

extern DLLAPI int DLLProject_register_callbacks()
	{
		cout<<"\n\n DLLProject_register_callbacks \n\n";
		/* tc_set_query_where_run -query=<query name> -run=query */
		CUSTOM_register_exit("DLLProject", "USER_execute_saved_query", user_exit_query_results_displayed_in_ui);
		return ITK_ok;
	}

	extern DLLAPI int user_exit_query_results_displayed_in_ui(int *decision, va_list args)
	{
		cout<<"\n\n user_exit_query_results_displayed_in_ui \n\n";

		*decision = ALL_CUSTOMIZATIONS;
		int ifail = ITK_ok;

		va_list largs;
		va_copy(largs, args);

		char  *qName = va_arg(largs, char*);
		int   num_args = va_arg(largs, int);
		char  **names = va_arg(largs, char **);
		char  **values = va_arg(largs, char **);
		int   *num_found = va_arg(largs, int*);
		tag_t **found = va_arg(largs, tag_t **);

		va_end(largs);

		if (strcmp(qName, "ExecuteQryToShowObjs") == 0)
		{
			int n_objects = 0;
			tag_t *objects = NULL;
			get_object_list(values[0], &n_objects, &objects);

			*num_found = n_objects;
			*found = (tag_t *)MEM_alloc(n_objects * sizeof(tag_t));
			for (int ii = 0; ii < n_objects; ii++)
			{
				(*found)[ii] = objects[ii];
			}
			MEM_free(objects);
		}
		return ifail;
	}

	static void get_object_list(char *user_input, int *n_objects, tag_t** objects)
	{
		/* add logic to find the desired objects */

		tag_t query = NULLTAG;
		QRY_find2("ExecuteQryToShowObjs", &query);

		char **entries=NULL;
		char **values = NULL;
		int NofEntries = 0;
		values = (char **)MEM_alloc(sizeof(char *) * 1);
		values[0] = (char *)MEM_alloc(strlen(user_input) + 1);
		strcpy(values[0], user_input);
		QRY_find_user_entries(query,&NofEntries,&entries,&values);
		QRY_execute(query, 1, entries, values, n_objects, objects);
		MEM_free(values[0]);
		MEM_free(values);
	}

}