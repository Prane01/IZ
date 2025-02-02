#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <list>
#include <time.h>
#include <map>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <sstream>
#include <cfm/cfm.h>
#include <bom/bom.h>
#include <ctime>
#include <iomanip>

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

void get_TimeStamp();
void create_csv_file(const char* path, const char* fileName);
int get_type_objects(char* object_type, const char* startDate, const char* endDate);
string concatenateStrings(string str1, string str2);
void findAndReplaceAll(string& data, const string& match, const string& replace);
void bom_sub_child(tag_t *tBomChildren, int iChildCount, std::vector<std::string> ECNAttr,
	std::vector<std::string> solItem, std::string parentBom, std::string asmbly, int temp_bom_level, std::string ParentItemID_temp, std::string ItemUid_temp);

string freeAndGetValue(char* &ptr);
std::string getFormattedDate(const date_t& date);
bool isDateEmpty(const date_t& date);
string replaceQuote(std::string source);
void printProperties(tag_t tBomChildren);
void need_new_csv_file();
bool isSubstringContains(const string& fullString, const string& subString);
void calculateUtilityPerformance(clock_t start);

FILE * file;
const char* csvFilePath = nullptr;
const char* csvFileFirstName = "BOM_ITK_Export";
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
	csvFilePath = ITK_ask_cli_argument("-fileOutPath=");

	printf("\n\n Input Arguments :- \n\t user:%s \n\t startDate:%s \n\t endDate:%s \n\t searchCriteria:%s \n\t outPath:%s", user, startDate, endDate, searchCriteria, csvFilePath);


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
	create_csv_file(csvFilePath, csvFileFirstName);
	//Execute Query create objects
	status = get_type_objects(searchCriteria, startDate, endDate);

	fclose(file);
	// Logout from Teamcenter
	ITK_exit_module(true);

	calculateUtilityPerformance(start);

	return 0;
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
	printf("\n Time taken to run utility: %f min\n", time_taken / 60);
}

std::string replaceQuote(std::string source) {
	size_t pos = 0;
	string from = "\"";
	string to = "";
	while ((pos = source.find("\"", pos)) != std::string::npos) {
		source.replace(pos, from.length(), to);
		pos += to.length(); // Move past the replacement
	}
	return source;
}

void printProperties(tag_t tBomChildren)
{
	printf("\n Printing BOMLine attributes");
	//clock_t printProperties = clock();
	int iNumberOfSubChild = 0;
	int iAttribute = 0;
	int blLevel = 0;
	char* blAttr = NULL;
	date_t date_creation;
	char* ItemRevisionUid = NULL;
	tag_t blRevTag = NULLTAG;
	tag_t* tSubChilds = NULLTAG;

	bool tempElemEffectBool;
	string tempElemEffect = "";
	BOM_line_look_up_attribute("bl_has_occ_effectivity", &iAttribute);
	BOM_line_ask_attribute_logical(tBomChildren, iAttribute, &tempElemEffectBool);
	//printf("\n\t Element Effectivity : %d", tempElemEffectBool);

	if (tempElemEffectBool == true) { tempElemEffect = "true"; }
	else if (tempElemEffectBool == false) { tempElemEffect = "false"; }
	
	int blVariant = 0;
	BOM_line_look_up_attribute("bl_variant_state_int", &iAttribute);
	BOM_line_ask_attribute_int(tBomChildren, iAttribute, &blVariant);
	//printf("\n\t Variant : %d", blVariant);

	BOM_line_look_up_attribute("bl_config_string", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempRuleConfigBy = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Rule Configured By : %s", tempRuleConfigBy.c_str());

	BOM_line_look_up_attribute("bl_item_item_id", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempChildItemId = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t ID : %s", tempChildItemId.c_str());

	BOM_line_look_up_attribute("bl_rev_item_revision_id", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempItemRevID = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Revision : %s", tempItemRevID.c_str());

	BOM_line_look_up_attribute("bl_rev_object_name", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempRevName = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Revision Name : %s", tempRevName.c_str());

	BOM_line_look_up_attribute("bl_rev_object_desc", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempDesc = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Description : %s", tempDesc.c_str());

	BOM_line_look_up_attribute("H4_is_negative", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempNegative = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Is Negative : %s", tempNegative.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_sap_transfer_required", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempSapTransferReq = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t SAP Transfer Required : %s", tempSapTransferReq.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_sap_transfer_status", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempSapTransferStatus = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t SAP Transfer Status : %s", tempSapTransferStatus.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_plant", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempErpPlant = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t ERP Plant : %s", tempErpPlant.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_gold_business_enterprise", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempGBE = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Gold Business Enterprise (GBE) : %s", tempGBE.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_line_of_business", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempLOB = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Line of Business (LOB) : %s", tempLOB.c_str());

	BOM_line_look_up_attribute("bl_H4_Com_Hon_PartRevision_h4_Inventory_UOM", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempInvUOM = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Inventory Unit Of Measure : %s", tempInvUOM.c_str());

	BOM_line_look_up_attribute("bl_H4_Com_Hon_PartRevision_h4_Purchase_Unit_Of_Measure", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempPurUOM = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Purchase Unit of Measure : %s", tempPurUOM.c_str());

	BOM_line_look_up_attribute("bl_rev_h4_mngloc", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempMngloc = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Managing Location : %s", tempMngloc.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_ecad_maturity", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempMaturity = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Maturity : %s", tempMaturity.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_net_weight", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempWeight = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Net Weight : %s", tempWeight.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_part_category", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempPrtCategory = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Part Category : %s", tempPrtCategory.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_accolade_project_tr", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempAccProjId = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Accolade Project ID : %s", tempAccProjId.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_product_hierarchy", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempProdHier = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Product Hierarchy : %s", tempProdHier.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_design_authority", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempDA = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Design Authority : %s", tempDA.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_strategic_business_unit", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempSBU = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Strategic Business Unit (SBU) : %s", tempSBU.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_strategic_business_grp", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempSBG = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Strategic Business Group (SBG) : %s", tempSBG.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_weight_unit", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempWtUnit = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Weight Unit : %s", tempWtUnit.c_str());

	BOM_line_look_up_attribute("H4_Usage", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempUsage = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Usage : %s", tempUsage.c_str());

	BOM_line_look_up_attribute("H4_Packing_List", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempPackList = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Packing List : %s", tempPackList.c_str());

	BOM_line_look_up_attribute("bl_all_notes", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempAllNotes = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t All Notes : %s", tempAllNotes.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_conversion_factor", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempConvFactor = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Conversion factor : %s", tempConvFactor.c_str());


	bool tempHasSubstBool;
	string tempHasSubst = "";
	BOM_line_look_up_attribute("bl_has_substitutes", &iAttribute);
	BOM_line_ask_attribute_logical(tBomChildren, iAttribute, &tempHasSubstBool);
	//printf("\n\t Has Substitutes : %d", tempHasSubstBool);

	if (tempHasSubstBool == true) { tempHasSubst = "true"; }
	else if (tempHasSubstBool == false) { tempHasSubst = "false"; }
	//printf("\n\t Has Substitutes : %s", tempHasSubst.c_str());


	BOM_line_look_up_attribute("bl_substitute", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempSubstitute = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Substitute : %s", tempSubstitute.c_str());


	bool tempHasGlobAlterBool;
	string tempHasGlobAlter = "";
	BOM_line_look_up_attribute("bl_has_global_alternates", &iAttribute);
	BOM_line_ask_attribute_logical(tBomChildren, iAttribute, &tempHasGlobAlterBool);
	//printf("\n\t Has Global Alternates : %d", tempHasGlobAlterBool);

	if (tempHasGlobAlterBool == true) { tempHasGlobAlter = "true"; }
	else if (tempHasGlobAlterBool == false) { tempHasGlobAlter = "false"; }
	//printf("\n\t Has Global Alternates : %s", tempHasGlobAlter.c_str());


	BOM_line_look_up_attribute("bl_item_has_global_alternates", &iAttribute); //bl_item_has_global_alternates
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempItemHasGlobAlter = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Item Has Global Alternates : %s", tempItemHasGlobAlter.c_str());

	BOM_line_look_up_attribute("bl_item_global_alt_list", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempGlobAlterList = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Global Alternate List : %s", tempGlobAlterList.c_str());


	bool tempSuppressedBool;
	string tempSuppressed = "";
	BOM_line_look_up_attribute("bl_is_occ_suppressed", &iAttribute);
	BOM_line_ask_attribute_logical(tBomChildren, iAttribute, &tempSuppressedBool);
	//printf("\n\t Suppressed : %d", tempSuppressedBool);

	if (tempSuppressedBool == true) { tempSuppressed = "true"; }
	else if (tempSuppressedBool == false) { tempSuppressed = "false"; }
	//printf("\n\t Suppressed : %s", tempSuppressed.c_str());


	BOM_line_look_up_attribute("H4_Hon_Notes", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempHonNotes = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Hon Notes : %s", tempHonNotes.c_str());

	BOM_line_look_up_attribute("bl_occ_ref_designator", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempRefDesignator = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Reference Designator : %s", tempRefDesignator.c_str());

	BOM_line_look_up_attribute("bl_sequence_no", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempSequence = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Sequence : %s", tempSequence.c_str());

	BOM_line_look_up_attribute("bl_quantity", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempQuantity = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Quantity : %s", tempQuantity.c_str());

	BOM_line_look_up_attribute("bl_rev_h4_release_status", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempRelStat = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Release Status : %s", tempRelStat.c_str());

	BOM_line_look_up_attribute("bl_rev_date_released", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempDateRelease = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Date Released : %s", tempDateRelease.c_str());

	BOM_line_look_up_attribute("bl_rev_effectivity_text", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempReleaseEffect = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Release Effectivity : %s", tempReleaseEffect.c_str());

	BOM_line_look_up_attribute("bl_rev_owning_user", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempOwner = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Owner : %s", tempOwner.c_str());

	BOM_line_look_up_attribute("bl_rev_owning_group", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempGroupID = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Group ID : %s", tempGroupID.c_str());

	BOM_line_look_up_attribute("bl_rev_last_mod_user", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempLastModUser = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Last Modifying User : %s", tempLastModUser.c_str());

	BOM_line_look_up_attribute("bl_level_starting_0", &iAttribute);
	BOM_line_ask_attribute_int(tBomChildren, iAttribute, &blLevel);
	//printf("\n\t Level : %d", blLevel);

	BOM_line_look_up_attribute("bl_item_is_variant_item", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempIsVarItem = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Is Variant Item : %s", tempIsVarItem.c_str());

	BOM_line_look_up_attribute("bl_used_arrangement_name", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempActArrange = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Active Arrangement : %s", tempActArrange.c_str());

	BOM_line_look_up_attribute("bl_rev_vendor_parts", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempVendorPrt = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Vendor Parts : %s", tempVendorPrt.c_str());

	BOM_line_look_up_attribute("bl_H4_Com_Hon_PartRevision_h4_hec_code", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempHEC = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t HEC : %s", tempHEC.c_str());

	BOM_line_look_up_attribute("bl_H4_Com_Hon_PartRevision_h4_tec_code", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempTEC = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t TEC : %s", tempTEC.c_str());

	BOM_line_look_up_attribute("bl_rev_gov_classification", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempGovCls = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Government Classification : %s", tempGovCls.c_str());

	BOM_line_look_up_attribute("bl_rev_owning_project", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempOwnProj = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Item Revision's Owning Project : %s", tempOwnProj.c_str());

	BOM_line_look_up_attribute("bl_rev_object_type", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempObjType = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Item Revision Type : %s", tempObjType.c_str());

	BOM_line_look_up_attribute("bl_rev_last_mod_date", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempLastModDate = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Last Modified Date : %s", tempLastModDate.c_str());

	BOM_line_look_up_attribute("bl_H4_Com_Hon_PartRevision_h4_type_of_part_object", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempTypePrtObj = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Type of Part Object : %s", tempTypePrtObj.c_str());

	BOM_line_look_up_attribute("bl_H4_Com_Hon_PartRevision_h4_weight", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempH4Weight = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Weight : %s", tempH4Weight.c_str());

	BOM_line_look_up_attribute("bl_H4_Com_Hon_PartRevision_h4_is_ecad_component", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempIsEcadComp = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Is ECAD component? : %s", tempIsEcadComp.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_comment", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempComments = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Comments : %s", tempComments.c_str());

	BOM_line_look_up_attribute("bl_H4_Com_Hon_PartRevision_h4_legacy_part_id", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempLegacyPrtId = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Legacy Part ID : %s", tempLegacyPrtId.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_mrp_alias", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempMRPAlias = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t MRP Alias : %s", tempMRPAlias.c_str());

	BOM_line_look_up_attribute("bl_H4_Com_Hon_PartRevision_h4_material_group", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempMatrialGrp = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Material Group : %s", tempMatrialGrp.c_str());

	BOM_line_look_up_attribute("bl_item_h4_reg_impact", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempRegImpact = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Regulatory Impact : %s", tempRegImpact.c_str());

	BOM_line_look_up_attribute("bl_H4_Abs_Comm_PartRevision_h4_isLibraryPart", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempIsLibPrt = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Is Library Part : %s", tempIsLibPrt.c_str());

	BOM_line_look_up_attribute("bl_part_source", &iAttribute); //bl_part_source //bl_H4_Abs_Comm_PartRevision_h4_part_category
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	string tempMakeBuy = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Make/Buy : %s", tempMakeBuy.c_str());

	BOM_line_look_up_attribute("bl_rev_creation_date", &iAttribute);
	BOM_line_ask_attribute_string(tBomChildren, iAttribute, &blAttr);
	//printf("\n\t Creation Date : %s", blAttr);
	string tempBlCreationDate = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t Bl Creation Date : %s", tempBlCreationDate.c_str());

	int blTraceLink = 0;
	BOM_line_look_up_attribute("fnd0bl_has_trace_link", &iAttribute);
	BOM_line_ask_attribute_int(tBomChildren, iAttribute, &blTraceLink);
	//printf("\n\t Trace Link : %d", blTraceLink);

	int blEndItemAsmState = 0;
	BOM_line_look_up_attribute("fnd0bl_is_mono_override", &iAttribute);
	BOM_line_ask_attribute_int(tBomChildren, iAttribute, &blEndItemAsmState);
	//printf("\n\t End Item Assembly State : %d", blEndItemAsmState);

	int blPackCount = 0;
	BOM_line_look_up_attribute("bl_pack_count", &iAttribute);
	BOM_line_ask_attribute_int(tBomChildren, iAttribute, &blPackCount);
	//printf("\n\t Pack Count : %d", blPackCount);

	AOM_UIF_ask_value(tBomChildren, "bl_substitute_list", &blAttr);
	string tempSubstituteListUif = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t UIF Substitute List : %s", tempSubstituteListUif.c_str());

	AOM_UIF_ask_value(tBomChildren, "bl_uom", &blAttr);
	string tempUomUif = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t UIF Unit Of Measure : %s", tempUomUif.c_str());

	AOM_UIF_ask_value(tBomChildren, "vm1Partners", &blAttr);
	string tempPartnerUif = replaceQuote(freeAndGetValue(blAttr));
	//printf("\n\t UIF Partners : %s", tempPartnerUif.c_str());

	fprintf(file, "\"%s\",\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%d\",\"%d\",", tempElemEffect.c_str(), blVariant, tempRuleConfigBy.c_str(), tempChildItemId.c_str(), tempItemRevID.c_str(), tempRevName.c_str(), tempDesc.c_str(), tempNegative.c_str(), tempSapTransferReq.c_str(), tempSapTransferStatus.c_str(), tempErpPlant.c_str(), tempGBE.c_str(), tempLOB.c_str(), tempInvUOM.c_str(), tempPurUOM.c_str(), tempMngloc.c_str(), tempMaturity.c_str(), tempWeight.c_str(), tempPrtCategory.c_str(), tempAccProjId.c_str(), tempProdHier.c_str(), tempDA.c_str(), tempSBU.c_str(), tempSBG.c_str(), tempWtUnit.c_str(), tempUsage.c_str(), tempPackList.c_str(), tempAllNotes.c_str(), tempConvFactor.c_str(), tempHasSubst.c_str(), tempSubstitute.c_str(), tempHasGlobAlter.c_str(), tempItemHasGlobAlter.c_str(), tempGlobAlterList.c_str(), tempSuppressed.c_str(), tempHonNotes.c_str(), tempRefDesignator.c_str(), tempSequence.c_str(), tempQuantity.c_str(), tempRelStat.c_str(), tempDateRelease.c_str(), tempReleaseEffect.c_str(), tempOwner.c_str(), tempGroupID.c_str(), tempLastModUser.c_str(), tempIsVarItem.c_str(), tempActArrange.c_str(), tempVendorPrt.c_str(), blTraceLink, tempHEC.c_str(), tempTEC.c_str(), tempGovCls.c_str(), tempOwnProj.c_str(), tempObjType.c_str(), tempLastModDate.c_str(), tempTypePrtObj.c_str(), tempH4Weight.c_str(), tempIsEcadComp.c_str(), tempComments.c_str(), tempLegacyPrtId.c_str(), tempMRPAlias.c_str(), tempMatrialGrp.c_str(), tempRegImpact.c_str(), tempIsLibPrt.c_str(), tempMakeBuy.c_str(), tempBlCreationDate.c_str(), tempSubstituteListUif.c_str(), tempUomUif.c_str(), tempPartnerUif.c_str(), blEndItemAsmState, blPackCount);

	printf("\t ...., Attributes printed\n");
}

int get_type_objects(char* searchCriteria, const char* startDate, const char* endDate) {
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

	const char* object_type = "HON Part Revision;Finished Good Revision";////Engineering Change Notice Revision, Engineering Work Request Revision, Obsolescence Notice Revision,
	//HON Part Revision;Finished Good Revision

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
	int n_values = 0;

	status = QRY_execute(query, n_entries, entries, values, &n_items, &items);
	printf("\n Query Execute Status : %d, n_items :%d\n", status, n_items);

	if (status == ITK_ok) {




		for (int ii = 0; ii < n_items; ii++)
		{

			char* hpnObjectType = NULL;
			char* gov_class = NULL;

			//int secCount = 0;

			std::vector<std::string> ECNAttr;  //ECNAttr == Hon part rev or Finished good rev attr


			//AOM_ask_value_string(items[ii], "object_type", &hpnObjectType);
			WSOM_ask_object_type2(items[ii], &hpnObjectType);


			string hpnObjectTypeStr = freeAndGetValue(hpnObjectType);
			//printf("\n Str HPN Object Type = %s ", hpnObjectTypeStr.c_str());

			AOM_ask_value_string(items[ii], "gov_classification", &gov_class);
			//printf("\n Gov Classificaion = %s", gov_class);
			string gov_classStr = freeAndGetValue(gov_class);
			//printf("\n Str Gov Classificaion = %s", gov_classStr.c_str());

			if (isSubstringContains(hpnObjectTypeStr, "H4_OrderSpecRevision") == 0 || isSubstringContains(hpnObjectTypeStr, "H4_Com_Hon_PartRevision") == 0) {

				//if (isSubstringContains(gov_classStr, "ESO1.0") == 0 || isSubstringContains(gov_classStr, "") == 0) {
				if (tc_strcmp(gov_classStr.c_str(), "ESO1.0") == 0 || tc_strcmp(gov_classStr.c_str(), NULL) == 0 || tc_strcmp(gov_classStr.c_str(), "") == 0)
				{

					//printf("\n inside if");

					char* hpnPartCategory = NULL;

					AOM_ask_value_string(items[ii], "h4_part_category", &hpnPartCategory);

					string hpnPartCategoryTemp = freeAndGetValue(hpnPartCategory);

					if (tc_strcmp(hpnPartCategoryTemp.c_str(), "Make Finished Good") == 0 || tc_strcmp(hpnPartCategoryTemp.c_str(), "Buy Finished Good") == 0) {

						printf("\nObject Type=%s , Gov Classificaion =%s , SAP Material Type = %s", hpnObjectTypeStr.c_str(), gov_classStr.c_str(), hpnPartCategoryTemp.c_str());

						std::vector<std::string> solItem; // solItem = Hon part rev or finished goof rev attr

						char* ParentItemID = NULL;

						char* ParentRevisionId = NULL;

						char* ItemUid = NULL;

						tag_t item_tag = NULLTAG;
						tag_t relationTag = NULLTAG;
						tag_t* tBOMLineTags = NULLTAG;
						tag_t revRuleTag = NULLTAG;
						tag_t tBOMWindow = NULLTAG;
						tag_t tBOMLine = NULLTAG;
						int nBomLineCount = 0;


						// BOM-SKU No(s) object string readitem_id DUE TO CHANGE

						//AOM_ask_value_string(items[ii], "object_string", &hpnObjectString);
						AOM_ask_value_string(items[ii], "item_id", &ParentItemID);
						string ParentItemID_temp = replaceQuote(freeAndGetValue(ParentItemID));
						printf("\n ParentItemID = %s", ParentItemID_temp.c_str());

						AOM_ask_value_string(items[ii], "item_revision_id", &ParentRevisionId);
						string ParentRevisionId_temp = freeAndGetValue(ParentRevisionId);
						printf("\n ParentRevisionId = %s", ParentRevisionId_temp.c_str());

						ITEM_ask_item_of_rev(items[ii], &item_tag);
						ITK__convert_tag_to_uid(item_tag, &ItemUid);
						string ItemUid_temp = freeAndGetValue(ItemUid);
						printf("\n ItemUid = %s", ItemUid_temp.c_str());

						solItem.push_back(hpnPartCategoryTemp);
						solItem.push_back(ParentItemID_temp);
						solItem.push_back(ParentRevisionId_temp);

						//int solItem_size = solItem.size();
						int iFail = 0;

						//For setting revision rule -
						iFail = BOM_create_window(&tBOMWindow);
						printf("\nBOM Window Created is %d\n", iFail);

						//For getting revision rule tag -Working and Released(Precise and Imprecise)
						iFail = CFM_find("HON Engineering", &revRuleTag); // HON Engineering // Working and Released(Precise and Imprecise)
						printf("\nRevision Rule Tag is %d and iFail is %d\n", revRuleTag, iFail);

						iFail = BOM_set_window_config_rule(tBOMWindow, revRuleTag);
						printf("\nBOM Revision Rule Configured tag is %d, iFail is %d\n", revRuleTag, iFail);

						iFail = BOM_set_window_top_line(tBOMWindow, NULLTAG, items[ii], NULLTAG, &tBOMLine);
						printf("\nBOM Line tag is %d and iFail is %d\n", tBOMLine, iFail);

						iFail = BOM_line_ask_all_child_lines(tBOMLine, &nBomLineCount, &tBOMLineTags);
						printf("\nBOM Line count is %d\n", nBomLineCount);

						if (nBomLineCount > 0)
						{
							int bom_level = 0;
							std::string asmbly = "A";

							need_new_csv_file();

							fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",", ParentItemID_temp.c_str(), "", asmbly.c_str(), "");
							printProperties(tBOMLine);
							fprintf(file, "%s\n", ItemUid_temp.c_str());
							bom_sub_child(tBOMLineTags, nBomLineCount, ECNAttr, solItem, solItem[solItem.size() - 2], asmbly, bom_level, ParentItemID_temp, ItemUid_temp);

							SAFE_MEM_free(tBOMLineTags);
						}

						BOM_close_window(tBOMWindow);
						solItem.clear();

					}
				}

			}

		}
		SAFE_MEM_free(items);
		SAFE_MEM_free(values);
		SAFE_MEM_free(entries);
	}

	printf("\nExit from Get Object Types Funtion...");
	//TC_write_syslog("\nExit from Get Object Types Funtion...");
	return status;
}

void get_TimeStamp() {
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = gmtime(&rawtime);
	strftime(timeStamp, sizeof(timeStamp), "%d_%m_%Y_%H_%M_%S", timeinfo);
	//printf("\n Current Time Stamp = %s", timeStamp);

}

void need_new_csv_file() {
	rowsTotal = rowsTotal + 1;
	if (file_row_count > 100000) {
		file_row_count = 3;
		fclose(file);
		file_number_count += 1;
		create_csv_file(csvFilePath, csvFileFirstName);
	}
	else {
		file_row_count += 1;
	}
}

void create_csv_file(const char* path, const char* fileName) {
	char filepathlocation[1000];

	filepathlocation[0] = '\0';
	tc_strcpy(filepathlocation, path);
	tc_strcat(filepathlocation, "/");
	tc_strcat(filepathlocation, fileName);
	tc_strcat(filepathlocation, "_");
	tc_strcat(filepathlocation, timeStamp);
	tc_strcat(filepathlocation, "_");
	tc_strcat(filepathlocation, std::to_string(file_number_count).c_str());
	tc_strcat(filepathlocation, ".csv");
	printf("\n filepathlocation path as %s", filepathlocation);
	//TC_write_syslog("\n filepathlocation path as %s", filepathlocation);

	file = fopen(filepathlocation, "w");

	// Write UTF-8 BOM to ensure the file is recognized as UTF-8
	//unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
	//fwrite(bom, sizeof(bom), 1, file);

	fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", "Parent BOM ID", "Child BOM ID", "Assembly(A)/Component(C)", "Level", "Element Effectivity", "Variant", "Rule Configured By", "ID", "Revision", "Revision Name", "Description", "Is Negative", "SAP Transfer Required", "SAP Transfer Status", "ERP Plant", "Gold Business Enterprise(GBE)", "Line of Business(LOB)", "Inventory Unit Of Measure", "Purchase Unit of Measure", "Managing Location", "Maturity", "Net Weight", "Part Category", "Accolade Project ID", "Product Hierarchy", "Design Authority", "Strategic Business Unit(SBU)", "Strategic Business Group(SBG)", "Weight Unit", "Usage", "Packing List", "All Notes", "Conversion factor", "Has Substitutes", "Substitute", "Has Global Alternates", "Item Has Global Alternates", "Global Alternate List", "Suppressed", "Hon Notes", "Reference Designator", "Sequence", "Quantity", "Release Status", "Date Released", "Release Effectivity", "Owner", "Group ID", "Last Modifying User", "Is Variant Item", "Active Arrangement", "Vendor Parts", "Trace Link", "HEC", "TEC", "Government Classification", "Item Revision's Owning Project", "Item Revision Type", "Last Modified Date", "Type of Part Object", "Weight", "Is ECAD component?", "Comments", "Legacy Part ID", "MRP Alias", "Material Group", "Regulatory Impact", "Is Library Part", "Make/Buy", "Creation_Date", "Substitute List", "Unit Of Measure", "Partners", "End Item Assembly State", "Pack Count", "TCUID");


	//"Element Effectivity", "End Item Assembly State", "Rule Configured By", "Suppressed",  "Has Substitutes", "Substitute List", "Has Global Alternates", "Unit Of Measure", "Release Effectivity", "Level", "Pack Count", "Partners", "Trace Link", "TCUID"

}

string concatenateStrings(string str1, string str2) {
	string result;
	if (!str1.empty() && !str2.empty()) {
		result = str1 + ":" + str2;
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

void bom_sub_child(tag_t *tBomChildren, int iChildCount, std::vector<std::string> ECNAttr, std::vector<std::string> solItem, std::string parentBom, std::string asmbly, int temp_bom_level, std::string ParentItemID_temp, std::string ItemUid_temp)
{
	printf("\n\n\n\t\t Sub-children count : %d", iChildCount);
	for (int i = 0; i < iChildCount; i++)
	{
		char* ItemRevisionUid = NULL;
		tag_t blRevTag = NULLTAG;
		tag_t* tSubChilds = NULLTAG;
		int iNumberOfSubChild = 0;
		int iAttribute = 0;
		char* blAttr = NULL;
		int bom_level = temp_bom_level + 1;

		BOM_line_ask_all_child_lines(tBomChildren[i], &iNumberOfSubChild, &tSubChilds);
		//printf("\n After BOM_line_ask_all_child_lines and NumberOfSubChild = %d", iNumberOfSubChild);

		ITK__convert_tag_to_uid(tBomChildren[i], &ItemRevisionUid);
		//printf(" \n ItemRevisionUid : %s", ItemRevisionUid);

		BOM_line_look_up_attribute("bl_item_item_id", &iAttribute);
		BOM_line_ask_attribute_string(tBomChildren[i], iAttribute, &blAttr);
		string tempChildItemId = replaceQuote(freeAndGetValue(blAttr));
		//printf("\n\t Item Id : %s", tempChildItemId.c_str());

		BOM_line_look_up_attribute("bl_occurrence_uid", &iAttribute);
		BOM_line_ask_attribute_string(tBomChildren[i], iAttribute, &blAttr);
		string tempUidOcc = replaceQuote(freeAndGetValue(blAttr));
		//printf("\n\t UID of the occurrence thread : %s", tempUidOcc.c_str());

		string tcuid = concatenateStrings(ItemUid_temp, tempUidOcc);
		//printf("\n\t TCUID : %s", tcuid.c_str());

		need_new_csv_file();

		if (iNumberOfSubChild > 0)
		{
			//printf("\n iNumberOfSubChild are greater than zero");
			asmbly = "A";
			fprintf(file, "\"%s\",\"%s\",\"%s\",\"%d\",", ParentItemID_temp.c_str(), tempChildItemId.c_str(), asmbly.c_str(), bom_level);
			printProperties(tBomChildren[i]);
			fprintf(file, "%s\n", tcuid.c_str());
			bom_sub_child(tSubChilds, iNumberOfSubChild, ECNAttr, solItem, parentBom, asmbly, bom_level, tempChildItemId, ItemUid_temp);

		}
		else
		{
			//printf("\n No child found");
			asmbly = "C";
			fprintf(file, "\"%s\",\"%s\",\"%s\",\"%d\",", ParentItemID_temp.c_str(), tempChildItemId.c_str(), asmbly.c_str(), bom_level);
			printProperties(tBomChildren[i]);
			fprintf(file, "%s\n", tcuid.c_str());
		}
	}
}

string freeAndGetValue(char* &ptr) {
	string temp = "";
	if (ptr && strlen(ptr) > 0) {
		temp = ptr;
		SAFE_MEM_free(ptr);

	}
	return temp;
}

std::string getFormattedDate(const date_t& date) {
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

bool isSubstringContains(const string& fullString, const string& subString) {
	return fullString.substr(0, subString.length()) == subString;
}