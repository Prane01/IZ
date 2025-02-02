#include<iostream>
#include<stdio.h>
#include<fstream>
#include<tccore/tctype.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<tc/tc_startup.h>
#include<tcinit\tcinit.h>
#include<tccore/item.h>
#include<tc\emh.h>
#include<stdlib.h>
#include<string> 
#include<vector>
#include <tccore/grm.h>
#include<sys/types.h>
using namespace std;

void display();
void readFile();
int checkifail();
int ifail = 0;
char* cError = NULL;
fstream inputFile;
fstream outputFile;
int lineCount = 0;
int ITK_user_main(int argc, char* argv[])
{
	char *uname = ITK_ask_cli_argument("-u="); // API takes user input as username

	char *pass = ITK_ask_cli_argument("-p="); // API takes user input as password

	char *grp = ITK_ask_cli_argument("-g="); // API takes user input as group

	if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function

		display();
		return ifail;
	}

	if ((tc_strcmp(uname, "") != 0) && (tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {  // Validates all values are filled
		if (ifail = ITK_init_module(uname, pass, grp) == ITK_ok) {
			cout << "\nLogin successful...\n\n";
			struct dirent *d;
			
			readFile();
		}
		else
		{
			tag_t aa = NULLTAG;
			EMH_ask_error_text(ifail, &cError);
			cout << "\n\n The error is : " << cError;
		}
	}
	else
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "\n\n The error is : " << cError;
	}
	return ifail;
}
void display() {
	cout << "\n -u\t ---> Enter username";
	cout << "\n -p\t ---> Enter password";
	cout << "\n -g\t ---> Enter group";
}
void readFile() {
	inputFile.open("D:\\SOFT\\Teamcenter\\TC_DATA\\model");
	string line;
	int j = 0;
	int eCounter = 0;
	string str;
	while (getline(inputFile, line)) {
		j++;
		//line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
		if (j == 1)
		{
			lineCount++;
			outputFile.open("C:\\Users\\T50372\\Downloads\\testTask02.txt", ios::app);
			for (int i = 0; i < line.size(); i++)
			{
				//if (line[i] != ',' && line[i] < 'A' || line[i] > 'Z' && line[i] < 'a' || line[i] > 'z')
				if (line[i] == '"')
				{
					line.erase(i, 1);
					i--;
				}
			}
			for (int i = 0; i < line.size(); i++)
			{
				outputFile.put(line[i]);
			}
			outputFile.put('\n');
			outputFile.close();
		}
		else {
			lineCount++;
			outputFile.open("C:\\Users\\T50372\\Downloads\\testTask02.txt", ios::app);
			for (int i = 0; i < line.size(); i++)
			{
				if (line[i] == ',')
				{
					eCounter++;
					modifyString(str, eCounter);
					outputFile.put(',');
					str = "";
				}
				else if (i == line.size() - 1)
				{
					eCounter++;
					//cout << str << endl;
					modifyString(str, eCounter);
					str = "";
				}
				else {
					str += line[i];
				}
			}
			outputFile.put('\n');
			outputFile.close();
		}
		eCounter = 0;
	}
	inputFile.close();
}
int checkifail()
{
	if (ifail != ITK_ok)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "Error: " << cError;
		exit(0);
	}
	else
	{
		return 0;
	}
}