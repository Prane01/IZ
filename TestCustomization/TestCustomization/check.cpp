#include <iostream>
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
#include <check/ITK_okCheck.h>
#define IFAIL_CHECK(X)
using namespace std;


int ITK_user_main(int argc, char* argv[])
{
	int iFail = 0;
	char* cError = NULL;
	IFAIL_CHECK(ITK_init_module("infoba", "infodba", "dba")); // Login to server
	return iFail;
}
