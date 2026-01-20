#include<iostream>
#include<tcinit/tcinit.h>
#include<tc/emh.h>
#include<base_utils/TcResultStatus.hxx>
#include<base_utils/ScopedSmPtr.hxx>
#include<base_utils/ScopedPtr.hxx>
#include<base_utils/IFail.hxx>
#include<tc/tc_macros.h>
#include<vector>
#include<string.h>
#include<sstream>
#include<tccore/project.h>
#include<tccore/aom.h>
#include<tccore/item.h>
#include<tccore/aom_prop.h>
#include<time.h>
#include<ctime>
#include<tccore/grm.h>
#include<tccore/tctype.h>
#include<sa/sa.h>
#include<epm/epm.h>
using namespace std;
using namespace Teamcenter;
#ifdef _WIN32
string path = "D:\\Files\\Text_Files\\outputFile.txt ";
#else
string path = "teamcenter/apps/volume";
#endif
extern "C" void AM__set_application_bypass(logical);
#define IZ_free_memory(p) {\
		if ( p != NULL ) {\
			MEM_free(p);\
			p = NULL;\
		}\
}

void display() {
	cout << "\n -u\t ---> username";
	cout << "\n -p\t ---> password";
	cout << "\n -g\t ---> group";
}

//Adding tag to tag array
static void add_tag_to_tag_array(tag_t add_tag, int* n_tag_array, tag_t** tag_array)
{
	int count = *n_tag_array;
	count++;
	if (count == 1)
	{
		(*tag_array) = (tag_t*)MEM_alloc(sizeof(tag_t));
	}
	else
	{
		(*tag_array) = (tag_t*)MEM_realloc((*tag_array), count * sizeof(tag_t));
	}
	(*tag_array)[count - 1] = add_tag;
	*n_tag_array = count;
}

vector<string> splitByDelimiter(const string& inputLine, char delimiter) {
	vector<string> result;
	stringstream ss(inputLine);
	string token;

	while (getline(ss, token, delimiter)) {
		result.push_back(token);
	}

	return result;
}



int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	ResultStatus status(0);

	try {
		const char* user = ITK_ask_cli_argument("-u=");
		const char* pass = ITK_ask_cli_argument("-p=");
		const char* grp = ITK_ask_cli_argument("-g=");

		if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function
			display();
			return ifail;
		}

		if ((user && tc_strcmp(user, "") != 0) && (pass && tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {
			ITK_init_to_login();
			status = ITK_init_module(user, pass, grp);
			AM__set_application_bypass(true);
			time_t tRawTime;
			struct tm* timeInfo;
			char timeStamp[20];
			time(&tRawTime);
			timeInfo = new struct tm;
			gmtime_s(timeInfo, &tRawTime);
			strftime(timeStamp, sizeof(timeStamp), "%d-%m-%Y %H:%M:%S", timeInfo);
			TC_write_syslog("[%s] User '%s' login successful.", timeStamp, user);
			tag_t tRevision = NULLTAG;
			tag_t tAssMtx = NULLTAG;
			int nsec = 0;
			tag_t* secObjects = NULL;
			status = ITEM_find_rev("000034", "A", &tRevision);
			cout << "tRevision : " << tRevision << endl;
			int iDeepCopyDataCount = 0;
			tag_t* tDeepCopyData = NULL;
			tag_t* tActualDeepData = NULL;
			status = TCTYPE_ask_deepcopydata(tRevision, TCTYPE_OPERATIONINPUT_SAVEAS, &iDeepCopyDataCount, &tDeepCopyData);
			cout << "iDeepCopyDataCount : " << iDeepCopyDataCount << endl;

			tag_t tObjectType = NULLTAG;
			status = TCTYPE_ask_object_type(tRevision, &tObjectType);

			scoped_smptr<char> s_objectType_name;
			status = TCTYPE_ask_name2(tObjectType, &s_objectType_name);
			cout << "Related Object Type: \n" << s_objectType_name.getString() << endl;

			tag_t tSaveAsObj = NULLTAG;
			status = TCTYPE_construct_saveasinput(tObjectType, &tSaveAsObj);

			scoped_smptr<char> cName;
			status = AOM_ask_value_string(tRevision, "object_name", &cName);

			status = AOM_set_value_string(tSaveAsObj, "object_name", cName.getString());

			cout << "Related Object Name: \n" << cName.getString() << endl;
			tag_t newSavedObj = NULLTAG;
			status = TCTYPE_saveas_object(tRevision, tSaveAsObj, iDeepCopyDataCount, tDeepCopyData, &newSavedObj);

			scoped_smptr<char> objName;
			int n_tags_in_list = 0;
			tag_t* all_deepcopydata_tags = NULL;
			tag_t targetObject = NULLTAG;
			
			tag_t bObject = NULLTAG;
			int relCount = 0;
			tag_t* matrix = NULL;
			tag_t bo_row = NULLTAG;

			status = AOM_ask_value_tags(newSavedObj, "Amx0AssignmentMatrixRel", &relCount, &matrix);
			tag_t* t_arrAssignMatrixRows = NULL;
			int assignMatrix_rows = 0;

			if (relCount > 0) {
				for (int i = 0; i < relCount; i++) {
					status = AOM_ask_value_string(matrix[i], "object_type", &objName);
					scoped_smptr<char> matName;
					scoped_smptr<char> itemID;
					scoped_smptr<char> itemRevID;
					string newMatName = "";
					cout << "objName : " << objName.getString() << endl;
					if (tc_strcmp(objName.getString(), "Amx0AssignmentMatrix") == 0) {
						int pn = 0;
						tag_t* amx0PropNameValues = NULL;
						int icount_Assigness = 0;
						tag_t* t_arrAssignees = NULL;
						status = AOM_ask_value_string(newSavedObj, "item_id", &itemID);
						status = AOM_ask_value_string(newSavedObj, "item_revision_id", &itemRevID);
						status = AOM_ask_value_string(matrix[i], "object_name", &matName);

						newMatName += itemID.getString();
						newMatName += "_";
						newMatName += itemRevID.getString();
						newMatName += "_";
						newMatName += matName.getString();
						cout << "\n newMatName: " << newMatName << endl;
						status = AOM_ask_value_tags(matrix[i], "amx0AssignmentMatrixRows", &assignMatrix_rows, &t_arrAssignMatrixRows);
						cout << "Number of Assignement Matrix Rows Object : " << assignMatrix_rows << endl;
						tag_t RowType = t_arrAssignMatrixRows[0];
						scoped_smptr<char> amx0AssignmentType;

						tag_t amx0AssignmentMatrixDefnTag = NULLTAG;
						status = AOM_ask_value_tag(matrix[i], "amx0AssignmentMatrixDefn", &amx0AssignmentMatrixDefnTag);

						tag_t tObjectType = NULLTAG;
						status = TCTYPE_ask_object_type(matrix[i], &tObjectType);

						scoped_smptr<char> s_objectType_name;
						status = TCTYPE_ask_name2(tObjectType, &s_objectType_name);
						cout << "Related Object Type: " << s_objectType_name.getString() << endl;

						tag_t tSaveAsInput = NULLTAG;
						status = TCTYPE_construct_create_input(tObjectType, &tSaveAsInput);

						scoped_smptr<char> cName;

						status = AOM_set_value_tag(tSaveAsInput, "amx0AssignmentMatrixDefn", amx0AssignmentMatrixDefnTag);

						status = AOM_set_value_string(tSaveAsInput, "object_name", newMatName.c_str());
						newMatName = "";
						status = TCTYPE_create_object(tSaveAsInput, &bObject);
						status = AOM_save_without_extensions(bObject);

						scoped_smptr<char> uid;
						ITK__convert_tag_to_uid(bObject, &uid);
						cout << "UID : " << uid.getString() << endl;

						//Create Assignment Matrix Rows Upto Order 6
						tag_t tAssigMatrixRow = NULLTAG;
						cout << "Create Assignement Matrix Row object \n";
						status = TCTYPE_ask_type("Amx0AssignmentMatrixRow", &tAssigMatrixRow);

						TC_write_syslog("Ask Object Type of Property Name Value string Object\n");
						//Get the Object Type of Property Name Value string Object
						tag_t tProNameValueStrObj = NULLTAG;
						status = TCTYPE_ask_type("Amx0PropNameValueString", &tProNameValueStrObj);

						for (int r = 0; r < assignMatrix_rows; r++) {
							tag_t tCreateInputRows = NULLTAG;
							scoped_smptr<char> propValue;
							status = TCTYPE_construct_create_input(tAssigMatrixRow, &tCreateInputRows);

							status = AOM_ask_value_string(t_arrAssignMatrixRows[r], "amx0AssignmentType", &amx0AssignmentType);
							cout << "amx0AssignmentType : " << amx0AssignmentType.getString() << endl;

							//Set Assignement Type on Rows Object
							status = AOM_set_value_string(tCreateInputRows, "amx0AssignmentType", amx0AssignmentType.getString());
							cout << "Set Assignement Type on Rows Object \n";

							//Set Assignment Matrix Rows Object on Assignement Matrix
							status = AOM_set_value_tag(tCreateInputRows, "amx0AssignmentMatrix", bObject);
							cout << "Set Assignment Matrix Object on Assignement Matrix Rows \n";

							//Add the Assignement  Matrix Rows Object
							tag_t tNewAssigMatrixROW = NULLTAG;
							status = TCTYPE_create_object(tCreateInputRows, &tNewAssigMatrixROW);

							//Save the Assignment Matrix Rows Object
							status = AOM_save_with_extensions(tNewAssigMatrixROW);
							cout << "New Assignement Matrix Row object created \n";

							//Construct Create Input for Property Name Value string Object
							tag_t tCreateInputPropNamVal = NULLTAG;
							status = TCTYPE_construct_create_input(tProNameValueStrObj, &tCreateInputPropNamVal);
							TC_write_syslog("Construct Create Input for Property Name Value string Object\n");


							//Ask the Property Name on Property Name Value string Object
							int iprop_Value = 0;
							tag_t* t_ArrPropValues = NULL;
							status = AOM_ask_value_tags(t_arrAssignMatrixRows[r], "amx0PropNameValues", &iprop_Value, &t_ArrPropValues);
							cout << "Number of Decisive Property present : \n" << iprop_Value <<endl;

							scoped_smptr<char> pNames;
							status = AOM_ask_value_string(t_ArrPropValues[0], "amx0PropertyName", &pNames);

							//Set the Property Name on Property Name Value string Object
							status = AOM_set_value_string(tCreateInputPropNamVal, "amx0PropertyName", pNames.getString());
							cout << "Set the Property Name on Property Name Value string Object\n";

							//Ask the Property Name on Property Name Value string Object
							
							scoped_smptr<char> pValue;
							status = AOM_ask_value_string(t_ArrPropValues[0], "amx0PropertyValue", &pValue);
							cout << "Set the Property Name on Property Name Value string Object\n";

							status = AOM_set_value_string(tCreateInputPropNamVal, "amx0PropertyValue", pValue.getString());
							cout << "Set the Property Value on Property Name Value string Object : %s\n"<< pValue.getString()<<endl;

							status = AOM_ask_value_tags(t_arrAssignMatrixRows[r], "amx0Assignees", &icount_Assigness, &t_arrAssignees);
							cout << "Number of Assignees present : " << icount_Assigness << endl;

							status = AOM_set_value_tags(tNewAssigMatrixROW, "amx0Assignees", icount_Assigness, t_arrAssignees);
							cout << "Set amx0Assignees : \n" << pValue.getString() << endl;
							status = AOM_save_with_extensions(tNewAssigMatrixROW);

							//Set the Property Name Value string Object on Assignment Matrix Rows Object
							status = AOM_set_value_tag(tCreateInputPropNamVal, "amx0AssignmentMatrixRow", tNewAssigMatrixROW);
							cout << "Set the  Assignment Matrix Rows on Property Name Value String Object \n";

							//Add the Property Name Value string Object
							tag_t tNewPropNamValue = NULLTAG;
							status = TCTYPE_create_object(tCreateInputPropNamVal, &tNewPropNamValue);

							//Save the  Property Name Value string Object
							status = AOM_save_with_extensions(tNewPropNamValue);
							cout << "Save the  Property Name Value string Object \n";
						}	
					}
					newMatName += "12345";
					newMatName += "_";
					newMatName += "C";
					newMatName += "_";
					newMatName += "Name";
					cout << "\n newMatName: " << newMatName << endl;
				}

			}
			status = AOM_refresh(newSavedObj, true);
			status = AOM_set_value_tags(newSavedObj, "Amx0AssignmentMatrixRel", 1, &bObject);
			status = AOM_save_without_extensions(newSavedObj);
			status = AOM_refresh(newSavedObj, false);
		}
		else {
			display();
			return ifail;
		}
		AM__set_application_bypass(false);
	}
	catch (IFail& ex) {
		ifail = ex.ifail();
		scoped_smptr <char> message;
		EMH_ask_error_text(ifail, &message);
		//writeToFile(message.getString());
		TC_write_syslog("\nThe error is %s", message.getString());
		cout << "\nThe error is " << message.getString();
	}
	return ifail;
}