#ifdef __cplusplus
extern "C" {
#endif

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
#include <sa/person.h>
#include <sa/user.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <ug_va_copy.h>


	extern DLLAPI int DLLProject_register_callbacks();
	static int get_users_task_to_perform_folder(char *user_name, tag_t *task_to_perform_folder);
	extern DLLAPI int user_exit_query_results_to_text_table(int *decision, va_list args);
	extern void make_query_row(int n_columns, QRY_user_query_row_t** row);
	static int get_task_information(tag_t task_tag, char task[WSO_name_size_c + 1],char state[WSO_name_size_c + 1], char parent[WSO_name_size_c + 1],char job[WSO_name_size_c + 1],char process[WSO_name_size_c + 1]);

	extern DLLAPI int DLLProject_register_callbacks()
	{
		printf("\n\n DLLProject is Registered in Callback \n\n");
		/*
			Execute this coomand after query is created
			%TC_BIN%\tc_set_query_where_run -query="User's Tasks to Perform" -run=user
		*/
		ITKCALL(CUSTOM_register_exit("DLLProject", "USER_query_execute", user_exit_query_results_to_text_table));
		return ITK_ok;
	}

	extern DLLAPI int user_exit_query_results_to_text_table(int *decision, va_list args)
	{
		printf("\n\n User-exit query results are \n\n");
		*decision = ALL_CUSTOMIZATIONS;
		int ifail = ITK_ok;

		va_list largs;
		va_copy(largs, args);
		char  *qName = va_arg(largs, char*);
		int   num_args = va_arg(largs, int);
		char  **fields = va_arg(largs, char **);
		char  **values = va_arg(largs, char **);
		int   *n_columns = va_arg(largs, int*);
		char  ***col_names = va_arg(largs, char ***);
		QRY_user_query_row_t **rows = va_arg(largs, QRY_user_query_row_t**);
		va_end(largs);
		printf("\n qName : %s", qName);
		printf("\n num_args : %d", num_args);
		printf("\n fields[0] : %s", fields[0]);
		printf("\n values[0] : %s", values[0]);
		printf("\n n_columns : %d", *n_columns);

		*n_columns = 0;
		*rows = NULL;

		if (tc_strcmp(qName, "000001.Find_User_Tasks") == 0)
		{
			char user_name[SA_name_size_c + 1] = "";
			strcpy(user_name, values[0]);

			/* Query only has one field/value pair, "User" */
			tag_t task_to_perform_folder = NULLTAG;
			get_users_task_to_perform_folder(user_name, &task_to_perform_folder);

			if (task_to_perform_folder == NULLTAG)
			{
				ifail = 919002; // custom error in ue_errors.xml
				EMH_store_error_s1(EMH_severity_information, ifail, user_name);
				*rows = NULL;
				return ifail;
			}

			int n_tasks = 0;
			tag_t *tasks = NULL;
			ifail = AOM_ask_value_tags(task_to_perform_folder, "contents", &n_tasks, &tasks);
			if ((ifail != ITK_ok) || (n_tasks < 1))
			{
				ifail = 919002; // custom error in ue_errors.xml
				EMH_store_error_s1(EMH_severity_information, ifail, user_name);
				*rows = NULL;
				return ifail;
			}

			/* set column number and column names */
			char names[][32] = { "User", "Task Name", "Task State", "Parent Task", "Job", "Process" };
			*n_columns = 6;
			*col_names = (char **)MEM_alloc(*n_columns * sizeof(char *));
			for (int i = 0; i < *n_columns; i++)
			{
				(*col_names)[i] = (char*)MEM_alloc(strlen(names[i]) + 1);
				strcpy((*col_names)[i], names[i]);
			}

			char task[WSO_name_size_c + 1] = "";
			char state[WSO_name_size_c + 1] = "";
			char parent[WSO_name_size_c + 1] = "";
			char job[WSO_name_size_c + 1] = "";
			char process[WSO_name_size_c + 1] = "";

			int count = 0;
			QRY_user_query_row_t *prev_row = NULL;
			QRY_user_query_row_t *current_row = NULL;
			QRY_user_query_row_t *first_row = NULL;

			for (int j = 0; j < n_tasks; j++)
			{
				make_query_row(*n_columns, &current_row);

				char *task_uid = NULL;
				ITK__convert_tag_to_uid(tasks[j], &task_uid);

				current_row->object_string = (char*)MEM_alloc(strlen(task_uid) + 1);
				strcpy(current_row->object_string, task_uid);
				MEM_free(task_uid);

				current_row->values[0] = (char*)MEM_alloc(strlen(user_name) + 1);
				strcpy(current_row->values[0], user_name);

				get_task_information(tasks[j], task, state, parent, job, process);

				current_row->values[1] = (char*)MEM_alloc(strlen(task) + 1);
				strcpy(current_row->values[1], task);

				current_row->values[2] = (char*)MEM_alloc(strlen(state) + 1);
				strcpy(current_row->values[2], state);

				current_row->values[3] = (char*)MEM_alloc(strlen(parent) + 1);
				strcpy(current_row->values[3], parent);

				current_row->values[4] = (char*)MEM_alloc(strlen(job) + 1);
				strcpy(current_row->values[4], job);

				current_row->values[5] = (char*)MEM_alloc(strlen(process) + 1);
				strcpy(current_row->values[5], process);

				if (count == 0)
				{
					first_row = current_row;
					prev_row = current_row;
				}
				else
				{
					prev_row->next = current_row;
					prev_row = current_row;
				}
				count++;
			}
			*rows = first_row;

			/* print text table */
			for (int k = 0; k < *n_columns; k++)
			{
				if (k == 0)
					printf("\n%s", (*col_names)[k]);
				else
					printf("\t %s", (*col_names)[k]);

			}
			printf("\n");

			for (QRY_user_query_row_t *row = (*rows); row != 0; row = row->next)
			{
				printf("\n %s \t %s \t %s \t %s \t %s\n",row->values[0], row->values[1], row->values[2], row->values[3], row->values[4]);
			}
		}
		return ITK_ok;
	}

	static int get_users_task_to_perform_folder(char *user_name, tag_t *task_to_perform_folder)
	{
		int ifail = ITK_ok;
		tag_t type_tag = NULLTAG;
		ifail = TCTYPE_find_type("TasksToPerform", NULL, &type_tag);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		char *display_name = NULL;
		ifail = TCTYPE_ask_display_name(type_tag, &display_name);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		tag_t query = NULLTAG;
		ifail = QRY_find("General...", &query);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		// Find user's "Tasks to Perform" folder
		char *entries[2] = { "Owning User", "Type" };
		char *values[2] = { user_name, display_name };
		int n_folder = 0;
		tag_t *folder = NULLTAG;
		ifail = QRY_execute(query, 2, entries, values, &n_folder, &folder);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		if (n_folder == 0)
		{
			*task_to_perform_folder = NULLTAG;
			return ifail;
		}

		*task_to_perform_folder = folder[0];
		MEM_free(folder);

		return ifail;
	}

	static int get_task_information(tag_t task_tag, char task[WSO_name_size_c + 1],char state[WSO_name_size_c + 1], char parent[WSO_name_size_c + 1],char job[WSO_name_size_c + 1], char process[WSO_name_size_c + 1])
	{
		int ifail = ITK_ok;

		ifail = EPM_ask_name(task_tag, task);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		tag_t  parent_tag = NULLTAG;
		ifail = EPM_ask_parent_task(task_tag, &parent_tag);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		ifail = EPM_ask_name(parent_tag, parent);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		EPM_state_t  epm_state;
		ifail = EPM_ask_state(task_tag, &epm_state);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		ifail = EPM_ask_state_string(epm_state, state);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		tag_t  job_tag = NULLTAG;
		ifail = EPM_ask_job(task_tag, &job_tag);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		ifail = WSOM_ask_name(job_tag, job);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		tag_t  root_tag = NULLTAG;
		ifail = EPM_ask_root_task(task_tag, &root_tag);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		ifail = EPM_ask_name(root_tag, process);
		if (ifail != ITK_ok)
		{
			return ifail;
		}

		return ifail;
	}

	extern void make_query_row(int n_columns, QRY_user_query_row_t** row)
	{
		*row = (QRY_user_query_row_t *)MEM_alloc(sizeof(QRY_user_query_row_t) + n_columns * sizeof(char *));
		(*row)->next = NULL;
		(*row)->object_string = NULL;
		(*row)->might_map_to_tc = false;
		for (int i = 0; i < n_columns; i++)
		{
			(*row)->values[i] = NULL;
		}
	}

#ifdef __cplusplus
}
#endif