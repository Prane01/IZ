#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <list>
#include <time.h>
#include <map>
#include <vector>

extern "C"
{
#include <tcinit/tcinit.h>
#include <tccore/aom.h>
#include <unidefs.h>
#include <epm/cr.h>
#include <epm/epm.h>
#include <ecm/ecm.h>
#include <pom/enq/enq.h>
#include <ps/ps.h>
#include <sa/sa.h>
#include <fclasses/tc_date.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include <lov/lov.h>
#include <tccore/tctype.h>
#include <sa/sa.h>
#include <tccore/grm.h>
#include <form/formtype.h>
#include <tc/tc_util.h>
#include <tccore/uom.h>
#include <tccore/idfr.h>
#include <textsrv/textserver.h>
#include <tc/preferences.h>
#include <ae/dataset.h>
#include <sa/tcvolume.h>
#include<epm/epm_task_template_itk.h>
#include <qry/qry.h>
#include<tccore/project.h>
}


using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#define SAFE_MEM_free(x)     \
  MEM_free(x);               \
  x = NULL;

#define ITK_ERR(X) (report_error( __FILE__, __LINE__, #X, (X)));

static int report_error(const char* file, int line, const char*, int rc)
{
	if ((rc) != ITK_ok)
	{
		char* msg = NULL;
		EMH_ask_error_text(rc, &msg);

		if (msg != NULL) {
			TC_write_syslog("***ERROR[%d]: %s file: %s  line:%d \n", rc, msg, file, line);
			printf("***ERROR[%d]: %s file: %s  line:%d \n", rc, msg, file, line);
			MEM_free(msg);
		}
	}
	return rc;
}

#include <stdio.h>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm> 

using namespace std;

void get_TimeStamp();
void create_csv_file(const char* path, const char* fileName, map<int, std::vector<std::vector<std::string> > > data);
int get_type_objects(char* searchCriteria, const char* startDate, const char* endDate,
	map<int, std::vector<std::vector<std::string> > > data, map<int, std::vector<std::vector<std::string> > > relation);
string concatenateStrings(string str1, string str2);
bool isSubstringContains(const string& fullString, const string& subString);
void findAndReplaceAll(string& data, const string& match, const string& replace);
std::pair<std::map<int, vector<std::vector<std::string> > >,
	std::map<int, vector<std::vector<std::string> > > > processAttrFile(const std::string& filePath);
string getPropValue(tag_t objectTag, const char* prop_name, const char* storage_type);
string printAttrValue(char* prop_value);
std::string getCombinedObjectByTag(int ObjectCount, tag_t* objectTags, std::vector<std::string> tokens);
int countChar(const std::string& str, char ch);
std::vector<std::string> splitString(const std::string& str, char delimiter);
void writeAttrValueInFile(std::vector<std::string> vector);
std::string printFormattedDate(const date_t& date);
bool isDateEmpty(const date_t& date);
void need_new_csv_file(map<int, std::vector<std::vector<std::string> > > data);
string freeAndGetValue(char* &ptr);
void replaceQuote(std::string& source);
void calculateUtilityPerformance(clock_t start);
int executeAuditLogQryJobName(const char* jobName, int &logRowsCount, tag_t* &logRows);
void printAuditLogToFile(tag_t auditLogRow, int isValidLog);
void printCSVHeaderColumns();
void printAffectedItemsInCSV(std::vector<std::vector<std::vector<std::string> > > allAffectedItems, std::vector<std::string> AttrForFile, map<int, std::vector<std::vector<std::string> > > data);
void getRelatedItemAttributesValue(string relation, int ObjectCount, tag_t* objectTags, int relationToken, std::vector<std::vector<std::string> > relatedItemAttributes, std::vector<std::vector<std::string> > &relatedItemAttributesValues);
int  printAffectedItemsRow(int isWFCase, int relationNumber, int countOfUniqueRel, vector<std::string> rowValueVector);
FILE * file;
stringstream ss;

const char* csvFilePath = NULL;
const char* csvFileFirstName = "ECN_ITK_Export";
char timeStamp[80];

int file_row_count = 2;
int file_number_count = 1;
int rowsTotal = 1;

int ITK_user_main(int argc, char* argv[])
{
	int status = ITK_ok;


	const char* user = ITK_ask_cli_argument("-u=");
	const char* pass = ITK_ask_cli_argument("-p=");
	const char* grp = ITK_ask_cli_argument("-g=");
	const char* startDate = ITK_ask_cli_argument("-startDate="); //Date formate : "01-Jul-2023 00:00:00" 
	const char* endDate = ITK_ask_cli_argument("-endDate=");     //Date formate : "01-Jul-2023 00:00:00" 
	char*  searchCriteria = ITK_ask_cli_argument("-searchCriteria=");  // Created/Modified/Released
	const char* attrFilePath = ITK_ask_cli_argument("-attrFilePath=");
	csvFilePath = ITK_ask_cli_argument("-fileOutPath=");

	printf("\n\n Input Arguments :- \n\t user:%s \n\t startDate:%s \n\t endDate:%s \n\t searchCriteria:%s \n\t outPath:%s  \n\t attrFilePath:%s", user, startDate, endDate, searchCriteria, csvFilePath, attrFilePath);



	std::pair<std::map<int, vector<std::vector<std::string> > >,
		std::map<int, vector<std::vector<std::string> > > > result = processAttrFile(attrFilePath);
	std::map<int, vector<std::vector<std::string> > > data = result.first;
	std::map<int, vector<std::vector<std::string> > > relation = result.second;

	// Check Attributes avaiable or not.
	if (data.empty() || data[0].empty()) {
		std::cerr << "\nError: No data found in file." << std::endl;
		return 1; // Exit function if no data found 
	}
	printf("\n Attribute File Loaded Successfully.");

	// Login to Teamcenter
	ITK_init_to_login();
	status = ITK_init_module(user, pass, grp);
	if (status != ITK_ok) {
		char* err_msg = NULL;
		EMH_ask_error_text(status, &err_msg);
		printf("\n Login failed. Status = %d and Error = %s \n", status, err_msg);
		printf("\n Login failed. Status = %d \n", status);
		SAFE_MEM_free(err_msg);
		exit(1);
	}

	ITK_set_bypass(true);

	clock_t start = clock();

	//Get TimeStamp
	get_TimeStamp();

	//CREATE csv fILE
	create_csv_file(csvFilePath, csvFileFirstName, data);

	//Execute Query create objects
	status = get_type_objects(searchCriteria, startDate, endDate, data, relation);

	fclose(file);

	// Logout from Teamcenter
	ITK_exit_module(true);

	calculateUtilityPerformance(start);

	return 0;

}

void printAuditLogToFile(tag_t auditLogRow, int isValidLog) {

	if (isValidLog == 1) {
		//fnd0LoggedDate ,
		char* sfnd0EventTypeName = NULL; char* sfnd0UserId = NULL; char* sobject_name = NULL; char* sobject_type = NULL; char* sjob_name = NULL; char* sprocess_templateDisp = NULL; char* stask_state = NULL; char* stask_result = NULL; char* scomments = NULL; char* sresponsible_partyDisp = NULL; char* sfnd0SecondaryObjDispName = NULL; char* sfnd0SecondaryObjectType = NULL; char* sfnd0SecondaryObjQualifier = NULL; char* sfnd0SignoffUserID = NULL;

		//char* dfnd0LoggedDate = NULL;
		date_t dfnd0LoggedDate;
		AOM_ask_value_date(auditLogRow, "fnd0LoggedDate", &dfnd0LoggedDate);
		string dfnd0LoggedDate_Formated = printFormattedDate(dfnd0LoggedDate);
		printf("\n Process Last Mod Date = %s", dfnd0LoggedDate_Formated.c_str());

		AOM_ask_value_string(auditLogRow, "fnd0EventTypeName", &sfnd0EventTypeName);
		printf("\n sfnd0EventTypeName = %s", sfnd0EventTypeName);
		string sfnd0EventTypeNameStr = freeAndGetValue(sfnd0EventTypeName);
		replaceQuote(sfnd0EventTypeNameStr);

		AOM_ask_value_string(auditLogRow, "fnd0UserId", &sfnd0UserId);
		printf("\n sfnd0UserId = %s", sfnd0UserId);
		string sfnd0UserIdStr = freeAndGetValue(sfnd0UserId);
		replaceQuote(sfnd0UserIdStr);

		AOM_ask_value_string(auditLogRow, "object_name", &sobject_name);
		printf("\n sobject_name = %s", sobject_name);
		string sobject_nameStr = freeAndGetValue(sobject_name);
		replaceQuote(sobject_nameStr);

		AOM_ask_value_string(auditLogRow, "object_type", &sobject_type);
		printf("\n sobject_type = %s", sobject_type);
		string sobject_typeStr = freeAndGetValue(sobject_type);
		replaceQuote(sobject_typeStr);

		AOM_ask_value_string(auditLogRow, "job_name", &sjob_name);
		printf("\n sjob_name = %s", sjob_name);
		string sjob_nameStr = freeAndGetValue(sjob_name);
		replaceQuote(sjob_nameStr);

		AOM_ask_value_string(auditLogRow, "process_templateDisp", &sprocess_templateDisp);
		printf("\n sprocess_templateDisp = %s", sprocess_templateDisp);
		string sprocess_templateDispStr = freeAndGetValue(sprocess_templateDisp);
		replaceQuote(sprocess_templateDispStr);

		AOM_ask_value_string(auditLogRow, "task_state", &stask_state);
		printf("\n stask_state = %s", stask_state);
		string stask_stateStr = freeAndGetValue(stask_state);
		replaceQuote(stask_stateStr);

		AOM_ask_value_string(auditLogRow, "task_result", &stask_result);
		printf("\n stask_result = %s", stask_result);
		string stask_resultStr = freeAndGetValue(stask_result);
		replaceQuote(stask_resultStr);

		AOM_ask_value_string(auditLogRow, "comments", &scomments);
		printf("\n scomments = %s", scomments);
		string scommentsStr = freeAndGetValue(scomments);
		replaceQuote(scommentsStr);

		AOM_ask_value_string(auditLogRow, "responsible_partyDisp", &sresponsible_partyDisp);
		printf("\n sresponsible_partyDisp = %s", sresponsible_partyDisp);
		string sresponsible_partyDispStr = freeAndGetValue(sresponsible_partyDisp);
		replaceQuote(sresponsible_partyDispStr);

		AOM_ask_value_string(auditLogRow, "fnd0SecondaryObjDispName", &sfnd0SecondaryObjDispName);
		printf("\n sfnd0SecondaryObjDispName = %s", sfnd0SecondaryObjDispName);
		string sfnd0SecondaryObjDispNameStr = freeAndGetValue(sfnd0SecondaryObjDispName);
		replaceQuote(sfnd0SecondaryObjDispNameStr);

		AOM_ask_value_string(auditLogRow, "fnd0SecondaryObjectType", &sfnd0SecondaryObjectType);
		printf("\n sfnd0SecondaryObjectType = %s", sfnd0SecondaryObjectType);
		string sfnd0SecondaryObjectTypeStr = freeAndGetValue(sfnd0SecondaryObjectType);
		replaceQuote(sfnd0SecondaryObjectTypeStr);

		AOM_ask_value_string(auditLogRow, "fnd0SecondaryObjQualifier", &sfnd0SecondaryObjQualifier);
		printf("\n sfnd0SecondaryObjQualifier = %s", sfnd0SecondaryObjQualifier);
		string sfnd0SecondaryObjQualifierStr = freeAndGetValue(sfnd0SecondaryObjQualifier);
		replaceQuote(sfnd0SecondaryObjQualifierStr);

		AOM_ask_value_string(auditLogRow, "fnd0SignoffUserID", &sfnd0SignoffUserID);
		printf("\n sfnd0SignoffUserID = %s", sfnd0SignoffUserID);
		string sfnd0SignoffUserIDStr = freeAndGetValue(sfnd0SignoffUserID);
		replaceQuote(sfnd0SignoffUserIDStr);

		sfnd0SignoffUserIDStr = "";

		fprintf(file, ",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", dfnd0LoggedDate_Formated.c_str(), sfnd0EventTypeNameStr.c_str(), sfnd0UserIdStr.c_str(), sobject_nameStr.c_str(), sobject_typeStr.c_str(), sjob_nameStr.c_str(), sprocess_templateDispStr.c_str(), stask_stateStr.c_str(), stask_resultStr.c_str(), scommentsStr.c_str(), sresponsible_partyDispStr.c_str(), sfnd0SecondaryObjDispNameStr.c_str(), sfnd0SecondaryObjectTypeStr.c_str(), sfnd0SecondaryObjQualifierStr.c_str(), sfnd0SignoffUserIDStr.c_str());

	}
	else {
		fprintf(file, ",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");
	}

}

int executeAuditLogQryJobName(const char* jobName, int &logRowsCount, tag_t* &logRows) {

	//printf("\n\n Entered into function : executeAuditLogQryJobName, Arg. Job Name : %s ", jobName);

	tag_t query = NULLTAG;
	QRY_find2("Audit - Workflow General", &query);
	int status = ITK_ok;
	int n_entries = 1;
	char **values = NULL;
	char **entries = NULL;

	entries = (char **)MEM_alloc(n_entries * sizeof(char *));
	entries[0] = (char *)MEM_alloc(strlen("Job Name") + 1);

	values = (char **)MEM_alloc(n_entries * sizeof(char *));
	values[0] = (char *)MEM_alloc(strlen(jobName) + 1);


	strcpy(entries[0], "Job Name");
	strcpy(values[0], jobName);

	status = QRY_execute(query, n_entries, entries, values, &logRowsCount, &logRows);
	//printf("\n Audit Log Query Execute Status : %d", status);

	//printf("\n\n Exit from function : executeAuditLogQryJobName");
	return status;

}


void checkAndPrintCMImplements(tag_t itemsTag, std::vector<std::string> AttrForFile, map<int, std::vector<std::vector<std::string> > > data) {

	tag_t relationTag = NULLTAG;
	int secCount = 0;
	tag_t* secondaryObjs = NULLTAG;

	int status = GRM_find_relation_type("CMImplements", &relationTag);
	if (status == ITK_ok) {
		status = GRM_list_secondary_objects_only(itemsTag, relationTag, &secCount, &secondaryObjs);
		if (status == ITK_ok) {
			printf("\n checkAndPrintCMImplements : Secondary Object Count = %d", secCount);
			TC_write_syslog("\n checkAndPrintCMImplements:  Impacted Items Count = %d \n", secCount);
		}
	}

	if (secCount > 0) {

		for (int i = 0; i < secCount; i++) {

			char* itemId = NULL;
			char* itemName = NULL;
			char* itemRevId = NULL;
			char* ecrProductLine = NULL;
			//ECR_ID, ECR_
			AOM_ask_value_string(secondaryObjs[i], "item_id", &itemId);
			printf("\n CMImplements : itemId : %s", itemId);
			string itemIdStr = freeAndGetValue(itemId);
			replaceQuote(itemIdStr);

			AOM_ask_value_string(secondaryObjs[i], "object_name", &itemName);
			printf("\n CMImplements : itemName : %s", itemName);
			string itemNameStr = freeAndGetValue(itemName);
			replaceQuote(itemNameStr);

			AOM_ask_value_string(secondaryObjs[i], "item_revision_id", &itemRevId);
			printf("\n CMImplements : itemRevId : %s", itemRevId);
			string itemRevIdStr = freeAndGetValue(itemRevId);
			replaceQuote(itemRevIdStr);

			AOM_ask_value_string(secondaryObjs[i], "h4_ProductLine", &ecrProductLine);
			printf("\n CMImplements : ecrProductLine : %s", ecrProductLine);
			string ecrProductLineStr = freeAndGetValue(ecrProductLine);
			replaceQuote(ecrProductLineStr);



			need_new_csv_file(data);
			writeAttrValueInFile(AttrForFile);

			fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", itemIdStr.c_str(), itemNameStr.c_str(), itemRevIdStr.c_str(), ecrProductLineStr.c_str());
			//122 +2+2 -1empty variable
			fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");

			//break;
		}

	}

	/*else {

		need_new_csv_file(data);
		writeAttrValueInFile(AttrForFile);

		fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", "", "", "", "");
		//122 empty variable
		fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "last2");
	}*/

	SAFE_MEM_free(secondaryObjs);
}

void calculateUtilityPerformance(clock_t start) {


	// Capture the end time
	clock_t end = clock();

	// Calculate the elapsed time in seconds
	double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

	// Output the time taken

	printf("\n\n**************************************************************************************");
	printf("\n Total count of CSV files generated: %d\n", file_number_count);
	printf("\n Count of total rows written: %d\n", rowsTotal);
	printf("\n Time taken to run utility: %f seconds\n", time_taken);
}

string freeAndGetValue(char* &ptr) {
	string temp = "";
	if (ptr) {
		temp = ptr;
		SAFE_MEM_free(ptr);
	}
	return temp;
}

void replaceQuote(std::string &source) {
	//printf("Entered into replace function");
	size_t pos = 0;
	string from = "\"";
	string to = "";
	if (source.size() > 0 && !source.empty()) {
		while ((pos = source.find("\"", pos)) != std::string::npos) {
			source.replace(pos, from.length(), to);
			pos += to.length(); // Move past the replacement
		}
	}
	else {
		source = "";

	}
	//printf("Exit from replace function");
	//return source;
}

void get_TimeStamp() {
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = gmtime(&rawtime);
	strftime(timeStamp, sizeof(timeStamp), "%d_%m_%Y_%H_%M_%S", timeinfo);
	printf("\n Current Time Stamp = %s", timeStamp);
	//TC_write_syslog("\n Current Time Stamp = %s", timeStamp);
}


void need_new_csv_file(map<int, std::vector<std::vector<std::string> > > data) {
	rowsTotal = rowsTotal + 1;
	if (file_row_count > 100000) {
		file_row_count = 3;
		fclose(file);
		file_number_count += 1;
		create_csv_file(csvFilePath, csvFileFirstName, data);
	}
	else {
		file_row_count += 1;
	}
}

void create_csv_file(const char* path, const char* fileName, map<int, std::vector<std::vector<std::string> > > data)
{
	char filepathlocation[1000];

	filepathlocation[0] = '\0';
	tc_strcpy(filepathlocation, path);
	tc_strcat(filepathlocation, "/");
	tc_strcat(filepathlocation, fileName);
	tc_strcat(filepathlocation, "_");
	tc_strcat(filepathlocation, timeStamp);
	tc_strcat(filepathlocation, "_");
	ss << file_number_count;
	tc_strcat(filepathlocation, ss.str().c_str());
	//tc_strcat(filepathlocation, std::to_string(file_number_count).c_str());
	tc_strcat(filepathlocation, ".csv");
	printf("\n filepathlocation path as %s", filepathlocation);
	//TC_write_syslog("\n filepathlocation path as %s", filepathlocation);

	// Open file in binary write mode
	file = fopen(filepathlocation, "wb");
	if (!file) {
		perror("Error opening file");

	}

	// Write UTF-8 BOM
	unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
	if (fwrite(bom, sizeof(unsigned char), sizeof(bom), file) != sizeof(bom)) {
		perror("Error writing BOM");
		fclose(file);

	}

	/*
	file = fopen(filepathlocation, "w");

	// Write UTF-8 BOM to ensure the file is recognized as UTF-8
	unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
	fwrite(bom, sizeof(bom), 1, file);
	}*/

	// for (const auto& pair : data) {
	// 	int count = pair.first;
	// 	const std::vector<std::vector<std::string> > vectors = pair.second;
	// 	if (count < 1) {
	// 		//std::cout << "Lines with " << count << " '>' characters:" << std::endl;
	// 		for (auto& partVector : vectors) {
	// 			string tmp = partVector[0];
	// 			replaceQuote(tmp);
	// 			fprintf(file, "\"%s\",", tmp.c_str());
	// 		}
	// 	}
	// }

	for (map<int, std::vector<std::vector<std::string> > >::iterator it = data.begin(); it != data.end(); ++it) {
		int count = it->first;
		const std::vector<std::vector<std::string> > vectors = it->second;
		if (count < 1) {
			//std::cout << "Lines with " << count << " '>' characters:" << std::endl;
			for (std::vector<std::vector<std::string> >::const_iterator vecIt = vectors.begin(); vecIt != vectors.end(); ++vecIt) {
				std::string tmp = (*vecIt)[0];
				replaceQuote(tmp);
				fprintf(file, "\"%s\",", tmp.c_str());
			}
		}
	}

	//Print other header columns in CSV file.
	printCSVHeaderColumns();

	//fclose(file);
}

void printCSVHeaderColumns() {

	fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", "CMImplements.item_id", "CMImplements.object_name", "CMImplements.item_revision_id", "CMImplements.h4_ProductLine");

	fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",",
		"CMHasProblemItem.object_string", "CMHasProblemItem.item_id", "CMHasProblemItem.h4_part_category", "CMHasProblemItem.h4_sap_transfer_status", "CMHasProblemItem.h4_project_accolade_id",
		"CMHasProblemItem.object_name", "CMHasProblemItem.object_type", "CMHasProblemItem.creation_date", "CMHasProblemItem.h4_release_status", "CMHasProblemItem.date_released", "CMHasProblemItem.owning_user", "CMHasProblemItem.item_revision_id", "CMHasProblemItem.h4_gold_business_enterprise", "CMHasProblemItem.h4_strategic_business_grp", "CMHasProblemItem.h4_strategic_business_unit", "CMHasProblemItem.h4_line_of_business", "CMHasProblemItem.h4_product_hierarchy", "CMHasProblemItem.h4_design_authority");

	fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",",
		"CMHasImpactedItem.object_string", "CMHasImpactedItem.item_id", "CMHasImpactedItem.h4_part_category", "CMHasImpactedItem.h4_sap_transfer_status", "CMHasImpactedItem.h4_project_accolade_id",
		"CMHasImpactedItem.object_name", "CMHasImpactedItem.object_type", "CMHasImpactedItem.creation_date", "CMHasImpactedItem.h4_release_status", "CMHasImpactedItem.date_released", "CMHasImpactedItem.owning_user", "CMHasImpactedItem.item_revision_id", "CMHasImpactedItem.h4_gold_business_enterprise", "CMHasImpactedItem.h4_strategic_business_grp", "CMHasImpactedItem.h4_strategic_business_unit", "CMHasImpactedItem.h4_line_of_business", "CMHasImpactedItem.h4_product_hierarchy", "CMHasImpactedItem.h4_design_authority");

	fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",",
		"CMHasSolutionItem.object_string", "CMHasSolutionItem.item_id", "CMHasSolutionItem.h4_part_category", "CMHasSolutionItem.h4_sap_transfer_status", "CMHasSolutionItem.h4_project_accolade_id",
		"CMHasSolutionItem.object_name", "CMHasSolutionItem.object_type", "CMHasSolutionItem.creation_date", "CMHasSolutionItem.h4_release_status", "CMHasSolutionItem.date_released", "CMHasSolutionItem.owning_user", "CMHasSolutionItem.item_revision_id", "CMHasSolutionItem.h4_gold_business_enterprise", "CMHasSolutionItem.h4_strategic_business_grp", "CMHasSolutionItem.h4_strategic_business_unit", "CMHasSolutionItem.h4_line_of_business", "CMHasSolutionItem.h4_product_hierarchy", "CMHasSolutionItem.h4_design_authority");

	fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",",
		"H4_AMPLItems.object_string", "H4_AMPLItems.item_id", "H4_AMPLItems.h4_part_category", "H4_AMPLItems.h4_sap_transfer_status", "H4_AMPLItems.h4_project_accolade_id",
		"H4_AMPLItems.object_name", "H4_AMPLItems.object_type", "H4_AMPLItems.creation_date", "H4_AMPLItems.h4_release_status", "H4_AMPLItems.date_released", "H4_AMPLItems.owning_user", "H4_AMPLItems.item_revision_id", "H4_AMPLItems.h4_gold_business_enterprise", "H4_AMPLItems.h4_strategic_business_grp", "H4_AMPLItems.h4_strategic_business_unit", "H4_AMPLItems.h4_line_of_business", "H4_AMPLItems.h4_product_hierarchy", "H4_AMPLItems.h4_design_authority");

	//37 +2 +2 -1 VARIABLES
	fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",", "gov_Classification", "WF.job_name", "WF.task_template", "WF.fnd0TaskPathName", "WF.task_result", "WF.state", "WF.comments", "WF.fnd0Priority", "WF.fnd0StartDate", "WF.fnd0EndDate", "WF.due_date", "WF.h4_escalation_level", "WF.the_task_duration", "WF.creation_date", "WF.last_mod_date", "WF.owning_user", "WF.task_type", "WF.resp_party", "WF.fnd0Performer", "Task.fnd0Assingee", "Task.fnd0AssingeeGroupRole", "Task.decision", "Task.decision_date", "TASK.task_template", "TASK.fnd0TaskPathName", "TASK.task_result", "TASK.state", "TASK.comments", "TASK.fnd0Priority", "TASK.fnd0StartDate", "TASK.fnd0EndDate", "TASK.due_date", "TASK.h4_escalation_level", "TASK.the_task_duration", "TASK.task_type", "TASK.last_mod_date", "TASK.creation_date", "TASK.resp_party", "TASK.fnd0Performer", "TASK.UID");


	fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", "WFAudit.fnd0LoggedDate", "WFAudit.fnd0EventTypeName", "WFAudit.fnd0UserId", "WFAudit.object_name", "WFAudit.object_type", "WFAudit.job_name", "WFAudit.process_templateDisp", "WFAudit.task_state", "WFAudit.task_result", "WFAudit.comments", "WFAudit.responsible_partyDisp", "WFAudit.fnd0SecondaryObjDispName", "WFAudit.fnd0SecondaryObjectType", "WFAudit.fnd0SecondaryObjQualifier", "WFAudit.fnd0SignoffUserID");


}

int getRelationCount(std::vector<std::vector<std::string> > relation) {

	std::vector<std::vector<std::string> > testrelation = relation;
	vector<std::string> uniqueRelations;

	for (int i = 0; i < testrelation.size(); i++) {

		string relationName = testrelation[i][0];
		//printf("\n relation name : %s", relationName.c_str());
		uniqueRelations.push_back(relationName);

	}

	std::sort(uniqueRelations.begin(), uniqueRelations.end());
	uniqueRelations.erase(std::unique(uniqueRelations.begin(), uniqueRelations.end()), uniqueRelations.end());

	printf("\nCount of total unique relations : %d", uniqueRelations.size());
	printf("\n For loop, Realtion : ");
	// Print the updated vector 
	for (int k = 0; k < uniqueRelations.size(); k++) {
		printf("%d. - %s,", k, uniqueRelations[k].c_str());
	}

	return uniqueRelations.size();
}

int  printAffectedItemsRow(int isWFCase, int relationNumber, int countOfUniqueRel, vector<std::string> rowValueVector) {

	int countOfRelatedAttributes = rowValueVector.size();

	printf("\n relationNumber:%d,countOfUniqueRel:%d , isWFCase:%d", relationNumber, countOfUniqueRel, isWFCase);

	if (isWFCase) {
		countOfRelatedAttributes = 18;
		relationNumber = 4;
		int emptyRelation = relationNumber * countOfRelatedAttributes;

		for (int ii = 0; ii < emptyRelation; ii++) {
			fprintf(file, "\"%s\",", "");
		}
		return 1;

	}

	for (int i = 0; i < relationNumber; i++) {

		for (int ii = 0; ii < countOfRelatedAttributes; ii++) {
			fprintf(file, "\"%s\",", "");
		}
	}


	for (int j = 0; j < countOfRelatedAttributes; j++) {

		//printf("\n\t\t %d. : %s", j, rowValueVector[j].c_str());
		fprintf(file, "\"%s\",", rowValueVector[j].c_str());

	}

	for (int i = countOfUniqueRel; i > relationNumber + 1; i--) {

		for (int ii = 0; ii < countOfRelatedAttributes; ii++) {
			fprintf(file, "\"%s\",", "");
		}
	}

	return 1;
}
void printAffectedItemsInCSV(std::vector<std::vector<std::vector<std::string> > > allAffectedItems, std::vector<std::string> AttrForFile, map<int, std::vector<std::vector<std::string> > > data) {

	//printf("\n\n Entered into function : printAffectedItemsInCSV");

	printf("\n Affected Items : \n\t Total count of unique related objects :%d", allAffectedItems.size());

	for (int i = 0; i < allAffectedItems.size(); i++) {

		//printf("\n\t Relation Index : %d.  Total count of attached objects: %d", i, allAffectedItems[i].size());



		for (int j = 0; j < allAffectedItems[i].size(); j++) {

			int attributesCount = allAffectedItems[i][j].size();
			//printf("\n\t Total count of attributes : %d", attributesCount);

			need_new_csv_file(data);
			writeAttrValueInFile(AttrForFile);
			fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", "", "", "", "");//CMImpliment objects
			printAffectedItemsRow(0, i, allAffectedItems.size(), allAffectedItems[i][j]);

			//WF 37+2 +2 -1 variables
			fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");

			printAuditLogToFile(NULLTAG, 0);

		}



	}
	printf("\n\n Exit from function : printAffectedItemsInCSV");

}

void getAccessorGroupRole(tag_t taskTag, string assigneeName, vector<string> &signOffPropValues) {

	//printf("\n Entered into getAccessorGroupRole ");
	tag_t relationTag = NULLTAG;
	int secCount = 0;
	tag_t* secondaryObjs = NULLTAG;
	string assignedGroupRole = "";

	int status = AOM_ask_value_tags(taskTag, "valid_signoffs", &secCount, &secondaryObjs);
	printf("\n status: %d, p-secCount : %d", status, secCount);

	if (secCount > 0) {


		for (int i = 0; i < secCount; i++) {


			tag_t fnd0AssigneeTag = NULLTAG;
			char* cpfnd0Assignee = NULL;

			//fnd0Assignee
			AOM_ask_value_tag(secondaryObjs[i], "fnd0Assignee", &fnd0AssigneeTag);
			AOM_ask_value_string(fnd0AssigneeTag, "object_string", &cpfnd0Assignee);
			//printf("\n Fnd0EPMSignoff : fnd0AssigneeGroupRole : %s", fnd0AssigneeGroupRole);
			string cpfnd0AssigneeStr = freeAndGetValue(cpfnd0Assignee);
			replaceQuote(cpfnd0AssigneeStr);
			printf("\n cpfnd0AssigneeStr :%s", cpfnd0AssigneeStr.c_str());

			//if (assigneeName == cpfnd0AssigneeStr) {
			if (true) {
				char* fnd0AssigneeGroupRole = NULL;
				char* decision = NULL;
				char* decision_date = NULL;
				char* assignee = NULL;

				signOffPropValues.clear();

				AOM_ask_value_string(secondaryObjs[i], "fnd0AssigneeGroupRole", &fnd0AssigneeGroupRole);
				assignedGroupRole = freeAndGetValue(fnd0AssigneeGroupRole);
				replaceQuote(assignedGroupRole);
				printf("\n assignedGroupRole :%s", assignedGroupRole.c_str());

				AOM_ask_value_string(secondaryObjs[i], "decision", &decision);
				string decisionStr = freeAndGetValue(decision);
				replaceQuote(decisionStr);
				printf("\n decisionStr :%s", decisionStr.c_str());

				AOM_ask_value_string(secondaryObjs[i], "decision_date", &decision_date);
				string decision_dateStr = freeAndGetValue(decision_date);
				replaceQuote(decision_dateStr);
				printf("\n decision_dateStr :%s", decision_dateStr.c_str());


				/*AOM_ask_value_string(secondaryObjs[i], "fnd0Assignee", &assignee);
				string assigneeStr = freeAndGetValue(assignee);
				replaceQuote(assigneeStr);
				printf("\n assigneeStr :%s", assigneeStr.c_str());*/

				signOffPropValues.push_back(assignedGroupRole);
				signOffPropValues.push_back(decisionStr);
				signOffPropValues.push_back(decision_dateStr);
				signOffPropValues.push_back(cpfnd0AssigneeStr);

			}
			else {
				signOffPropValues.push_back("");
				signOffPropValues.push_back("");
				signOffPropValues.push_back("");
				signOffPropValues.push_back("");
			}



			//if (i > 0) { assignedGroupRoleAll = fnd0AssigneeGroupRoleStr + ";" + assignedGroupRoleAll; }else { assignedGroupRoleAll = fnd0AssigneeGroupRoleStr; }
		}
		SAFE_MEM_free(secondaryObjs);
	}


	else {
		signOffPropValues.push_back("");
		signOffPropValues.push_back("");
		signOffPropValues.push_back("");
		signOffPropValues.push_back("");

	}
	//printf("\n Exit from getAccessorGroupRole ");



}


void getChildTaskDetails(tag_t tTaskTag) {

	char* epmTask_TaskName = NULL;
	char* epmTask_LastModDate = NULL;
	char* epmTask_TempName = NULL;
	char* epmTask_FNDTPName = NULL;
	char* epmTask_TaskResult = NULL;
	char* epmTask_Comments = NULL;
	char* epmTask_Priority = NULL;
	char* epmTask_SDate = NULL;
	char* epmTask_EDate = NULL;
	char* epmTask_DueDate = NULL;
	char* epmTask_EscLevel = NULL;
	char* epmTask_TaskDur = NULL;
	char* epmTask_CreationDate = NULL;
	char* epmTask_TaskType = NULL;
	char* epmTask_RespPty = NULL;
	char* epmTask_Performer = NULL;
	char* epmTaskUID = NULL;

	char* epmTask_assignee = NULL;
	tag_t epmTask_template = NULLTAG;
	int epmTask_State = 0;

	tag_t epmtask_assigneeTag = NULLTAG;
	tag_t epmtask_assigneeGroupRoleTag = NULLTAG;
	tag_t task_performer = NULLTAG;

	string epmTaskassigneeGroupRoleStr = "";
	string epmTaskdecisionStr = "";
	string epmTaskdecision_dateStr = "";

	AOM_ask_value_tag(tTaskTag, "fnd0Assignee", &epmtask_assigneeTag);
	AOM_ask_value_string(epmtask_assigneeTag, "object_string", &epmTask_assignee);
	printf("\n  Task fnd0Assignee = %s", epmTask_assignee);
	string epmTasktask_assigneeStr = freeAndGetValue(epmTask_assignee);
	replaceQuote(epmTasktask_assigneeStr);

	AOM_ask_value_string(tTaskTag, "resp_party", &epmTask_RespPty);
	printf("\n Task Responsible Party = %s", epmTask_RespPty);

	string epmTask_RespPtyStr = freeAndGetValue(epmTask_RespPty);
	replaceQuote(epmTask_RespPtyStr);

	//printf("\nBefore performer");

	AOM_ask_value_tag(tTaskTag, "fnd0Performer", &task_performer);
	//SA_ask_user_person_name2(task_performer, &epmTask_Performer);
	AOM_ask_value_string(task_performer, "object_string", &epmTask_Performer);
	printf("\n Task Performer = %s", epmTask_Performer);
	//TC_write_syslog("\n Task Performer = %s", epmTask_Performer);

	string epmTask_PerformerStr = freeAndGetValue(epmTask_Performer);
	replaceQuote(epmTask_PerformerStr);

	AOM_ask_value_string(tTaskTag, "task_type", &epmTask_TaskType);
	printf("\n Task Type = %s", epmTask_TaskType);
	string epmTask_TaskTypeStr = freeAndGetValue(epmTask_TaskType);
	replaceQuote(epmTask_TaskTypeStr);

	vector<string> signOffPropValues;
	if (isSubstringContains(epmTask_TaskTypeStr, "EPMPerformSignoffTask")) {

		getAccessorGroupRole(tTaskTag, epmTasktask_assigneeStr, signOffPropValues);
		epmTaskassigneeGroupRoleStr = signOffPropValues[0];
		epmTaskdecisionStr = signOffPropValues[1];
		epmTaskdecision_dateStr = signOffPropValues[2];
		epmTask_PerformerStr = signOffPropValues[3];
		epmTasktask_assigneeStr = signOffPropValues[3];

	}


	signOffPropValues.clear();


	AOM_ask_value_string(tTaskTag, "object_name", &epmTask_TaskName);
	printf("\n Task Name = %s", epmTask_TaskName);
	//TC_write_syslog("\n Task Name = %s", epmTask_TaskName);
	string epmTask_TaskNameStr = freeAndGetValue(epmTask_TaskName);
	replaceQuote(epmTask_TaskNameStr);

	date_t T_last_mod_date;
	AOM_ask_value_date(tTaskTag, "last_mod_date", &T_last_mod_date);
	string epmTask_LastModDate_Formated = printFormattedDate(T_last_mod_date);
	printf("\n Task Last Mod Date = %s", epmTask_LastModDate_Formated.c_str());
	//TC_write_syslog("\n Task Last Mod Date = %s", epmTask_LastModDate_Formated.c_str());

	AOM_ask_value_tag(tTaskTag, "task_template", &epmTask_template);
	AOM_ask_value_string(epmTask_template, "object_string", &epmTask_TempName);
	printf("\n Task Template Name = %s", epmTask_TempName);
	//TC_write_syslog("\n Task Template Name = %s", epmTask_TempName);
	string epmTask_TempNameStr = freeAndGetValue(epmTask_TempName);
	replaceQuote(epmTask_TempNameStr);

	AOM_ask_value_string(tTaskTag, "fnd0TaskPathName", &epmTask_FNDTPName);
	printf("\n Task Path = %s", epmTask_FNDTPName);
	//TC_write_syslog("\n Task Path Name = %s", epmTask_FNDTPName);
	string epmTask_FNDTPNameStr = freeAndGetValue(epmTask_FNDTPName);
	replaceQuote(epmTask_FNDTPNameStr);

	AOM_ask_value_string(tTaskTag, "task_result", &epmTask_TaskResult);
	printf("\n Task Result = %s", epmTask_TaskResult);
	//TC_write_syslog("\n Task Result Name = %s", epmTask_TaskResult);
	string epmTask_TaskResultStr = freeAndGetValue(epmTask_TaskResult);
	replaceQuote(epmTask_TaskResultStr);

	AOM_ask_value_int(tTaskTag, "state", &epmTask_State);
	printf("\n Task State = %d", epmTask_State);
	//TC_write_syslog("\n Task State = %d, epmTask_State);

	AOM_ask_value_string(tTaskTag, "comments", &epmTask_Comments);
	string epmTask_CommentsTemp = freeAndGetValue(epmTask_Comments);
	findAndReplaceAll(epmTask_CommentsTemp, "\n", " ");
	printf("\n Task Comment = %s", epmTask_CommentsTemp.c_str());
	//TC_write_syslog("\n Task Comment = %s", epmTask_CommentsTemp.c_str());

	replaceQuote(epmTask_CommentsTemp);

	AOM_ask_value_string(tTaskTag, "fnd0Priority", &epmTask_Priority);
	string epmTask_PriorityTemp = freeAndGetValue(epmTask_Priority);
	findAndReplaceAll(epmTask_PriorityTemp, "\n", " ");
	printf("\n Task Priority = %s", epmTask_PriorityTemp.c_str());
	//TC_write_syslog("\n Task Priority = %s", epmTask_PriorityTemp.c_str());
	replaceQuote(epmTask_PriorityTemp);

	date_t T_fnd0StartDate;
	AOM_ask_value_date(tTaskTag, "fnd0StartDate", &T_fnd0StartDate);
	string epmTask_SDate_Formated = printFormattedDate(T_fnd0StartDate);
	printf("\n Task Start Date = %s", epmTask_SDate_Formated.c_str());
	//TC_write_syslog("\n Task Start Date = %s", epmTask_SDate_Formated.c_str());

	date_t T_fnd0EndDate;
	AOM_ask_value_date(tTaskTag, "fnd0EndDate", &T_fnd0EndDate);
	string epmTask_EDate_Formated = printFormattedDate(T_fnd0EndDate);
	printf("\n Task End Date = %s", epmTask_EDate_Formated.c_str());
	//TC_write_syslog(""\n Task End Date = %s", epmTask_EDate_Formated.c_str());



	date_t T_due_date;
	AOM_ask_value_date(tTaskTag, "due_date", &T_due_date);
	string epmTask_DueDate_Formated = printFormattedDate(T_due_date);
	printf("\n Task Due Date = %s", epmTask_DueDate_Formated.c_str());
	//TC_write_syslog("\n Task Due Date = %s", epmTask_DueDate_Formated.c_str());

	AOM_ask_value_string(tTaskTag, "h4_escalation_level", &epmTask_EscLevel);
	printf("\n Task Escalation Level = %s", epmTask_EscLevel);
	//TC_write_syslog("\n Task Escalation Level = %s", epmTask_EscLevel);
	string epmTask_EscLevelStr = freeAndGetValue(epmTask_EscLevel);
	replaceQuote(epmTask_EscLevelStr);

	AOM_ask_value_string(tTaskTag, "the_task_duration", &epmTask_TaskDur);
	printf("\n Task Duration = %s", epmTask_TaskDur);
	//TC_write_syslog("\n Task Duration = %s", epmTask_TaskDur);
	string epmTask_TaskDurStr = freeAndGetValue(epmTask_TaskDur);
	replaceQuote(epmTask_TaskDurStr);

	date_t T_creation_date;
	AOM_ask_value_date(tTaskTag, "creation_date", &T_creation_date);
	string epmTask_CreationDate_Formated = printFormattedDate(T_creation_date);
	printf("\n Task Creation Date = %s", epmTask_CreationDate_Formated.c_str());





	ITK__convert_tag_to_uid(tTaskTag, &epmTaskUID);
	printf("\n Task epmTaskUID = %s", epmTaskUID);
	string epmTaskUIDStr = freeAndGetValue(epmTaskUID);
	replaceQuote(epmTaskUIDStr);


	//37 +2 +2 -1 variables -19
	fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"", epmTasktask_assigneeStr.c_str(), epmTaskassigneeGroupRoleStr.c_str(), epmTaskdecisionStr.c_str(), epmTaskdecision_dateStr.c_str(), epmTask_TempNameStr.c_str(), epmTask_FNDTPNameStr.c_str(), epmTask_TaskResultStr.c_str(), epmTask_State, epmTask_CommentsTemp.c_str(), epmTask_PriorityTemp.c_str(), epmTask_SDate_Formated.c_str(), epmTask_EDate_Formated.c_str(), epmTask_DueDate_Formated.c_str(), epmTask_EscLevelStr.c_str(), epmTask_TaskDurStr.c_str(), epmTask_TaskTypeStr.c_str(), epmTask_LastModDate_Formated.c_str(), epmTask_CreationDate_Formated.c_str(), epmTask_RespPtyStr.c_str(), epmTask_PerformerStr.c_str(), epmTaskUIDStr.c_str());

	SAFE_MEM_free(epmTask_TempName);
	SAFE_MEM_free(epmTask_FNDTPName);
	SAFE_MEM_free(epmTask_TaskResult);
	SAFE_MEM_free(epmTask_Comments);
	SAFE_MEM_free(epmTask_Priority);
	SAFE_MEM_free(epmTask_SDate);
	SAFE_MEM_free(epmTask_EDate);
	SAFE_MEM_free(epmTask_DueDate);

	SAFE_MEM_free(epmTask_EscLevel);
	SAFE_MEM_free(epmTask_TaskDur);
	SAFE_MEM_free(epmTask_CreationDate);

	SAFE_MEM_free(epmTask_TaskType);
	SAFE_MEM_free(epmTask_RespPty);
	SAFE_MEM_free(epmTask_Performer);


	SAFE_MEM_free(epmTask_TaskName);
	SAFE_MEM_free(epmTask_LastModDate);

}
int get_type_objects(char* searchCriteria, const char* startDate, const char* endDate,
	map<int, std::vector<std::vector<std::string> > > data, map<int, std::vector<std::vector<std::string> > > relation) {

	// 	printf("\nEntered into Get Type Objects function.");

	// 	string afterCriteria = searchCriteria + string(" After");
	// 	string beforeCriteria = searchCriteria + string(" Before");

	// 	printf("\n\n afterCriteria: %s , beforeCriteria: %s", afterCriteria.c_str(), beforeCriteria.c_str());


	// 	int status = ITK_ok;

	// 	tag_t query = NULLTAG;
	// 	QRY_find2("Item Revision...", &query);

	// 	int n_entries = 3;
	// 	char **values = NULL;
	// 	char **entries = NULL;

	// 	const char* object_type = "Engineering Change Notice Revision";////Engineering Change Notice Revision, Engineering Work Request Revision, Obsolescence Notice Revision, 


	// 	entries = (char **)MEM_alloc(n_entries * sizeof(char *));
	// 	entries[0] = (char *)MEM_alloc(strlen("Type") + 1);

	// 	entries[1] = (char *)MEM_alloc(strlen(afterCriteria.c_str()) + 1);
	// 	entries[2] = (char *)MEM_alloc(strlen(beforeCriteria.c_str()) + 1);

	// 	values = (char **)MEM_alloc(n_entries * sizeof(char *));
	// 	values[0] = (char *)MEM_alloc(strlen(object_type) + 1);
	// 	values[1] = (char *)MEM_alloc(strlen(startDate) + 1);
	// 	values[2] = (char *)MEM_alloc(strlen(endDate) + 1);

	// 	strcpy(entries[0], "Type");
	// 	strcpy(values[0], object_type);

	// 	strcpy(entries[1], afterCriteria.c_str());
	// 	strcpy(values[1], startDate);

	// 	strcpy(entries[2], beforeCriteria.c_str());
	// 	strcpy(values[2], endDate);

	// 	tag_t *items = NULL;

	// 	int n_items = 0;
	// 	//int n_values = 0;

	// 	status = QRY_execute(query, n_entries, entries, values, &n_items, &items);

	// 	printf("\n Query Execute Status : %d, %d record found \n", status, n_items);

	// 	if (status == ITK_ok) {
	// 		int iProcessCount = 0;
	// 		int ecnPState = 0;
	// 		int iTaskCount = 0;


	// 		tag_t task_template = NULLTAG;
	// 		tag_t powning_user = NULLTAG;



	// 		tag_t* tpProcessList = NULLTAG;
	// 		tag_t* tTaskList = NULLTAG;

	// 		char* gov_class = NULL;
	// 		char* ecnProcessJobName = NULL;
	// 		char* ecnPTempName = NULL;
	// 		char* ecnPFNDTPName = NULL;
	// 		char* ecnPTaskResult = NULL;
	// 		char* ecnPComments = NULL;
	// 		char* ecnPPriority = NULL;
	// 		char* ecnPSDate = NULL;
	// 		char* ecnPEDate = NULL;
	// 		char* ecnPDueDate = NULL;
	// 		char* ecnPEscLevel = NULL;
	// 		char* ecnPTaskDur = NULL;
	// 		char* ecnPCreationDate = NULL;
	// 		char* ecnPLastModDate = NULL;
	// 		char* ecnPOwningUser = NULL;

	// 		char* pTask_TaskType = NULL;
	// 		char* pTask_RespPty = NULL;
	// 		char* ptask_performer = NULL;
	// 		tag_t pttask_performer = NULLTAG;
	// 		vector<std::string> rowValueVectorEmpty;

	// 		for (int ii = 0; ii < n_items; ii++) {
	// 			AOM_ask_value_string(items[ii], "gov_classification", &gov_class);
	// 			printf("\n Gov Classificaion = %s", gov_class);
	// 			string gov_classStr = freeAndGetValue(gov_class);
	// 			replaceQuote(gov_classStr);
	// 			//TC_write_syslog("\n Gov Classificaion = %s\n", gov_class);
	// 			if (tc_strcmp(gov_class, "ESO1.0") == 0 || tc_strcmp(gov_class, NULL) == 0 || tc_strcmp(gov_class, "") == 0) {
	// 				int n = 1;

	// 				std::vector<std::string> AttrForFile;
	// 				std::vector<std::vector<std::vector<std::string> > > allAffectedItemsByRelation;

	// 				std::vector<std::string> affectedItems;

	// 				for (const auto& tokens : data[0]) {
	// 					string value = getPropValue(items[ii], tokens[0].c_str(), tokens[1].c_str());
	// 					//replaceQuote(value);
	// 					printf("\n%d. %s ->  %s", n, tokens[0].c_str(), value.c_str());
	// 					AttrForFile.push_back(value);
	// 					n += 1;
	// 				}

	// 				if (relation.size() > 0) {
	// 					string relationToken;
	// 					int secCount = 0;
	// 					tag_t* secondaryObjs = NULLTAG;
	// 					int rel_token_size = relation[1].size();
	// 					int relationCount = getRelationCount(relation[1]);

	// 					int relatedPropCount = rel_token_size / relationCount;
	// 					printf("\n rel_token_size = %d, relationCount : %d, relatedPropCount:%d", rel_token_size, relationCount, relatedPropCount);

	// 					int relAttIndex = 0;
	// 					for (int ij = 0; ij < rel_token_size; ij++) {
	// 						string value;
	// 						if (relationToken.empty() || relationToken != relation[1][ij][0]) {

	// 							if (relationToken != relation[1][ij][0]) {
	// 								SAFE_MEM_free(secondaryObjs);
	// 							}
	// 							vector<std::vector<std::string> > relatedItemAttributesValues;

	// 							relationToken = relation[1][ij][0];
	// 							tag_t relationTag = NULLTAG;
	// 							status = GRM_find_relation_type(relationToken.c_str(), &relationTag);
	// 							if (status == ITK_ok) {
	// 								status = GRM_list_secondary_objects_only(items[ii], relationTag, &secCount, &secondaryObjs);
	// 								if (status == ITK_ok) {
	// 									printf("\n Secondary Object Count = %d", secCount);
	// 									//TC_write_syslog("\n Impacted Items Count = %d \n", secCount);
	// 								}
	// 							}
	// 							if (secCount > 0)
	// 							{

	// 								value = getCombinedObjectByTag(secCount, secondaryObjs, data[1][ij]);

	// 								getRelatedItemAttributesValue(relationToken.c_str(), secCount, secondaryObjs, relatedPropCount, data[1], relatedItemAttributesValues);

	// 								allAffectedItemsByRelation.push_back(relatedItemAttributesValues);

	// 								relatedItemAttributesValues.clear();

	// 								relAttIndex = 0;

	// 							}
	// 							else {

	// 								vector<std::vector<std::string> > relatedItemAttributesValues;
	// 								vector<std::string> relatedItememptyValues;

	// 								for (int l = 0; l < relatedPropCount; l++) {
	// 									relatedItememptyValues.push_back(" ");

	// 								}

	// 								relatedItemAttributesValues.push_back(relatedItememptyValues);
	// 								allAffectedItemsByRelation.push_back(relatedItemAttributesValues);

	// 								relatedItememptyValues.clear();
	// 								relatedItemAttributesValues.clear();

	// 							}
	// 						}
	// 						else {
	// 							if (secCount > 0) {
	// 								//printf("\n checking next and previous relation are same or not = %s, %s", relation[1][ij][0].c_str(), relationToken.c_str());
	// 								if (relationToken == relation[1][ij][0]) {
	// 									value = getCombinedObjectByTag(secCount, secondaryObjs, data[1][ij]);
	// 								}
	// 							}
	// 						}
	// 						//printf("\n%d. %s", n, value.c_str());
	// 						printf("\n%d.%d Relation:%s,  %s ->  %s", n, relAttIndex, relationToken.c_str(), data[1][ij][0].c_str(), value.c_str());
	// 						relAttIndex = relAttIndex + 1;
	// 						//AttrForFile.push_back(value);
	// 					}
	// 					SAFE_MEM_free(secondaryObjs);

	// 					checkAndPrintCMImplements(items[ii], AttrForFile, data);

	// 					//Print all affected items related attributes values
	// 					printAffectedItemsInCSV(allAffectedItemsByRelation, AttrForFile, data);


	// 					allAffectedItemsByRelation.clear();
	// 					printf("\n allAffectedItems vector cleared");

	// 					n += 1;
	// 				}

	printf("\nEntered into Get Type Objects function.");

	string afterCriteria = searchCriteria + string(" After");
	string beforeCriteria = searchCriteria + string(" Before");

	printf("\n\n afterCriteria: %s , beforeCriteria: %s", afterCriteria.c_str(), beforeCriteria.c_str());


	int status = ITK_ok;

	tag_t query = NULLTAG;
	QRY_find2("Item Revision...", &query);

	int n_entries = 3;
	char **values = NULL;
	char **entries = NULL;

	const char* object_type = "Engineering Change Notice Revision";////Engineering Change Notice Revision, Engineering Work Request Revision, Obsolescence Notice Revision,


	entries = (char **)MEM_alloc(n_entries * sizeof(char *));
	entries[0] = (char *)MEM_alloc(strlen("Type") + 1);

	entries[1] = (char *)MEM_alloc(strlen(afterCriteria.c_str()) + 1);
	entries[2] = (char *)MEM_alloc(strlen(beforeCriteria.c_str()) + 1);

	values = (char **)MEM_alloc(n_entries * sizeof(char *));
	values[0] = (char *)MEM_alloc(strlen(object_type) + 1);
	values[1] = (char *)MEM_alloc(strlen(startDate) + 1);
	values[2] = (char *)MEM_alloc(strlen(endDate) + 1);

	strcpy(entries[0], "Type");
	strcpy(values[0], object_type);

	strcpy(entries[1], afterCriteria.c_str());
	strcpy(values[1], startDate);

	strcpy(entries[2], beforeCriteria.c_str());
	strcpy(values[2], endDate);

	tag_t *items = NULL;

	int n_items = 0;
	//int n_values = 0;

	status = QRY_execute(query, n_entries, entries, values, &n_items, &items);

	printf("\n Query Execute Status : %d, %d record found \n", status, n_items);

	if (status == ITK_ok) {
		int iProcessCount = 0;
		int ecnPState = 0;
		int iTaskCount = 0;


		tag_t task_template = NULLTAG;
		tag_t powning_user = NULLTAG;


		tag_t* tpProcessList = NULLTAG;
		tag_t* tTaskList = NULLTAG;

		char* gov_class = NULL;
		char* ecnProcessJobName = NULL;
		char* ecnPTempName = NULL;
		char* ecnPFNDTPName = NULL;
		char* ecnPTaskResult = NULL;
		char* ecnPComments = NULL;
		char* ecnPPriority = NULL;
		char* ecnPSDate = NULL;
		char* ecnPEDate = NULL;
		char* ecnPDueDate = NULL;
		char* ecnPEscLevel = NULL;
		char* ecnPTaskDur = NULL;
		char* ecnPCreationDate = NULL;
		char* ecnPLastModDate = NULL;
		char* ecnPOwningUser = NULL;
		char* pTask_TaskType = NULL;
		char* pTask_RespPty = NULL;
		char* ptask_performer = NULL;
		tag_t pttask_performer = NULLTAG;
		vector<std::string> rowValueVectorEmpty;

		for (int ii = 0; ii < n_items; ii++) {
			AOM_ask_value_string(items[ii], "gov_classification", &gov_class);
			printf("\n Gov Classificaion = %s", gov_class);
			string gov_classStr = freeAndGetValue(gov_class);
			replaceQuote(gov_classStr);
			//TC_write_syslog("\n Gov Classificaion = %s\n", gov_class);
			if (tc_strcmp(gov_class, "ESO1.0") == 0 || tc_strcmp(gov_class, NULL) == 0 || tc_strcmp(gov_class, "") == 0) {
				int n = 1;

				std::vector<std::string> AttrForFile;
				std::vector<std::vector<std::vector<std::string> > > allAffectedItemsByRelation;

				std::vector<std::string> affectedItems;

				for (std::vector<std::vector<std::string> >::iterator it = data[0].begin(); it != data[0].end(); ++it) {
					string value = getPropValue(items[ii], (*it)[0].c_str(), (*it)[1].c_str());
					//replaceQuote(value);
					printf("\n%d. %s ->  %s", n, (*it)[0].c_str(), value.c_str());
					AttrForFile.push_back(value);
					n += 1;
				}

				if (relation.size() > 0) {
					string relationToken;
					int secCount = 0;
					tag_t* secondaryObjs = NULLTAG;
					int rel_token_size = relation[1].size();
					int relationCount = getRelationCount(relation[1]);

					int relatedPropCount = rel_token_size / relationCount;
					printf("\n rel_token_size = %d, relationCount : %d, relatedPropCount:%d", rel_token_size, relationCount, relatedPropCount);

					int relAttIndex = 0;
					for (int ij = 0; ij < rel_token_size; ij++) {
						string value;
						if (relationToken.empty() || relationToken != relation[1][ij][0]) {

							if (relationToken != relation[1][ij][0]) {
								SAFE_MEM_free(secondaryObjs);
							}
							vector<std::vector<std::string> > relatedItemAttributesValues;

							relationToken = relation[1][ij][0];
							tag_t relationTag = NULLTAG;
							status = GRM_find_relation_type(relationToken.c_str(), &relationTag);
							if (status == ITK_ok) {
								status = GRM_list_secondary_objects_only(items[ii], relationTag, &secCount, &secondaryObjs);
								if (status == ITK_ok) {
									printf("\n Secondary Object Count = %d", secCount);
									//TC_write_syslog("\n Impacted Items Count = %d \n", secCount);
								}
							}
							if (secCount > 0)
							{

								value = getCombinedObjectByTag(secCount, secondaryObjs, data[1][ij]);

								getRelatedItemAttributesValue(relationToken.c_str(), secCount, secondaryObjs, relatedPropCount, data[1], relatedItemAttributesValues);

								allAffectedItemsByRelation.push_back(relatedItemAttributesValues);

								relatedItemAttributesValues.clear();

								relAttIndex = 0;

							}
							else {

								vector<std::vector<std::string> > relatedItemAttributesValues;
								vector<std::string> relatedItememptyValues;

								for (int l = 0; l < relatedPropCount; l++) {
									relatedItememptyValues.push_back(" ");

								}

								relatedItemAttributesValues.push_back(relatedItememptyValues);
								allAffectedItemsByRelation.push_back(relatedItemAttributesValues);

								relatedItememptyValues.clear();
								relatedItemAttributesValues.clear();

							}
						}
						else {
							if (secCount > 0) {
								//printf("\n checking next and previous relation are same or not = %s, %s", relation[1][ij][0].c_str(), relationToken.c_str());
								if (relationToken == relation[1][ij][0]) {
									value = getCombinedObjectByTag(secCount, secondaryObjs, data[1][ij]);
								}
							}
						}
						//printf("\n%d. %s", n, value.c_str());
						printf("\n%d.%d Relation:%s,  %s ->  %s", n, relAttIndex, relationToken.c_str(), data[1][ij][0].c_str(), value.c_str());
						relAttIndex = relAttIndex + 1;
						//AttrForFile.push_back(value);
					}
					SAFE_MEM_free(secondaryObjs);

					checkAndPrintCMImplements(items[ii], AttrForFile, data);

					//Print all affected items related attributes values
					printAffectedItemsInCSV(allAffectedItemsByRelation, AttrForFile, data);


					allAffectedItemsByRelation.clear();
					printf("\n allAffectedItems vector cleared");

					n += 1;
				}


				AOM_ask_value_tags(items[ii], "fnd0AllWorkflows", &iProcessCount, &tpProcessList);
				printf("\n Process Template Count = %d", iProcessCount);
				//TC_write_syslog("\n Process Template Count = %d", iProcessCount);

				if (iProcessCount > 0) {

					int isAnyTargetWFInitiated = 0;

					for (int ij = 0; ij < iProcessCount; ij++)
					{

						char* P_attr_value = NULL;

						//char* ptask_assignee = NULL;
						char* cpassignedGroupRole = NULL;
						char* escalationLevel = NULL;

						EPM_ask_name2(tpProcessList[ij], &P_attr_value);
						string P_ProcessName_temp = freeAndGetValue(P_attr_value);
						printf("\n Process Template Name = %s", P_ProcessName_temp.c_str());
						//TC_write_syslog("\n Process Template Name = %s", P_ProcessName_temp.c_str());

						//"Test Review Workflow" with ""HON EWM ECN Process" after dev env.
						//if (true) {
						if (isSubstringContains(P_ProcessName_temp, "HON ECN Workflow") || isSubstringContains(P_ProcessName_temp, "HON ECN CM Only workflow")
							|| isSubstringContains(P_ProcessName_temp, "HON ECN AMPL Approval") || isSubstringContains(P_ProcessName_temp, "HON Obsolete Workflow")
							|| isSubstringContains(P_ProcessName_temp, "HON ECN T&M Workflow") || isSubstringContains(P_ProcessName_temp, "HON IGS ECN Quick Release")
							|| isSubstringContains(P_ProcessName_temp, "HON IGS ECN Workflow") || isSubstringContains(P_ProcessName_temp, "HON ECN WISMAR Workflow Projects")
							|| isSubstringContains(P_ProcessName_temp, "HON EWM ECN Process")) {


							isAnyTargetWFInitiated = 1;

							date_t P_last_mod_date;
							AOM_ask_value_date(tpProcessList[ij], "last_mod_date", &P_last_mod_date);
							string ecnPLastModDate_Formated = printFormattedDate(P_last_mod_date);
							printf("\n Process Last Mod Date = %s", ecnPLastModDate_Formated.c_str());
							//TC_write_syslog("\n Process Last Mod Date = %s", ecnPLastModDate_Formated.c_str());

							AOM_ask_value_string(tpProcessList[ij], "job_name", &ecnProcessJobName);
							printf("\n Workflow Name display = %s", ecnProcessJobName);
							//TC_write_syslog(" Workflow Name display = %s \n", ecnProcessJobName);
							string ecnProcessJobNameStr = freeAndGetValue(ecnProcessJobName);
							replaceQuote(ecnProcessJobNameStr);

							AOM_ask_value_tag(tpProcessList[ij], "task_template", &task_template);
							AOM_ask_value_string(task_template, "object_string", &ecnPTempName);
							printf("\n Process Task Template Name = %s", ecnPTempName);
							//TC_write_syslog("\n Process Task Template Name = %s", ecnPTempName);
							string ecnPTempNameStr = freeAndGetValue(ecnPTempName);
							replaceQuote(ecnPTempNameStr);

							AOM_ask_value_string(tpProcessList[ij], "fnd0TaskPathName", &ecnPFNDTPName);
							printf("\n Process Task Path = %s", ecnPFNDTPName);
							//TC_write_syslog("\n Process Task Path Name = %s", ecnPFNDTPName);
							string ecnPFNDTPNameStr = freeAndGetValue(ecnPFNDTPName);
							replaceQuote(ecnPFNDTPNameStr);

							AOM_ask_value_string(tpProcessList[ij], "task_result", &ecnPTaskResult);
							printf("\n Process Task Result = %s", ecnPTaskResult);
							//TC_write_syslog("\n Process Task Result Name = %s", ecnPTaskResult);
							string ecnPTaskResultStr = freeAndGetValue(ecnPTaskResult);
							replaceQuote(ecnPTaskResultStr);

							AOM_ask_value_int(tpProcessList[ij], "state", &ecnPState);
							printf("\n Process State = %d", ecnPState);
							//TC_write_syslog("\n Process State = %d, ecnPState);

							AOM_ask_value_string(tpProcessList[ij], "comments", &ecnPComments);
							string ecnPCommentsTemp = freeAndGetValue(ecnPComments);
							findAndReplaceAll(ecnPCommentsTemp, "\n", " ");
							printf("\n Process Comment = %s", ecnPCommentsTemp.c_str());
							//TC_write_syslog("\n Process Comment = %s", ecnPCommentsTemp.c_str());

							replaceQuote(ecnPCommentsTemp);

							AOM_ask_value_string(tpProcessList[ij], "fnd0Priority", &ecnPPriority);
							string ecnPPriorityTemp = freeAndGetValue(ecnPPriority);
							findAndReplaceAll(ecnPPriorityTemp, "\n", " ");
							printf("\n Process Priority = %s", ecnPPriorityTemp.c_str());
							//TC_write_syslog("\n Process Priority = %s", ecnPPriorityTemp.c_str());
							replaceQuote(ecnPPriorityTemp);

							date_t P_fnd0StartDate;
							AOM_ask_value_date(tpProcessList[ij], "fnd0StartDate", &P_fnd0StartDate);
							string ecnPSDate_Formated = printFormattedDate(P_fnd0StartDate);
							printf("\n Process Start Date = %s", ecnPSDate_Formated.c_str());
							//TC_write_syslog("\n Process Start Date = %s", ecnPSDate_Formated.c_str());


							date_t P_fnd0EndDate;
							AOM_ask_value_date(tpProcessList[ij], "fnd0EndDate", &P_fnd0EndDate);
							string ecnPEDate_Formated = printFormattedDate(P_fnd0EndDate);
							printf("\n Process End Date = %s", ecnPEDate_Formated.c_str());
							//TC_write_syslog("\n Process End Date = %s", ecnPEDate_Formated.c_str());

							date_t P_due_date;
							AOM_ask_value_date(tpProcessList[ij], "due_date", &P_due_date);
							string ecnPDueDate_Formated = printFormattedDate(P_due_date);
							printf("\n Process Due Date = %s", ecnPDueDate_Formated.c_str());
							//TC_write_syslog("\n Process Due Date = %s", ecnPDueDate_Formated.c_str());

							AOM_ask_value_string(tpProcessList[ij], "h4_escalation_level", &ecnPEscLevel);
							printf("\n Process Escalation Level = %s", ecnPEscLevel);
							//TC_write_syslog("\n Process Escalation Level = %s", ecnPEscLevel);
							string ecnPEscLevelStr = freeAndGetValue(ecnPEscLevel);
							replaceQuote(ecnPEscLevelStr);

							AOM_ask_value_string(tpProcessList[ij], "the_task_duration", &ecnPTaskDur);
							printf("\n Process Task Duration = %s", ecnPTaskDur);
							//TC_write_syslog("\n Process Task Duration = %s", ecnPTaskDur);
							string ecnPTaskDurStr = freeAndGetValue(ecnPTaskDur);
							replaceQuote(ecnPTaskDurStr);

							date_t P_creation_date;
							AOM_ask_value_date(tpProcessList[ij], "creation_date", &P_creation_date);
							string ecnPCreationDate_Formated = printFormattedDate(P_creation_date);
							printf("\n Process Creation Date = %s", ecnPCreationDate_Formated.c_str());
							//TC_write_syslog("\n Process Creation Date = %s", ecnPCreationDate_Formated.c_str());

							AOM_ask_value_tag(tpProcessList[ij], "owning_user", &powning_user);
							//SA_ask_user_person_name2(powning_user, &ecnPOwningUser);
							AOM_ask_value_string(powning_user, "object_string", &ecnPOwningUser);
							printf("\n Process Owning User = %s", ecnPOwningUser);
							//TC_write_syslog("\n Process Owning User = %s", ecnPOwningUser);
							string ecnPOwningUserStr = freeAndGetValue(ecnPOwningUser);
							replaceQuote(ecnPOwningUserStr);

							AOM_ask_value_string(tpProcessList[ij], "task_type", &pTask_TaskType);
							printf("\n WF Task Type = %s", pTask_TaskType);
							//TC_write_syslog("\n Task Type = %s", epmTask_TaskType);
							string ecnPTask_TypeStr = freeAndGetValue(pTask_TaskType);
							replaceQuote(ecnPTask_TypeStr);

							AOM_ask_value_string(tpProcessList[ij], "resp_party", &pTask_RespPty);
							printf("\n WF Responsible Party = %s", pTask_RespPty);
							//TC_write_syslog("\n Task Responsible Party = %s", epmTask_RespPty);
							string ecnPTask_RespPtyStr = freeAndGetValue(pTask_RespPty);
							replaceQuote(ecnPTask_RespPtyStr);

							AOM_ask_value_tag(tpProcessList[ij], "fnd0Performer", &pttask_performer);
							AOM_ask_value_string(pttask_performer, "object_string", &ptask_performer);
							printf("\n WF Task Performer = %s", ptask_performer);
							//TC_write_syslog("\n Task Performer = %s", epmTask_Performer);
							string ecnPtask_performerStr = freeAndGetValue(ptask_performer);
							replaceQuote(ecnPtask_performerStr);

							AOM_ask_value_tags(tpProcessList[ij], "child_tasks", &iTaskCount, &tTaskList);
							printf("\n Task Count = %d", iTaskCount);
							//TC_write_syslog("\n Task Count = %d", iTaskCount);

							if (iTaskCount > 0) {
								for (int ik = 0; ik < iTaskCount; ik++) {

									int iRevChildTaskCount = 0;
									tag_t* tRevChildTaskList = NULLTAG;

									AOM_ask_value_tags(tTaskList[ik], "child_tasks", &iRevChildTaskCount, &tRevChildTaskList);
									printf("\n tRevChildTaskList Count = %d", iRevChildTaskCount);


									if (iRevChildTaskCount > 0) {
										//Review task
										for (int t = 0; t < iRevChildTaskCount; t++) {

											need_new_csv_file(data);
											writeAttrValueInFile(AttrForFile);
											fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", "", "", "", "");//CMImpliment objects

											printAffectedItemsRow(1, 0, 0, rowValueVectorEmpty);


											//37 +2 +2 -1 variables -21
											fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",",
												gov_classStr.c_str(), ecnProcessJobNameStr.c_str(), ecnPTempNameStr.c_str(), ecnPFNDTPNameStr.c_str(), ecnPTaskResultStr.c_str(), ecnPState, ecnPCommentsTemp.c_str(), ecnPPriorityTemp.c_str(), ecnPSDate_Formated.c_str(), ecnPEDate_Formated.c_str(), ecnPDueDate_Formated.c_str(),
												ecnPEscLevelStr.c_str(), ecnPTaskDurStr.c_str(), ecnPCreationDate_Formated.c_str(), ecnPLastModDate_Formated.c_str(), ecnPOwningUserStr.c_str(), ecnPTask_TypeStr.c_str(), ecnPTask_RespPtyStr.c_str(), ecnPtask_performerStr.c_str());

											getChildTaskDetails(tRevChildTaskList[t]);

											printAuditLogToFile(NULLTAG, 0);
											printf("\n");

										}
										SAFE_MEM_free(tRevChildTaskList);
									}
									else {
										//Normal tasks
										need_new_csv_file(data);
										writeAttrValueInFile(AttrForFile);
										fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", "", "", "", "");//CMImpliment objects

										printAffectedItemsRow(1, 0, 0, rowValueVectorEmpty);


										//37 +2 +2 -1 variables -21
										fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",",
											gov_classStr.c_str(), ecnProcessJobNameStr.c_str(), ecnPTempNameStr.c_str(), ecnPFNDTPNameStr.c_str(), ecnPTaskResultStr.c_str(), ecnPState, ecnPCommentsTemp.c_str(), ecnPPriorityTemp.c_str(), ecnPSDate_Formated.c_str(), ecnPEDate_Formated.c_str(), ecnPDueDate_Formated.c_str(),
											ecnPEscLevelStr.c_str(), ecnPTaskDurStr.c_str(), ecnPCreationDate_Formated.c_str(), ecnPLastModDate_Formated.c_str(), ecnPOwningUserStr.c_str(), ecnPTask_TypeStr.c_str(), ecnPTask_RespPtyStr.c_str(), ecnPtask_performerStr.c_str());

										getChildTaskDetails(tTaskList[ik]);

										printAuditLogToFile(NULLTAG, 0);
										printf("\n");

									}



									//print

								}
								SAFE_MEM_free(tTaskList);
							}
							else {
								need_new_csv_file(data);
								writeAttrValueInFile(AttrForFile);
								fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", "", "", "", "");//CMImpliment objects
								printAffectedItemsRow(1, 0, 0, rowValueVectorEmpty);

								//37+2+2-1 variables
								fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"",
									gov_classStr.c_str(), ecnProcessJobNameStr.c_str(), ecnPTempNameStr.c_str(), ecnPFNDTPNameStr.c_str(), ecnPTaskResultStr.c_str(), ecnPState, ecnPCommentsTemp.c_str(), ecnPPriorityTemp.c_str(), ecnPSDate_Formated.c_str(), ecnPEDate_Formated.c_str(), ecnPDueDate_Formated.c_str(),
									ecnPEscLevelStr.c_str(), ecnPTaskDurStr.c_str(), ecnPCreationDate_Formated.c_str(), ecnPLastModDate_Formated.c_str(), ecnPOwningUserStr.c_str(), ecnPTask_TypeStr.c_str(), ecnPTask_RespPtyStr.c_str(), ecnPtask_performerStr.c_str(), "", "", "", "", "", "", "", "", "",
									"", "", "", "", "", "", "", "", "", "", "", "");

								printAuditLogToFile(NULLTAG, 0);

								printf("\n");
							}

							//Get AuditLog Info
							int logRowsCount = 0;
							tag_t* logRows = NULLTAG;

							int status = executeAuditLogQryJobName(ecnProcessJobNameStr.c_str(), logRowsCount, logRows);

							printf("\n Return value from finction : executeAuditLogQryJobName - \n status:%d, logRowsCount:%d, logRows Tag : %d", status, logRowsCount, logRows);

							if (status == ITK_ok && logRowsCount > 0) {

								for (int j = 0; j < logRowsCount; j++) {

									need_new_csv_file(data);
									writeAttrValueInFile(AttrForFile);

									fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", "", "", "", "");//CMImpliment objects

									printAffectedItemsRow(1, 0, 0, rowValueVectorEmpty);

									//37 +2 +2 -1 variables
									fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"",
										gov_classStr.c_str(), ecnProcessJobNameStr.c_str(), ecnPTempNameStr.c_str(), ecnPFNDTPNameStr.c_str(), ecnPTaskResultStr.c_str(), ecnPState, ecnPCommentsTemp.c_str(), ecnPPriorityTemp.c_str(), ecnPSDate_Formated.c_str(), ecnPEDate_Formated.c_str(), ecnPDueDate_Formated.c_str(),
										ecnPEscLevelStr.c_str(), ecnPTaskDurStr.c_str(), ecnPCreationDate_Formated.c_str(), ecnPLastModDate_Formated.c_str(), ecnPOwningUserStr.c_str(), ecnPTask_TypeStr.c_str(), ecnPTask_RespPtyStr.c_str(), ecnPtask_performerStr.c_str(), "", "", "", "", "", "", "",
										"", "", "", "", "", "", "", "", "", "", "", "", "", "");

									printAuditLogToFile(logRows[j], 1);

								}

							}
							else {

								printf("\n Failed in executing Audit Log Query.\n");
							}



							SAFE_MEM_free(ecnProcessJobName);
							SAFE_MEM_free(ecnPTempName);
							SAFE_MEM_free(ecnPFNDTPName);
							SAFE_MEM_free(ecnPTaskResult);
							SAFE_MEM_free(ecnPComments);
							SAFE_MEM_free(ecnPPriority);
							SAFE_MEM_free(ecnPSDate);
							SAFE_MEM_free(ecnPEDate);
							SAFE_MEM_free(ecnPDueDate);

							SAFE_MEM_free(ecnPEscLevel);
							SAFE_MEM_free(ecnPTaskDur);
							SAFE_MEM_free(ecnPCreationDate);

							SAFE_MEM_free(ecnPOwningUser);
						}
						SAFE_MEM_free(P_attr_value);
						SAFE_MEM_free(ecnPLastModDate);

					}


					if (isAnyTargetWFInitiated != 1) {

						need_new_csv_file(data);
						writeAttrValueInFile(AttrForFile);
						fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", "", "", "", "");//CMImpliment objects
						printAffectedItemsRow(1, 0, 0, rowValueVectorEmpty);
						//37 +2 +2 -1variables
						fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"",
							gov_classStr.c_str(), "", "", "", "", "", "", "", "", "", "", "", "", "",
							"", "", "", "", "", "", "", "", "", "",
							"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");


						printAuditLogToFile(NULLTAG, 0);

					}
					SAFE_MEM_free(tpProcessList);
				}
				else {
					need_new_csv_file(data);
					writeAttrValueInFile(AttrForFile);
					fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", "", "", "", "");//CMImpliment objects
					printAffectedItemsRow(1, 0, 0, rowValueVectorEmpty);
					//37+2 +2-1 variables
					fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"",
						gov_classStr.c_str(), "", "", "", "", "", "", "", "", "", "", "", "", "",
						"", "", "", "", "", "", "", "", "", "",
						"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");


					printAuditLogToFile(NULLTAG, 0);
				}

				printf("\n ---------------------------------------------------------------- \n");
			}
			SAFE_MEM_free(gov_class);
		}
		SAFE_MEM_free(items);
		SAFE_MEM_free(values);
		SAFE_MEM_free(entries);
	}

	printf("\nExit from Get Object Types Funtion...");
	//TC_write_syslog("\nExit from Get Object Types Funtion...");
	return status;
}

std::pair<std::map<int, vector<std::vector<std::string> > >,
	std::map<int, vector<std::vector<std::string> > > > processAttrFile(const std::string& filePath) {

	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filePath << std::endl;
		return {};
	}

	std::string line;
	std::map<int, vector<std::vector<std::string> > > data; // Vector to Attributes Information
	std::map<int, vector<std::vector<std::string> > > relation; // Vectore to Store Relations Information

	while (std::getline(file, line)) {
		int count = countChar(line, '>');
		if (count > 0) {
			std::vector<std::string> rel = splitString(line, '>');
			if (rel.size() > 1) {
				std::string attrInfo = rel.back();
				std::vector<std::string> tokens = splitString(attrInfo, '|');

				data[count].push_back(tokens); // Stores the Attribute Info

				rel.pop_back();
				relation[count].push_back(rel); // Stores the Relation Info
			}
		}
		else {
			std::vector<std::string> dt = splitString(line, '|');
			data[count].push_back(dt);
		}
	}

	file.close();

	return std::make_pair(data, relation);
}

string getPropValue(tag_t objectTag, const char* prop_name, const char* storage_type) {
	char* atrValue = NULL;
	string prop_value;
	if (tc_strcmp("String", storage_type) == 0 || tc_strcmp("LongString", storage_type) == 0) {
		if (tc_strcmp("puid", prop_name) == 0) {
			POM_tag_to_uid(objectTag, &atrValue);
		}
		else {
			//AOM_ask_value_string(objectTag, prop_name, &atrValue);
			AOM_UIF_ask_value(objectTag, prop_name, &atrValue);
		}
	}
	else if (tc_strcmp("Date", storage_type) == 0) {

		date_t attr_date;
		AOM_ask_value_date(objectTag, prop_name, &attr_date);
		string attr_date_formatted = printFormattedDate(attr_date);

		return attr_date_formatted;

	}
	else if (tc_strcmp("Boolean", storage_type) == 0) {
		logical val_logical = 0;
		AOM_ask_value_logical(objectTag, prop_name, &val_logical);
		string value_logical = (val_logical != 0) ? "True" : "False";
		return value_logical;
	}
	else if (tc_strcmp("TypedReference", storage_type) == 0 || tc_strcmp("UntypedReference", storage_type) == 0) {
		tag_t attrTag = NULLTAG;
		AOM_ask_value_tag(objectTag, prop_name, &attrTag);
		if (tc_strcmp("owning_project", prop_name) == 0) {
			PROJ_ask_name2(attrTag, &atrValue);
		}
		else {
			//SA_ask_user_person_name2(attrTag, &atrValue);
			AOM_ask_value_string(attrTag, "object_string", &atrValue);
		}
	}
	else {
		printf("\n Invalid Attribute = %s! Check Input File.\n", prop_name);
		//TC_write_syslog("\n Invalid Attribute = %s! Check Input File.", prop_name);
		return "";
	}
	if (atrValue == NULL || atrValue[0] == '\0') {
		return "";
	}
	prop_value = printAttrValue(atrValue);
	SAFE_MEM_free(atrValue);
	return prop_value;
}

string printAttrValue(char* prop_value) {
	string strAtrVal = prop_value;
	findAndReplaceAll(strAtrVal, "\n", " ");
	replaceQuote(strAtrVal);
	//printf(" : Value = %s", strAtrVal.c_str());
	//TC_write_syslog("\n Attribute Value = %s", strAtrVal.c_str());
	return strAtrVal;
}

//data[1][0...]    : list of properties
								//data[1][ij][0] : property name
								//data[1][ij][1] : property type
void getRelatedItemAttributesValue(string relation, int ObjectCount, tag_t* objectTags, int relatedPropCount, std::vector<std::vector<std::string> > relatedItemAttributes, std::vector<std::vector<std::string> > &relatedItemAttributesValues) {

	//std::vector<std::vector<std::string> > relatedItemAttributes;
	printf("\n\n  Entered into functon : getRelatedItemAttributesValue, relation: %s, ObjectCount: %d, relatedPropCount: %d\n", relation.c_str(), ObjectCount, relatedPropCount);

	for (int i = 0; i < ObjectCount; i++) {
		string ecnItemCMImpl = "";
		vector<std::string> relatedAttributeValues;
		string allAttributeValues = "";
		for (int j = 0; j < relatedPropCount; j++) {
			string temp_CMImpl;
			temp_CMImpl = getPropValue(objectTags[i], relatedItemAttributes[j][0].c_str(), relatedItemAttributes[j][1].c_str());
			relatedAttributeValues.push_back(temp_CMImpl);
			//ecnItemCMImpl = concatenateStrings(ecnItemCMImpl, temp_CMImpl);
			allAttributeValues = allAttributeValues + ", " + temp_CMImpl;

			printf(" > %s", allAttributeValues.c_str());
		}
		relatedItemAttributesValues.push_back(relatedAttributeValues);
		relatedAttributeValues.clear();
		printf("\n relation : %s, all values : %s", relation.c_str(), allAttributeValues.c_str());

	}

	//printf("\n\n Exit from functon : getRelatedItemAttributesValue\n");
}

std::string getCombinedObjectByTag(int ObjectCount, tag_t* objectTags, std::vector<std::string> tokens) {
	//printf("\n Entered into functon : getCombinedObjectByTag");
	string ecnItemCMImpl = "";
	for (int ij = 0; ij < ObjectCount; ij++)
	{
		string temp_CMImpl;
		temp_CMImpl = getPropValue(objectTags[ij], tokens[0].c_str(), tokens[1].c_str());

		ecnItemCMImpl = concatenateStrings(ecnItemCMImpl, temp_CMImpl);
	}
	//printf("\n Exit from functon : getCombinedObjectByTag");
	return ecnItemCMImpl;
}

string concatenateStrings(string str1, string str2) {
	string result;
	if (!str1.empty() && !str2.empty()) {
		result = str1 + ";" + str2;
	}
	else {
		if (!str2.empty()) {
			result = str2;
		}
		else if (!str1.empty()) {
			result = str1;
		}
	}
	// Return the concatenated string
	return result;
}

int countChar(const std::string& str, char ch) {
	return std::count(str.begin(), str.end(), ch);
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::istringstream stream(str);
	std::string token;

	while (std::getline(stream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

// void writeAttrValueInFile(std::vector<std::string> vector) {
// 	//printf("\nEntered into writeAttrValueInFile");
// 	for (auto& token : vector) {
// 		string tmp = token;
// 		fprintf(file, "\"%s\",", tmp.c_str());
// 	}
// 	//printf("\tExit from writeAttrValueInFile");
// }

void writeAttrValueInFile(std::vector<std::string> vector) {
	//printf("\nEntered into writeAttrValueInFile");
	for (std::vector<std::string>::iterator it = vector.begin(); it != vector.end(); ++it) {
		std::string tmp = *it;
		fprintf(file, "\"%s\",", tmp.c_str());
	}
	//printf("\tExit from writeAttrValueInFile");
}

bool isSubstringContains(const string& fullString, const string& subString) {
	return fullString.substr(0, subString.length()) == subString;
}

void findAndReplaceAll(string& data, const string& match, const string& replace) {
	// Get the first occurrence 
	size_t pos = data.find(match);
	// Repeat till end is reached 
	while (pos != string::npos)
	{
		data.replace(pos, match.size(), replace);
		// Get the next occurrence from the current position 
		pos = data.find(match, pos + replace.size());
	}
}

std::string printFormattedDate(const date_t& date) {
	if (!isDateEmpty(date)) {
		struct tm timeinfo = { 0 };
		timeinfo.tm_year = date.year - 1900;
		timeinfo.tm_mon = date.month;
		timeinfo.tm_mday = date.day;
		timeinfo.tm_hour = date.hour;
		timeinfo.tm_min = date.minute;
		timeinfo.tm_sec = date.second;
		//yyyy/m/d h:mm:ss.000
		char buffer[80];
		//strftime(buffer, 80, "%Y/%m/%d %H:%M:%S", &timeinfo);
		strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &timeinfo);
		std::string formattedDate(buffer);
		return formattedDate + ".000";
	}
	else {
		return "";
	}
}

bool isDateEmpty(const date_t& date) {
	return date.year == 0 && date.month == 0 && date.day == 0;
}
