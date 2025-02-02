#include <iostream>
#include <tc/emh.h>
#include <tc/emh_errors.h>
using namespace std;
#define IFAIL_CHECK(X) (report_error( __FILE__, __LINE__, #X, (X)))


static void report_error(char *file, int line, char *function, int iFail)
{
	if (iFail != ITK_ok)
	{
		char *cError;
		EMH_ask_error_text(iFail, &cError);
		cout << "ERROR: " << iFail << " ERROR MSG: " << cError << '\n';
		cout << "FUNCTION: " << function << " FILE: " << file << " LINE: " << line << '\n';
		if (cError) MEM_free(cError);
		cout << "\nExiting program!\n";
		exit(EXIT_FAILURE);
	}
}
