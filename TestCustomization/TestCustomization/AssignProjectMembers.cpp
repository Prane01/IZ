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
#include <sstream>
#include <vector>
#include <string>
using namespace std;

// Function to split a string by a delimiter
std::vector<std::string> split(const std::string &str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string temp;

	while (std::getline(ss, temp, delimiter)) {
		tokens.push_back(temp);
	}
	return tokens;
}

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

		std::ifstream infile("D:\\Siemens\\Teamcenter2406\\Export\\TC_Project\\UpdateProject.txt");  // file with your data
		if (!infile.is_open()) {
			std::cerr << "Error opening file!" << std::endl;
			return 1;
		}
		std::string line;
		while (std::getline(infile, line)) {
			// Split by '|'
			std::vector<std::string> parts = split(line, '|');

			std::cout << "Line split by '|':\n";
			for (size_t i = 0; i < parts.size(); i++) {
				std::cout << "  Part[" << i << "]: " << parts[i] << "\n";

				// If you want to also split by '~'
				if (parts[i].find('~') != std::string::npos) {
					std::vector<std::string> subParts = split(parts[i], '~');
					for (size_t j = 0; j < subParts.size(); j++) {
						std::cout << "    SubPart[" << j << "]: " << subParts[j] << "\n";
					}
				}
			}
			std::cout << "----------------------------------------\n";
		}

		infile.close();

	//	PROJ_find("SAS_Reg_Commercial Access", &tProject); // finding specific project with project_id and storing it in tag_t
	//	cout << "Found project\n";

	//	ITK_set_bypass(true); // setting privileges to allow making changes

	//	/*SA_find_user2("izn", &tAuthor);
	//	cout << "Found izn\n";*/

	//	SA_find_user2("infodba", &tUsers); // finding user and storing it's address in tUsers
	//	cout << "Found infodba\n";

	//	//SA_find_user2("izn", tUsers);

	//	if (tProject != NULLTAG && tUsers != NULLTAG)
	//	{
	//		/*PROJ_add_author_members(tProject, 1, &tUsers); // adding author members in tUser to project
	//		cout << "Project author added to T001\n";*/
	//		
	//		PROJ_add_members(tProject, 1, &tUsers); // adding members in tUser to project
	//		cout << "Project members added to T001\n";

	//		/*PROJ_add_team_admin(tProject, 1, &tUsers); // adding adminmembers in tUser to project
	//		cout << "Project admin added to T001\n";*/

	//		AOM_save_without_extensions(tProject);
	//		cout << "Saved changes on T001\n";
	//	}
	//	else
	//	{
	//		cout << "\n\n The error is : unable to add members\n";
	//	}
	//}
	//else
	//{
	//	EMH_ask_error_text(ifail, &cError);
	//	cout << "\n\n The error is : " << cError;
	//}
	return ifail;
}