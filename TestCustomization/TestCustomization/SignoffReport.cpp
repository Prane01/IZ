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
#include<tccore/aom.h>
#include<tccore/item.h>
#include<tccore/aom_prop.h>
#include<time.h>
#include<ctime>
#include<tc/tc_util.h>
#include<qry/crf.h>
#include<ae/ae.h>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<cstdlib>
#include<cstring>
#include<tccore/grm.h>
#include<ss/ss_const.h>
#include<pom/pom/pom.h>
#include<qry/qry.h>
#include<set>
using namespace std;
using namespace Teamcenter;

void display() {
	cout << "\n -u\t ---> username";
	cout << "\n -p\t ---> password";
	cout << "\n -g\t ---> group";
	cout << "\n -d\t ---> Initiated after";
}

void unloadObjects(tag_t object) {
	logical is_loaded;
	ITKCALL(POM_is_loaded(object, &is_loaded));
	TC_write_syslog("\nPOM_is_loaded %d", is_loaded);
	cout << "POM_is_loaded " << is_loaded << endl;
	if (is_loaded == TRUE) {
		ITKCALL(AOM_unload(object));
		TC_write_syslog("\nPOM_is_unloaded\n");
		cout << "After: POM_is_unloaded " << endl;
	}
}

int ITK_user_main(int argc, char* argv[])
{
	int ifail = 0;
	ResultStatus status(0);

	try {
		const char* user = ITK_ask_cli_argument("-u=");
		const char* pass = ITK_ask_cli_argument("-p=");
		const char* grp = ITK_ask_cli_argument("-g=");
		const char* creationDate = ITK_ask_cli_argument("-d=");

		if (ITK_ask_cli_argument("-h")) {  // For help command specified in display() function
			display();
			return ifail;
		}

		if ((user && tc_strcmp(user, "") != 0) && (pass && tc_strcmp(pass, "") != 0) && (tc_strcmp(grp, "") != 0)) {
			ITK_init_to_login();
			status = ITK_init_module(user, pass, grp);
			time_t tRawTime;
			struct tm* timeInfo;
			char timeStamp[20];
			time(&tRawTime);
			timeInfo = new struct tm;
			gmtime_s(timeInfo, &tRawTime);
			strftime(timeStamp, sizeof(timeStamp), "%d-%m-%Y %H:%M:%S", timeInfo);
			TC_write_syslog("[%s] User '%s' login successful.", timeStamp, user);

			scoped_smptr<char> report_path_name;
			scoped_smptr<char> ItemStr;
			scoped_smptr<char> ItemRevId;
			scoped_smptr<char> definitionsName;
			tag_t tItemRev = NULLTAG;
			tag_t stylesheet = NULLTAG;
			scoped_smptr<tag_t> definitions;
			int n_defs = 0;
			int revCount = 0;
			scoped_smptr<tag_t> revList;
			tag_t tItem = NULLTAG;
			tag_t tDatasetType = NULLTAG;
			tag_t tRelation_type = NULLTAG;

			date_t createdAfter = NULLDATE;
			status = ITK_string_to_date(creationDate, &createdAfter);
			
			status = AE_find_datasettype2("MSExcel", &tDatasetType);
			TC_write_syslog("\ntDatasetType : %d\n", tDatasetType);
			status = GRM_find_relation_type("IMAN_specification", &tRelation_type);
			TC_write_syslog("\ntRelation_type : %d\n", tRelation_type);

			status = CRF_get_report_definitions("TC_11_51_00_EPM_CUS_AS_ITEM_RPT", NULL, NULL, NULL, NULL, NULL, NULL, &n_defs, &definitions);
			status = AE_find_dataset2("EPMSignoffHistoryReport_excel.xsl", &stylesheet);
			cout << "\nn_defs " << n_defs << endl;

			if (n_defs > 0) {
				status = AOM_ask_value_string(definitions[0], "object_string", &definitionsName);
				TC_write_syslog("\ndefinitionsName : %s\n", definitionsName.getString());
				cout << "definitionsName : " << definitionsName.getString() << endl;

				const char* names[] = { "Process","Output","Method_Name" };

				const char* values[] = { "Method::AWCustomAsItem","Method::AWCustomAsItem", "EPMSignoffHistoryReport" };

				size_t count1 = sizeof(names) / sizeof(names[0]);

				char** actNames = (char**)malloc(count1 * sizeof(char*));

				for (size_t i = 0; i < count1; ++i) {
					size_t len = strlen(names[i]) + 1;
					actNames[i] = (char*)malloc(len);
					strcpy_s(actNames[i], len, names[i]);
				}

				size_t count2 = sizeof(values) / sizeof(values[0]);
				char** actValues = (char**)malloc(count2 * sizeof(char*));

				for (size_t i = 0; i < count2; ++i) {
					size_t len = strlen(values[i]) + 1;
					actValues[i] = (char*)malloc(len);
					strcpy_s(actValues[i], len, values[i]);
				}

				tag_t tQuery = NULLTAG;

				status = QRY_find2("WF - Object In Process", &tQuery); // finding query to get Objects In WF process

				int n_entries = 1;
				scoped_smptr<tag_t> results;
				int foundObj = 0;
				set<tag_t> signoffObjects;
				scoped_smptr<tag_t> stageList;
				int n_stageList = NULL;

				char** entries = (char**)MEM_alloc(sizeof(char*) * 1);
				entries[0] = (char*)MEM_alloc(tc_strlen("Type") + 1);
				tc_strcpy(entries[0], "Type");

				char** valuesOut = (char**)MEM_alloc(sizeof(char*) * 1);
				valuesOut[0] = (char*)MEM_alloc(tc_strlen("Item Revision") + 1);
				tc_strcpy(valuesOut[0], "Item Revision");

				status = QRY_execute(tQuery, n_entries, entries, valuesOut, &foundObj, &results);
				cout << "foundObj : " << foundObj << endl;
				for (int i = 0; i < foundObj; i++) {
					status = AOM_ask_value_tags(results[i], "process_stage_list", &n_stageList, &stageList);
					cout << "process_stage_list : " << n_stageList << endl;
					for (int j = 0; j < n_stageList; j++) {
						date_t initiatedDate = NULLDATE;
						int compareDate = 0;
						status = AOM_ask_value_date(stageList[j], "fnd0JobCreationDate", &initiatedDate);

						scoped_smptr<char> date1;
						status = ITK_date_to_string(createdAfter, &date1);
						cout << "\ncreatedAfter: " << date1.getString() << endl;

						scoped_smptr<char> date;
						status = ITK_date_to_string(initiatedDate, &date);
						cout << "\ninitiatedDate: " << date.getString() << endl;

						status = POM_compare_dates(createdAfter, initiatedDate, &compareDate);

						if (compareDate == -1 || compareDate == 0) {
							signoffObjects.insert(results[i]);
							cout << "insert : " << results[i] << endl;
							break;
						}
					}
				}

				cout << "\nNo.of objects for signoff history: " << signoffObjects.size() << endl;
				for (tag_t tRevision : signoffObjects) {
					status = AOM_ask_value_string(tRevision, "item_id", &ItemStr);
					TC_write_syslog("\nItemStr : %s\n", ItemStr.getString());
					cout << "ItemStr : " << ItemStr.getString() << endl;

					status = AOM_ask_value_string(tRevision, "item_revision_id", &ItemRevId);
					TC_write_syslog("\nItemRevId : %s\n", ItemRevId.getString());
					cout << "ItemRevId : " << ItemRevId.getString() << endl;

					tag_t contextObjs[1] = { tRevision };

					ostringstream oss1;
					oss1 << ItemStr.getString() << "_" << ItemRevId.getString();
					string reportNameStr = oss1.str();

					char* reportName = new char[reportNameStr.length() + 1];  // +1 for null terminator
					strcpy_s(reportName, reportNameStr.length() + 1, reportNameStr.c_str());

					status = CRF_generate_report(definitions[0], stylesheet, 1, contextObjs, 3, actNames, actValues, reportName, &report_path_name);
					cout << "\nreportName : " << reportName << endl;
					delete[] reportName;

					TC_write_syslog("\nReport Path :%s\n", report_path_name.getString());
					cout << "\nReport Path : \n" << report_path_name.getString();

					tag_t tDataset = NULLTAG;
					tag_t tRelation = NULLTAG;

					// Delete existing Report
					//int aCount = 0;
					//tag_t* aSecData = NULL;

					//status = GRM_list_secondary_objects_only(tRevision, tRelation_type, &aCount, &aSecData);
					//for (int i = 0; i < aCount; i++) {
					//	scoped_smptr<char> dName;
					//	tag_t fRelation = NULLTAG;
					//	status = AOM_ask_value_string(aSecData[i], "object_name", &dName);
					//	cout << "dName : " << dName.getString() << endl;
					//	if (tc_strstr(dName.getString(), "Report") != 0) {
					//		cout << "found" << endl;
					//		status = AOM_refresh(tRevision, true);
					//		//status = AOM_delete(aSecData[i]);
					//		status = GRM_find_relation(tRevision, aSecData[i], tRelation_type, &fRelation);
					//		status = GRM_delete_relation(fRelation);
					//		status = AOM_save_without_extensions(tRevision);
					//		status = AOM_refresh(tRevision, false);
					//		cout << "Deleted existing relation of report" << endl;
					//	}
					//}

					ostringstream oss;
					oss << ItemStr.getString() << "_" << ItemRevId.getString() << "_" << timeStamp << "_Report";
					string datasetName = oss.str();

					TC_write_syslog("\nDatasetName : %s\n", datasetName.c_str());
					cout << "DatasetName : " << datasetName.c_str() << endl;

					status = AE_create_dataset_with_id(tDatasetType, datasetName.c_str(), datasetName.c_str(), "12345", "A", &tDataset);
					status = AOM_save_without_extensions(tDataset);

					status = GRM_create_relation(tRevision, tDataset, tRelation_type, NULLTAG, &tRelation);
					status = GRM_save_relation(tRelation);

					status = AOM_refresh(tDataset, true);
					status = AE_import_named_ref(tDataset, "excel", report_path_name.getString(), datasetName.c_str(), SS_BINARY);
					status = AOM_save_without_extensions(tDataset);
					status = AOM_refresh(tDataset, false);

					TC_write_syslog("\nReport attached to Revision\n");
					cout << "\nReport attached to Revision\n";

					unloadObjects(tRevision);
					unloadObjects(tDataset);

				}
			}
		}
		else {
			display();
			return ifail;
		}
	}
	catch (IFail& ex) {
		ifail = ex.ifail();
		scoped_smptr <char> message;
		EMH_ask_error_text(ifail, &message);
		TC_write_syslog("\nThe error is %s", message.getString());
		cout << "\nThe error is " << message.getString();
	}
	return ifail;
}