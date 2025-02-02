#include <iostream>
#include<fstream>
#include<tccore/tctype.h>
#include<tccore/aom_prop.h>
#include<tccore/aom.h>
#include<tc/tc_startup.h>
#include<tcinit\tcinit.h>
#include<tccore/item.h>
#include<tc\emh.h>
#include<string.h>
#include <property/prop.h>
#define EM_GETLINE              0x00C4

using namespace std;


void display();
//void create(char* propNames, char* value);

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	char* cError = NULL;
	tag_t tItem = NULLTAG;
	int Pcount = 0;
	int d = 0;
	char** propNames;

	fstream fout;
	char* value = NULL;
	char* uname = ITK_ask_cli_argument("-u=");
	char* pass = ITK_ask_cli_argument("-p=");
	char* grp = ITK_ask_cli_argument("-g=");

	if (ITK_ask_cli_argument("-h"))
	{
		display();
		return ifail;
	}
	if (uname != NULL && pass != NULL && grp != NULL)
	{
		if (ITK_init_module(uname,pass,grp) == ITK_ok)
		{
			cout << "\n\n Login Successful...\n";
			ITEM_find_item("152441", &tItem);
			if (tItem != NULLTAG)
			{
				fout.open("D:\sample file.csv", ios::out);
				AOM_ask_prop_names(tItem, &Pcount, &propNames);
				cout << "Generating csv of properties..\n";
				for (int i = 0; i < Pcount; i++) {
					//cout << Pcount << endl;
					cout << propNames[i] << "  ----->  ";
					PROP_ask_composite_property(tItem, propNames[i], &value);
					cout << value << endl;
					//create(propNames[i], value);
					fout << propNames[i] << "," << value << endl;
				}
			}
			else
			{
				EMH_ask_error_text(ifail, &cError);
				cout << "\n\n The error is : " << cError;
			}
		}
		else
		{
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

//void create(char* propNames, char* value) {
//	fstream fout;
//	fout.open("properties.csv", ios::out, ios::app);
//	cout << "Generating csv of properties..\n";
//	fout << propNames << " = " << value << ",";
//}

void display() {

}
