#ifdef __cplusplus
extern "C" {
#endif
	/*
	*    Teamcenter Query Builder Definition
	*        Name: Find Object by UID
	*        Search Type: WorkspaceObject
	*        Modifiable Query Types: Local Query
	*
	*        Search Criteria
	*           Attribute: object_name
	*           User Entry L10N Key: Name
	*           User Entry Name: Name
	*           operator =
	*        Create
	*
	*    Execute utility to convert "Local Query" to "User Query"
	*
	*       %TC_BIN%\tc_set_query_where_run -query="SM_get_all_parents" -run=query
	*/

#include <tccore/custom.h>
#include <tccore/method.h>
#include <tccore/aom_prop.h>
#include <fclasses/tc_string.h>
#include <user_exits/user_exits.h>
#include <stdlib.h>
#include <string.h>
#include <tc/emh.h>

#define DLLAPI _declspec(dllexport)
#define PLM_error (EMH_USER_error_base +3)

	int status = 0;

	/* Function declarations */
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int userExitQryResults(int *decision, va_list args);
	void findParents(tag_t* tPrnts, int iPrntCnt, tag_t **allTags, int *allCount);

	/* Utility to check if tag already exists */
	int tag_exists_in_array(tag_t *tags, int count, tag_t t) {
		for (int i = 0; i < count; i++) {
			if (tags[i] == t) {
				return 1; // exists
			}
		}
		return 0; // not found
	}

	/* Function definitions */
	extern DLLAPI int DLLProject01_register_callbacks() {
		ITKCALL(CUSTOM_register_exit("DLLProject01", "USER_execute_saved_query", userExitQryResults));
		return status;
	}

	extern DLLAPI int userExitQryResults(int* decision, va_list args) {
		*decision = ALL_CUSTOMIZATIONS;
		TC_write_syslog("***** Entered into userExitQryResults *****\n");

		va_list largs;
		va_copy(largs, args);

		char  *qName = va_arg(largs, char*);
		int   num_args = va_arg(largs, int);
		char  **names = va_arg(largs, char **);
		char  **values = va_arg(largs, char **);
		int   *num_found = va_arg(largs, int*);
		tag_t **found = va_arg(largs, tag_t **);

		va_end(largs);

		TC_write_syslog("Query Name: %s\n", qName);
		if (tc_strcmp(qName, "SM_get_all_parents") == 0)
		{
			TC_write_syslog("Query Args: %d\n", num_args);
			for (int i = 0; i < num_args; i++) {
				TC_write_syslog("Name: %s\n", names[i]);
				TC_write_syslog("Value: %s\n", values[i]);
			}

			tag_t tQuery = NULLTAG;
			int n_entries = 1;
			char **qry_values = NULL;
			char **entries = NULL;
			int iFound = 0;
			tag_t* tObjectsFound = NULL;

			/* store all tags dynamically */
			tag_t *allTags = NULL;
			int allCount = 0;

			ITKCALL(QRY_find2("Item Revision...", &tQuery));

			entries = (char **)MEM_alloc(n_entries * sizeof(char*));
			entries[0] = (char *)MEM_alloc(strlen("Item ID") + 1);
			tc_strcpy(entries[0], "Item ID");

			qry_values = (char **)MEM_alloc(n_entries * sizeof(char*));
			qry_values[0] = (char *)MEM_alloc(strlen(values[0]) + 1);
			tc_strcpy(qry_values[0], values[0]);

			ITKCALL(QRY_execute(tQuery, n_entries, entries, qry_values, &iFound, &tObjectsFound));
			TC_write_syslog("Actual Search Objects: %d\n", iFound);

			for (int i = 0; i < iFound; i++) {
				int iPrntCnt = 0;
				tag_t* tPrnts = NULL;
				char *objStr = NULL;

				ITKCALL(AOM_ask_value_string(tObjectsFound[i], "object_string", &objStr));
				TC_write_syslog("Search Object : %d\n", i + 1);
				TC_write_syslog("Object String: %s\n", objStr);
				MEM_free(objStr);

				ITKCALL(AOM_ask_value_tags(tObjectsFound[i], "ps_parents", &iPrntCnt, &tPrnts));

				/* Add this object if not already added */
				if (!tag_exists_in_array(allTags, allCount, tObjectsFound[i])) {
					allTags = (tag_t *)MEM_realloc(allTags, (allCount + 1) * sizeof(tag_t));
					allTags[allCount++] = tObjectsFound[i];
				}

				if (iPrntCnt > 0) {
					TC_write_syslog("Getting Parents For : %d\n", i + 1);
					findParents(tPrnts, iPrntCnt, &allTags, &allCount);
				}
				else {
					TC_write_syslog("No Parents Exists for %d\n", i + 1);
				}
			}

			/* return results if caller expects */
			*num_found = allCount;
			*found = allTags;
		}

		TC_write_syslog("***** Exit from userExitQryResults *****\n");
		return status;
	}

	void findParents(tag_t* tPrnts, int iPrntCnt, tag_t **allTags, int *allCount) {
		int fndParentsCount = 0;
		tag_t* fndParents = NULL;

		if (iPrntCnt == 0) {
			return;
		}

		for (int i = 0; i < iPrntCnt; i++) {
			char *objStr = NULL;
			ITKCALL(AOM_ask_value_string(tPrnts[i], "object_string", &objStr));
			TC_write_syslog("Object String: %s\n", objStr);
			MEM_free(objStr);

			/* Add only if not already in list */
			if (!tag_exists_in_array(*allTags, *allCount, tPrnts[i])) {
				*allTags = (tag_t *)MEM_realloc(*allTags, (*allCount + 1) * sizeof(tag_t));
				(*allTags)[(*allCount)++] = tPrnts[i];
			}
			else {
				TC_write_syslog("Skipping duplicate: already added\n");
			}

			ITKCALL(AOM_ask_value_tags(tPrnts[i], "ps_parents", &fndParentsCount, &fndParents));

			if (fndParentsCount > 0) {
				TC_write_syslog("Getting Parents For : %d\n", i + 1);
				findParents(fndParents, fndParentsCount, allTags, allCount);
			}
			else {
				TC_write_syslog("No Parents Exists for %d\n", i + 1);
			}
		}
	}

#ifdef __cplusplus
}
#endif
