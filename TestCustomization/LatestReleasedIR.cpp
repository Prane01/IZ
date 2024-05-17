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
//#define uname "izn"
//#define pass "izn"
//#define grp "dba"
using namespace std;

void display();
int checkifail();
int checkNullTag(tag_t tag);
int ifail = 0;
char* cError = NULL;

int ITK_user_main(int argc, char* argv[])
{
	tag_t tItem = NULLTAG;
	tag_t opRev = NULLTAG;
	int count = 0;
	tag_t* revList = NULLTAG;
	date_t date;
	date_t after;
	date_t op;
	char* opIR = NULL;
	int cc = 0;
	char** dateS = NULL;
	char* relDate = NULL;
	bool flag = false;

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

			ifail = ITEM_find_item("000392", &tItem);
			ifail = ITEM_list_all_revs(tItem, &count, &revList);
			int j = 1;
			for (int i = 0; i < count; i++){
				if (j < count)
{
					AOM_ask_value_date(revList[i], "date_released", &date);
					AOM_ask_displayable_values(revList[i], "date_released", &cc, &dateS);
					relDate = dateS[0];
					cout << relDate << endl;
					if (tc_strcmp(relDate,"") != 0){
						flag = true;
						AOM_ask_value_date(revList[j], "date_released", &after);
						if (date.year >= after.year && date.month >= after.month && date.day >= after.day && date.hour >= after.hour && date.minute >= after.minute && date.second > after.second)
						{
							op = date;
							opRev = revList[i];
						}
						else{
							op = after;
							opRev = revList[j];
						}
						j++;
					}
				}
			}
			if (flag){
				AOM_ask_value_string(opRev, "object_string", &opIR);
				cout << opIR << endl;
			}
			else{
				cout << "No objects were released...\n\n";
			}
		}
		else{
			EMH_ask_error_text(ifail, &cError);
			cout << "\n\n The error is : " << cError;
		}
	}
	else{
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

int checkNullTag(tag_t tag)
{
	if (tag == NULLTAG)
	{
		EMH_ask_error_text(ifail, &cError);
		cout << "The Error is: " << cError;
		exit(0);
	}
	else
	{
		return 0;
	}
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