#include<stdio.h>
#include<conio.h>
#include<iostream>
#include<stdio.h>
#include<conio.h>
#include<iostream>
#include<fstream>
#include<tccore/tctype.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<tc/tc_startup.h>
#include<tcinit\tcinit.h>
#include<tccore/item.h>
#include<tc\emh.h>
#include<stdlib.h>
#include<string.h>
#include <tccore/project.h>
#include <sa/user.h>
using namespace std;

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	tag_t tProject = NULLTAG;
	tag_t tAuthor = NULLTAG;
	tag_t tUsers = NULLTAG;

	ifail = ITK_init_module("izn", "izn", "dba"); // Login to server

	if (ifail == ITK_ok)
	{
		printf("\n\n Successfully done\n\n");

		PROJ_find("T001", &tProject); // finding specific project with project_id and storing it in tag_t
		cout << "Found project\n";

		ITK_set_bypass(true); // setting privileges to allow making changes

		/*SA_find_user2("izn", &tAuthor);
		cout << "Found izn\n";*/

		SA_find_user2("infodba", &tUsers); // finding user and storing it's address in tUsers
		cout << "Found infodba\n";

		//SA_find_user2("izn", tUsers);

		if (tProject != NULLTAG && tUsers != NULLTAG)
		{
			/*PROJ_add_author_members(tProject, 1, &tUsers); // adding author members in tUser to project
			cout << "Project author added to T001\n";*/

			PROJ_add_members(tProject, 1, &tUsers); // adding members in tUser to project
			cout << "Project members added to T001\n";

			/*PROJ_add_team_admin(tProject, 1, &tUsers); // adding adminmembers in tUser to project
			cout << "Project admin added to T001\n";*/

			AOM_save_without_extensions(tProject);
			cout << "Saved changes on T001\n";
		}
		else
		{
			cout << "\n\n The error is : unable to add members\n";
		}
	}
	else
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "\n\n The error is : " << cError;
	}
	return ifail;
}