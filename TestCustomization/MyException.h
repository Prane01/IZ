#include <exception> 
#include <tc\emh.h>
using namespace std;

char* errorMessage = NULL;
int code = 0;

class MyException : public exception{
public:
	MyException() {};
public: 
	void error()
	{
		EMH_ask_error_text(code, &errorMessage);
		cout << "Error : " << errorMessage << endl;
	}
	void operator = (int ifail)
	{
		if (ifail != ITK_ok)
		{
			code = ifail;
			MyException obj;
			throw obj;
		}
	}
};

