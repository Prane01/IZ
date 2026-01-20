package com.teamcenter.soa.EDW.AMPL.Util;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;
import java.util.stream.Collectors;
import java.time.format.DateTimeParseException;

import com.teamcenter.clientx.AppXSession;

import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsData2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsOutput2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsPref2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsResponse2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationship;
import com.teamcenter.soa.EDW.AMPL.EDWDataMgmt;
import com.teamcenter.soa.EDW.AMPL.model.AMPLData;
import com.teamcenter.soa.EDW.AMPL.model.HPNData;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.BOMLine;
import com.teamcenter.soa.client.model.strong.BOMWindow;
import com.teamcenter.soa.client.model.strong.ImanRelation;
import com.teamcenter.soa.client.model.strong.ItemRevision;
import com.teamcenter.soa.client.model.strong.RevisionRule;
import com.teamcenter.soa.exceptions.NotLoadedException;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Locale;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.internal.strong.cad._2017_05.StructureManagement;
import com.teamcenter.services.strong.cad.StructureManagementService;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement.CloseBOMWindowsResponse;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement.CreateBOMWindowsResponse;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement.GetRevisionRulesResponse;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement.RevisionRuleConfigInfo;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement.RevisionRuleInfo;
import com.teamcenter.services.strong.cad._2019_06.StructureManagement.CreateWindowsInfo3;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.BOMLine;
import com.teamcenter.soa.client.model.strong.BOMWindow;
import com.teamcenter.soa.client.model.strong.ItemRevision;
import com.teamcenter.soa.client.model.strong.Property;
import com.teamcenter.soa.client.model.strong.RevisionRule;

import com.teamcenter.services.strong.administration.PreferenceManagementService;
import com.teamcenter.services.strong.administration._2012_09.PreferenceManagement.CompletePreference;
import com.teamcenter.services.strong.administration._2012_09.PreferenceManagement.GetPreferencesResponse;
import com.teamcenter.services.strong.administration._2012_09.PreferenceManagement.PreferenceValue;

@SuppressWarnings("unused")
public class ProcessUtil {

	public static StringBuilder eDWDataBuilder = null;
	public static StringBuilder EDWLogBuilder = null;
	public  String StrCSVDirPath = "";
	public  String StrLogDirPath = "";
	public  String StrFileTimeStamp = "";

	public DataManagementService EDWDataManagementSevice = EDWDataMgmt.EDWDataManagementSevice;

	
	public DateTimeFormatter inputFormatter = DateTimeFormatter.ofPattern("dd-MMM-yyyy HH:mm:ss");
	public DateTimeFormatter outputFormatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss.SSS");
	public String date = "";
	
	FileWriter EDWDataWriter = null;

	int isECN_AMPL_MPN = 0;

	public void LoadAndPrintProperties(ArrayList<ModelObject> revObjects, String StrCSVDirPath, String StrLogDirPath,
			String StrFileTimeStamp) {

		this.StrCSVDirPath = StrCSVDirPath;
		this.StrLogDirPath = StrLogDirPath;
		this.StrFileTimeStamp = StrFileTimeStamp;

		EDWDataMgmt.EDWLogBuilder
				.append(EDWMessageUtil.LogMessage(("INFO"), "Loading properties for the objects...\n"));
		try {

			AMPLData amplDataObj = new AMPLData();

			if (revObjects.size() > 0) {
				int i = 0;
				int ilines = 0;
				int fileCount = 1;
				
				EDWDataManagementSevice.getProperties(revObjects.toArray(new ModelObject[] {}),
						new String[] { "puid", "gov_classification", "object_type", "item_revision_id", "object_name",
								"object_desc", "h4_strategic_business_grp", "h4_strategic_business_unit",
								"h4_gold_business_enterprise", "h4_line_of_business", "h4_status_rating", "h4_mpn_type",
								"puid", "item_id", "h4_vendor_name", "project_list", "h4_design_authority",
								"h4_vendor_id", "h4_comment", "owning_project" });

				for (ModelObject RevObject : revObjects) {
					i++;
					System.out.println("The count of object " + i);

					eDWDataBuilder = new StringBuilder();
					
					String StrGovClsfn = "";
					String StrObjtype = "";

					StrGovClsfn = RevObject.getPropertyDisplayableValue("gov_classification");
					StrObjtype = RevObject.getPropertyDisplayableValue("object_type");
					String id=RevObject.getPropertyDisplayableValue("item_id");
					
					System.out.println("The item id "+id);
					
					isECN_AMPL_MPN = 0;
					if (StrObjtype.equalsIgnoreCase("Manufacturer Part Revision")
							&& (StrGovClsfn.equalsIgnoreCase("ESO1.0") || StrGovClsfn == null
									|| StrGovClsfn.equalsIgnoreCase(""))) {

						amplDataObj.setMpnRevObjectType(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("object_type")));
						amplDataObj.setMpnRevItemRevisionId(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("item_revision_id")));
						amplDataObj.setMpnRevObjectName(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("object_name")));
						amplDataObj.setMpnRevObjectDesc(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("object_desc")));
						amplDataObj.setMpnRevH4StrategicBusinessGrp(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("h4_strategic_business_grp")));
						amplDataObj.setMpnRevH4StrategicBusinessUnit(escapeDoubleQuotes(
								RevObject.getPropertyDisplayableValue("h4_strategic_business_unit")));
						amplDataObj.setMpnRevH4GoldBusinessEnterprise(escapeDoubleQuotes(
								RevObject.getPropertyDisplayableValue("h4_gold_business_enterprise")));
						amplDataObj.setMpnRevH4LineOfBusiness(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("h4_line_of_business")));
						amplDataObj.setMpnRevH4StatusRating(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("h4_status_rating")));
						amplDataObj.setMpnRevH4MpnType(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("h4_mpn_type")));
						amplDataObj.setMpnRevItemId(escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("item_id")));
						amplDataObj.setMpnRevH4VendorName(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("h4_vendor_name")));

						List<String> projects = RevObject.getPropertyObject("project_list").getDisplayableValues();
//						String projectsList = String.join(",", projects);

						amplDataObj.setMpnRevProjectsList(projects.stream()
			                    .filter(s -> s != null && !s.isEmpty())
			                    .collect(Collectors.joining(",")));
						amplDataObj.setMpnRevH4DesignAuthority(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("h4_design_authority")));
						amplDataObj.setMpnRevH4VendorId(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("h4_vendor_id")));
						amplDataObj.setMpnRevH4Comment(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("h4_comment")));
						amplDataObj.setMpnRevOwningProject(
								escapeDoubleQuotes(RevObject.getPropertyDisplayableValue("owning_project")));
						amplDataObj.setMpnRevUid(escapeDoubleQuotes(RevObject.getUid()));

						// MPN Rev Details
						eDWDataBuilder.append("\"").append(amplDataObj.getMpnRevObjectType()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevItemRevisionId()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevObjectName()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevObjectDesc()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevH4StrategicBusinessGrp()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevH4StrategicBusinessUnit()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevH4GoldBusinessEnterprise()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevH4LineOfBusiness()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevH4StatusRating()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevH4MpnType()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevItemId()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevH4VendorName()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevProjectsList()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevH4DesignAuthority()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevH4VendorId()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevH4Comment()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevOwningProject()).append("\",").append("\"")
								.append(amplDataObj.getMpnRevUid()).append("\",");

						ModelObject mpnItem = RevObject.getPropertyObject("items_tag").getModelObjectValue();

						if (mpnItem != null) {
							EDWDataManagementSevice.getProperties(new ModelObject[] { mpnItem },
									new String[] { "commercialparts", "H4_AMPLItems", "h4_mfg_part_maturity",
											"owning_user", "owning_group", "last_mod_user", "last_mod_date",
											"h4_reg_impact", "puid", "vendor_name", "vendor_id", "vendor_locations",
											"h4_mpn_ppl_rating", "h4_mpn_status" });

							amplDataObj.setMpnH4MfgPartMaturity(
									escapeDoubleQuotes(mpnItem.getPropertyDisplayableValue("h4_mfg_part_maturity")));
							amplDataObj.setMpnOwningUser(
									escapeDoubleQuotes(mpnItem.getPropertyDisplayableValue("owning_user")));
							amplDataObj.setMpnOwningGroup(
									escapeDoubleQuotes(mpnItem.getPropertyDisplayableValue("owning_group")));
							amplDataObj.setMpnLastModUser(
									escapeDoubleQuotes(mpnItem.getPropertyDisplayableValue("last_mod_user")));

							date = mpnItem.getPropertyDisplayableValue("last_mod_date");
							if (date != null && !date.isEmpty()) {
								LocalDateTime localDateTime = LocalDateTime.parse(date, inputFormatter);
								date = localDateTime.format(outputFormatter);
							}
							amplDataObj.setMpnLastModDate(date);
							amplDataObj.setMpnH4RegImpact(
									escapeDoubleQuotes(mpnItem.getPropertyDisplayableValue("h4_reg_impact")));
							amplDataObj.setMpnVendorName(
									escapeDoubleQuotes(mpnItem.getPropertyDisplayableValue("vendor_name")));
							amplDataObj.setMpnVendorId(
									escapeDoubleQuotes(mpnItem.getPropertyDisplayableValue("vendor_id")));

							List<String> vendorLocations = mpnItem.getPropertyObject("vendor_locations")
									.getDisplayableValues();
//							String vendorLocationsList = String.join(",", vendorLocations);

							amplDataObj.setMpnVm0Location(vendorLocations.stream()
				                    .filter(s -> s != null && !s.isEmpty())
				                    .collect(Collectors.joining(",")));

							amplDataObj.setMpnH4MpnPplRating(
									escapeDoubleQuotes(mpnItem.getPropertyDisplayableValue("h4_mpn_ppl_rating")));
							amplDataObj.setMpnH4MpnStatus(
									escapeDoubleQuotes(mpnItem.getPropertyDisplayableValue("h4_mpn_status")));
							amplDataObj.setMpnUid(escapeDoubleQuotes(mpnItem.getUid()));

							// MPN Details
							eDWDataBuilder.append("\"").append(amplDataObj.getMpnH4MfgPartMaturity()).append("\",")
									.append("\"").append(amplDataObj.getMpnOwningUser()).append("\",").append("\"")
									.append(amplDataObj.getMpnOwningGroup()).append("\",").append("\"")
									.append(amplDataObj.getMpnLastModUser()).append("\",").append("\"")
									.append(amplDataObj.getMpnLastModDate()).append("\",").append("\"")
									.append(amplDataObj.getMpnH4RegImpact()).append("\",").append("\"")
									.append(amplDataObj.getMpnVendorName()).append("\",").append("\"")
									.append(amplDataObj.getMpnVendorId()).append("\",").append("\"")
									.append(amplDataObj.getMpnVm0Location()).append("\",").append("\"")
									.append(amplDataObj.getMpnH4MpnPplRating()).append("\",").append("\"")
									.append(amplDataObj.getMpnH4MpnStatus()).append("\",").append("\"")
									.append(amplDataObj.getMpnUid()).append("\",");
						
							List<ModelObject> amplItems = getPrimaryObjects(mpnItem);

							if (amplItems != null && amplItems.size() > 0) {
								EDWDataManagementSevice.getProperties(amplItems.toArray(new ModelObject[] {}),
										new String[] { "item_id", "object_type", "CMMaturity" });
								int couter = 0;
								String amplItemID = "";
								String amplCMMaturity = "";
								String actItemID = "";
								String actStatusID = "";
								List<String> ecnNoList = new ArrayList<>();
								List<String> ecnStatusList = new ArrayList<>();
								for (ModelObject amplItem : amplItems) {
									isECN_AMPL_MPN = 1;

									amplItemID = amplItem.getPropertyDisplayableValue("item_id");

									amplCMMaturity = amplItem.getPropertyDisplayableValue("CMMaturity");
									ecnNoList.add(amplItemID);
									ecnStatusList.add(amplCMMaturity);

									couter++;
								}
//								actItemID = String.join(",", ecnNoList);
//								actStatusID = String.join(",", ecnStatusList);

								amplDataObj.setMpnEcnReleaseNo(ecnNoList.stream()
					                    .filter(s -> s != null && !s.isEmpty())
					                    .collect(Collectors.joining(",")));
								amplDataObj.setMpnEcnStatus(ecnStatusList.stream()
					                    .filter(s -> s != null && !s.isEmpty())
					                    .collect(Collectors.joining(",")));
								
								eDWDataBuilder.append("\"").append(amplDataObj.getMpnEcnReleaseNo()).append("\",")
										.append("\"").append(amplDataObj.getMpnEcnStatus()).append("\",");
							} else {
								amplDataObj.setMpnEcnReleaseNo("");
								amplDataObj.setMpnEcnStatus("");

								eDWDataBuilder.append("\"").append("").append("\",").append("\"").append("")
										.append("\",");
							}

							ModelObject[] comParts = mpnItem.getPropertyObject("commercialparts")
									.getModelObjectArrayValue();
							if (comParts.length > 0) {

								EDWDataManagementSevice.getProperties(comParts, new String[] {});
								int flag = 0;

								for (ModelObject comPart : comParts) {
									if (flag != 0) {

										eDWDataBuilder.append("\"").append(amplDataObj.getMpnRevObjectType()).append("\",")
												.append("\"").append(amplDataObj.getMpnRevItemRevisionId()).append("\",")
												.append("\"").append(amplDataObj.getMpnRevObjectName()).append("\",")
												.append("\"").append(amplDataObj.getMpnRevObjectDesc()).append("\",")
												.append("\"").append(amplDataObj.getMpnRevH4StrategicBusinessGrp())
												.append("\",").append("\"")
												.append(amplDataObj.getMpnRevH4StrategicBusinessUnit()).append("\",")
												.append("\"").append(amplDataObj.getMpnRevH4GoldBusinessEnterprise())
												.append("\",").append("\"").append(amplDataObj.getMpnRevH4LineOfBusiness())
												.append("\",").append("\"").append(amplDataObj.getMpnRevH4StatusRating())
												.append("\",").append("\"").append(amplDataObj.getMpnRevH4MpnType())
												.append("\",").append("\"").append(amplDataObj.getMpnRevItemId())
												.append("\",").append("\"").append(amplDataObj.getMpnRevH4VendorName())
												.append("\",").append("\"").append(amplDataObj.getMpnRevProjectsList())
												.append("\",").append("\"")
												.append(amplDataObj.getMpnRevH4DesignAuthority()).append("\",")
												.append("\"").append(amplDataObj.getMpnRevH4VendorId()).append("\",")
												.append("\"").append(amplDataObj.getMpnRevH4Comment()).append("\",")
												.append("\"").append(amplDataObj.getMpnRevOwningProject()).append("\",")
												.append("\"").append(amplDataObj.getMpnRevUid()).append("\",");
										eDWDataBuilder.append("\"").append(amplDataObj.getMpnH4MfgPartMaturity())
												.append("\",").append("\"").append(amplDataObj.getMpnOwningUser())
												.append("\",").append("\"").append(amplDataObj.getMpnOwningGroup())
												.append("\",").append("\"").append(amplDataObj.getMpnLastModUser())
												.append("\",").append("\"").append(amplDataObj.getMpnLastModDate())
												.append("\",").append("\"").append(amplDataObj.getMpnH4RegImpact())
												.append("\",").append("\"").append(amplDataObj.getMpnVendorName())
												.append("\",").append("\"").append(amplDataObj.getMpnVendorId())
												.append("\",").append("\"").append(amplDataObj.getMpnVm0Location())
												.append("\",").append("\"").append(amplDataObj.getMpnH4MpnPplRating())
												.append("\",").append("\"").append(amplDataObj.getMpnH4MpnStatus())
												.append("\",").append("\"").append(amplDataObj.getMpnUid() + "\",");
										eDWDataBuilder.append("\"").append(amplDataObj.getMpnEcnReleaseNo()).append("\",")
												.append("\"").append(amplDataObj.getMpnEcnStatus()).append("\",");

									}

									getHPNItemRevisionDetails(comPart, eDWDataBuilder);
									flag++;
								}
							} else {
								// eDWDataBuilder.append("\n"); 44 HPN props 2 ecnReleaseno and status 1 link
								// file 47 total

								eDWDataBuilder.append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\",").append("\"").append("")
										.append("\",").append("\"").append("").append("\"\n");
							}

						}
					}
					// Print Logic
					try {
						EDWDataMgmt.EDWLogBuilder.append(
								EDWMessageUtil.LogMessage("INFO", "EDW CSV file writing process started ...\n"));
						
						if (!eDWDataBuilder.toString().isEmpty()) {
							String[] lines = eDWDataBuilder.toString().split("\n");

							String StrCSVFilePath = "";

							if (EDWDataWriter == null) {

								StrCSVFilePath = StrCSVDirPath + File.separator + "AMPL_ITK_Export_" + StrFileTimeStamp
										+ "_" + fileCount + ".csv";
								EDWDataWriter = new FileWriter(StrCSVFilePath, StandardCharsets.UTF_8);
								EDWDataWriter.write("\"MPNRev_object_type\",\"MPNRev_item_revision_id\",\"MPNRev_object_name\",\"MPNRev_object_desc\",\"MPNRev_h4_strategic_business_grp\",\"MPNRev_h4_strategic_business_unit\",\"MPNRev_h4_gold_business_enterprise\",\"MPNRev_h4_line_of_business\",\"MPNRev_h4_status_rating\",\"MPNRev_h4_mpn_type\",\"MPNRev_item_id\",\"MPNRev_h4_vendor_name\",\"MPNRev_projects_list\",\"MPNRev_h4_design_authority\",\"MPNRev_h4_vendor_id\",\"MPNRev_h4_comment\",\"MPNRev_owning_project\",\"MPNRev_uid\",\"MPN_h4_mfg_part_maturity\",\"MPN_owning_user\",\"MPN_owning_group\",\"MPN_last_mod_user\",\"MPN_last_mod_date\",\"MPN_h4_reg_impact\",\"MPN_vendor_name\",\"MPN_vendor_id\",\"MPN_vm0location\",\"MPN_h4_mpn_ppl_rating\",\"MPN_h4_mpn_status\",\"MPN_uid\",\"MPN_ECN_Release_No\",\"MPN_ECN_Status\",\"HPNRev_item_revision_id\",\"HPNRev_object_name\",\"HPNRev_object_type\",\"HPNRev_object_desc\",\"HPNRev_creation_date\",\"HPNRev_owning_user\",\"HPNRev_owning_group\",\"HPNRev_last_mod_user\",\"HPNRev_last_mod_date\",\"HPNRev_h4_hon_source_system\",\"HPNRev_h4_strategic_business_grp\",\"HPNRev_h4_strategic_business_unit\",\"HPNRev_h4_gold_business_enterprise\",\"HPNRev_h4_line_of_business\",\"HPNRev_h4_design_authority\",\"HPNRev_h4_product_hierarchy\",\"HPNRev_h4_sap_lab_office\",\"HPNRev_h4_Productfamily\",\"HPNRev_h4_accolade_combined_ids\",\"HPNRev_h4_mngloc\",\"HPNRev_gov_classification\",\"HPNRev_h4_part_type\",\"HPNRev_h4_part_category\",\"HPNRev_h4_sap_transfer_status\",\"HPNRev_item_id\",\"HPNRev_projects_list\",\"HPNRev_h4_MrpData\",\"HPNRev_h4_plant\",\"HPNRev_release_status_list\",\"HPNRev_date_released\",\"HPNRev_owning_project\",\"HPNRev_h4_accolade_project_tr\",\"HPNRev_h4_type_of_part_object\",\"HPNRev_h4_ecad_maturity\",\"HPNRev_h4_material_group\",\"HPNRev_h4_honeywell_requirements\",\"HPNRev_h4_sap_transfer_required\",\"HPNRev_h4_is_ecad_component\",\"HPNRev_h4_agency_approval_req\",\"HPNRev_uid\",\"HPN_uom_tag\",\"HPN_h4_reg_impact\",\"HPN_h4_hec_code\",\"HPN_h4_tec_code\",\"HPN_ECN_Release_No\",\"HPN_ECN_Status\",\"Link_to_File\"\n");
								ilines++;
							}

							for (String line : lines) {

								if (!line.isEmpty()) {

									if (ilines < 100000) {
										ilines++;
										if (EDWDataWriter != null)
											EDWDataWriter.write(line + "\n");

									} else {
										ilines = 0;
										fileCount++;
										EDWDataWriter.close();
										StrCSVFilePath = StrCSVDirPath + File.separator + "OBS_ITK_Export_"
												+ StrFileTimeStamp + "_" + fileCount + ".csv";
										EDWDataWriter = new FileWriter(StrCSVFilePath, StandardCharsets.UTF_8);
										EDWDataWriter.write("\"MPNRev_object_type\",\"MPNRev_item_revision_id\",\"MPNRev_object_name\",\"MPNRev_object_desc\",\"MPNRev_h4_strategic_business_grp\",\"MPNRev_h4_strategic_business_unit\",\"MPNRev_h4_gold_business_enterprise\",\"MPNRev_h4_line_of_business\",\"MPNRev_h4_status_rating\",\"MPNRev_h4_mpn_type\",\"MPNRev_item_id\",\"MPNRev_h4_vendor_name\",\"MPNRev_projects_list\",\"MPNRev_h4_design_authority\",\"MPNRev_h4_vendor_id\",\"MPNRev_h4_comment\",\"MPNRev_owning_project\",\"MPNRev_uid\",\"MPN_h4_mfg_part_maturity\",\"MPN_owning_user\",\"MPN_owning_group\",\"MPN_last_mod_user\",\"MPN_last_mod_date\",\"MPN_h4_reg_impact\",\"MPN_vendor_name\",\"MPN_vendor_id\",\"MPN_vm0location\",\"MPN_h4_mpn_ppl_rating\",\"MPN_h4_mpn_status\",\"MPN_uid\",\"MPN_ECN_Release_No\",\"MPN_ECN_Status\",\"HPNRev_item_revision_id\",\"HPNRev_object_name\",\"HPNRev_object_type\",\"HPNRev_object_desc\",\"HPNRev_creation_date\",\"HPNRev_owning_user\",\"HPNRev_owning_group\",\"HPNRev_last_mod_user\",\"HPNRev_last_mod_date\",\"HPNRev_h4_hon_source_system\",\"HPNRev_h4_strategic_business_grp\",\"HPNRev_h4_strategic_business_unit\",\"HPNRev_h4_gold_business_enterprise\",\"HPNRev_h4_line_of_business\",\"HPNRev_h4_design_authority\",\"HPNRev_h4_product_hierarchy\",\"HPNRev_h4_sap_lab_office\",\"HPNRev_h4_Productfamily\",\"HPNRev_h4_accolade_combined_ids\",\"HPNRev_h4_mngloc\",\"HPNRev_gov_classification\",\"HPNRev_h4_part_type\",\"HPNRev_h4_part_category\",\"HPNRev_h4_sap_transfer_status\",\"HPNRev_item_id\",\"HPNRev_projects_list\",\"HPNRev_h4_MrpData\",\"HPNRev_h4_plant\",\"HPNRev_release_status_list\",\"HPNRev_date_released\",\"HPNRev_owning_project\",\"HPNRev_h4_accolade_project_tr\",\"HPNRev_h4_type_of_part_object\",\"HPNRev_h4_ecad_maturity\",\"HPNRev_h4_material_group\",\"HPNRev_h4_honeywell_requirements\",\"HPNRev_h4_sap_transfer_required\",\"HPNRev_h4_is_ecad_component\",\"HPNRev_h4_agency_approval_req\",\"HPNRev_uid\",\"HPN_uom_tag\",\"HPN_h4_reg_impact\",\"HPN_h4_hec_code\",\"HPN_h4_tec_code\",\"HPN_ECN_Release_No\",\"HPN_ECN_Status\",\"Link_to_File\"\n");
										EDWDataWriter.write(line + "\n");
										ilines = ilines + 2;
									}
								}
							}
						}
					} catch (IOException ex) {
						ex.printStackTrace();
						EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
								"EDW CSV file process is failed with error meaasge  :[" + ex.getMessage() + "]...\n"));

					}
				}
			}
			if (EDWDataWriter != null)
				EDWDataWriter.close();
			System.out.println("****************** The process has completed ***************************");
		} catch (Exception e) {
			EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
					"EDW CSV file process is failed with error meaasge  :[" + e.getMessage() + "]...\n"));
			e.printStackTrace();
		}
	}

	private void getHPNItemRevisionDetails(ModelObject comPart, StringBuilder eDWDataBuilder) {

		HPNData hpnData = new HPNData();
		String comPartObjType = "";
		int cForHPR = 0;
		int cForHPAndStP = 0;
		EDWDataManagementSevice.getProperties(new ModelObject[] { comPart },
				new String[] { "puid", "items_tag", "item_revision_id", "object_name", "object_type", "object_desc",
						"creation_date", "owning_user", "owning_group", "last_mod_user", "last_mod_date",
						"h4_hon_source_system", "h4_strategic_business_grp", "h4_strategic_business_unit",
						"h4_gold_business_enterprise", "h4_line_of_business", "h4_design_authority",
						"h4_product_hierarchy", "h4_sap_lab_office", "h4_Productfamily", "h4_accolade_combined_ids",
						"h4_mngloc", "gov_classification", "h4_part_type", "h4_part_category", "h4_sap_transfer_status",
						"puid", "item_id", "project_list", "h4_MrpData", "h4_plant", "release_status_list",
						"date_released", "owning_project", "h4_accolade_project_tr", "h4_type_of_part_object",
						"h4_ecad_maturity", "h4_material_group", "h4_honeywell_requirements",
						"h4_sap_transfer_required", "h4_is_ecad_component", "h4_agency_approval_req" });

		try {
			comPartObjType = comPart.getPropertyDisplayableValue("object_type");
			hpnData.setHpnRevObjectType(escapeDoubleQuotes(comPart.getPropertyDisplayableValue("object_type")));
			hpnData.setHpnRevItemRevisionId(
					escapeDoubleQuotes(comPart.getPropertyDisplayableValue("item_revision_id")));
			hpnData.setHpnRevObjectName(escapeDoubleQuotes(comPart.getPropertyDisplayableValue("object_name")));
			hpnData.setHpnRevObjectDesc(escapeDoubleQuotes(comPart.getPropertyDisplayableValue("object_desc")));

			date = comPart.getPropertyDisplayableValue("creation_date");
			if (date != null && !date.isEmpty()) {
				LocalDateTime localDateTime = LocalDateTime.parse(date, inputFormatter);
				date = localDateTime.format(outputFormatter);
			}
			hpnData.setHpnRevCreationDate(date);
			hpnData.setHpnRevOwningUser(escapeDoubleQuotes(comPart.getPropertyDisplayableValue("owning_user")));
			hpnData.setHpnRevOwningGroup(escapeDoubleQuotes(comPart.getPropertyDisplayableValue("owning_group")));
			hpnData.setHpnRevLastModUser(escapeDoubleQuotes(comPart.getPropertyDisplayableValue("last_mod_user")));
			date = comPart.getPropertyDisplayableValue("last_mod_date");
			if (date != null && !date.isEmpty()) {
				LocalDateTime localDateTime = LocalDateTime.parse(date, inputFormatter);
				date = localDateTime.format(outputFormatter);
			}
			hpnData.setHpnRevLastModDate(date);

			int edaC = 0;
			if (!comPartObjType.equalsIgnoreCase("HON ECAD Project Revision")
					&& !comPartObjType.equalsIgnoreCase("HON Mfg Container Revision")
					&& !comPartObjType.equalsIgnoreCase("ECAD Schematic (Published) Revision")) {
				edaC = 1;
				hpnData.setHpnRevH4HonSourceSystem(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_hon_source_system")));
			}
			if (edaC == 0) {
				hpnData.setHpnRevH4HonSourceSystem("");
			}
			edaC = 0;

			if (!comPartObjType.equalsIgnoreCase("HON Mfg Container Revision")
					&& !comPartObjType.equalsIgnoreCase("ECAD Schematic (Published) Revision")) {
				edaC = 1;
				hpnData.setHpnRevH4StrategicBusinessGrp(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_strategic_business_grp")));
				hpnData.setHpnRevH4StrategicBusinessUnit(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_strategic_business_unit")));
				hpnData.setHpnRevH4GoldBusinessEnterprise(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_gold_business_enterprise")));
				hpnData.setHpnRevH4LineOfBusiness(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_line_of_business")));
				hpnData.setHpnRevH4ProductHierarchy(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_product_hierarchy")));
				hpnData.setHpnRevH4SapLabOffice(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_sap_lab_office")));
				hpnData.setHpnRevH4AccoladeCombinedIds(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_accolade_combined_ids")));
				hpnData.setHpnRevH4PartCategory(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_part_category")));
				hpnData.setHpnRevH4SapTransferStatus(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_sap_transfer_status")));
						
				List<String> plantData = comPart.getPropertyDisplayableValues("h4_plant");
				String plantDataList = plantData.stream()
                    .filter(s -> s != null && !s.isEmpty())
                    .collect(Collectors.joining(","));		
				hpnData.setHpnRevH4Plant(plantDataList);
				
				hpnData.setHpnRevH4AccoladeProjectTr(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_accolade_project_tr")));
				hpnData.setHpnRevH4SapTransferRequired(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_sap_transfer_required")));
			}
			if (edaC == 0) {
				hpnData.setHpnRevH4StrategicBusinessGrp("");
			}
			edaC = 0;

			hpnData.setHpnRevH4DesignAuthority(
					escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_design_authority")));
			hpnData.setHpnRevH4ProductFamily(
					escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_Productfamily")));

			hpnData.setHpnRevH4MngLoc(escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_mngloc")));
			hpnData.setHpnRevGovClassification(
					escapeDoubleQuotes(comPart.getPropertyDisplayableValue("gov_classification")));

			if (!comPartObjType.equalsIgnoreCase("HON ECAD Project Revision")
					&& !comPartObjType.equalsIgnoreCase("HON Mfg Container Revision")
					&& !comPartObjType.equalsIgnoreCase("ECAD Schematic (Published) Revision")) {
				edaC = 1;
				hpnData.setHpnRevH4PartType(escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_part_type")));
			}
			if (edaC == 0) {
				hpnData.setHpnRevH4PartType("");
			}
			edaC = 0;

			hpnData.setHpnRevItemId(escapeDoubleQuotes(comPart.getPropertyDisplayableValue("item_id")));

			List<String> projects = comPart.getPropertyDisplayableValues("project_list");

			hpnData.setHpnRevProjectsList(projects.stream()
                    .filter(s -> s != null && !s.isEmpty())
                    .collect(Collectors.joining(",")));

			List<String> mrpData = comPart.getPropertyDisplayableValues("h4_MrpData");

			hpnData.setHpnRevH4MrpData(mrpData.stream()
                    .filter(s -> s != null && !s.isEmpty())
                    .collect(Collectors.joining(",")));

			List<String> releaseStatusList = comPart.getPropertyObject("release_status_list").getDisplayableValues();

			hpnData.setHpnRevReleaseStatusList(releaseStatusList.stream()
                    .filter(s -> s != null && !s.isEmpty())
                    .collect(Collectors.joining(",")));

			date = comPart.getPropertyDisplayableValue("date_released");
			if (date != null && !date.isEmpty()) {
				LocalDateTime localDateTime = LocalDateTime.parse(date, inputFormatter);
				date = localDateTime.format(outputFormatter);
			}
			hpnData.setHpnRevDateReleased(date);

			hpnData.setHpnRevOwningProject(escapeDoubleQuotes(comPart.getPropertyDisplayableValue("owning_project")));

			if (comPartObjType.equalsIgnoreCase("HON ECAD Project Revision")) {
				edaC = 1;
				hpnData.setHpnRevH4TypeOfPartObject(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_type_of_part_object")));
				hpnData.setHpnRevH4AgencyApprovalReq(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_agency_approval_req")));
			}
			if (edaC == 0) {
				hpnData.setHpnRevH4TypeOfPartObject("");
				hpnData.setHpnRevH4AgencyApprovalReq("");
			}
			edaC = 0;

			if (comPartObjType.equalsIgnoreCase("HON Part Revision")) {

				cForHPR = +1;
				hpnData.setHpnRevH4TypeOfPartObject(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_type_of_part_object")));
				hpnData.setHpnRevH4MaterialGroup(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_material_group")));
				hpnData.setHpnRevH4HoneywellRequirements(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_honeywell_requirements")));
				hpnData.setHpnRevH4IsEcadComponent(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_is_ecad_component")));
				hpnData.setHpnRevH4AgencyApprovalReq(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_agency_approval_req")));

			}
			if (comPartObjType.equalsIgnoreCase("Standard Part Revision")) {

				cForHPAndStP = +1;
				hpnData.setHpnRevH4IsEcadComponent(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_is_ecad_component")));
				hpnData.setHpnRevH4AgencyApprovalReq(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_agency_approval_req")));

			}

			if (!comPartObjType.equalsIgnoreCase("HON ECAD Project Revision")
					&& !comPartObjType.equalsIgnoreCase("HON Mfg Container Revision")
					&& !comPartObjType.equalsIgnoreCase("ECAD Schematic (Published) Revision")) {
				edaC = 1;
				hpnData.setHpnRevH4EcadMaturity(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_ecad_maturity")));
			}
			if (edaC == 0) {
				hpnData.setHpnRevH4EcadMaturity("");
			}
			edaC = 0;

			if (comPartObjType.equalsIgnoreCase("HON ECAD Project Revision")) {
				edaC = 1;
				hpnData.setHpnRevH4AgencyApprovalReq(
						escapeDoubleQuotes(comPart.getPropertyDisplayableValue("h4_agency_approval_req")));
			}
			if (edaC == 0) {
				hpnData.setHpnRevH4AgencyApprovalReq("");
			}
			edaC = 0;
			if (cForHPR == 0) {

				hpnData.setHpnRevH4TypeOfPartObject("");
				hpnData.setHpnRevH4MaterialGroup("");
				hpnData.setHpnRevH4HoneywellRequirements("");
				hpnData.setHpnRevH4IsEcadComponent("");
				hpnData.setHpnRevH4AgencyApprovalReq("");

			}

			if (cForHPAndStP == 0) {

				hpnData.setHpnRevH4IsEcadComponent("");
				hpnData.setHpnRevH4AgencyApprovalReq("");

			}
			// HPN Rev Details
			hpnData.setHpnRevPuid(escapeDoubleQuotes(comPart.getUid()));
			eDWDataBuilder.append("\"").append(hpnData.getHpnRevItemRevisionId()).append("\",").append("\"")
					.append(hpnData.getHpnRevObjectName()).append("\",").append("\"")
					.append(hpnData.getHpnRevObjectType()).append("\",").append("\"")
					.append(hpnData.getHpnRevObjectDesc()).append("\",").append("\"")
					.append(hpnData.getHpnRevCreationDate()).append("\",").append("\"")
					.append(hpnData.getHpnRevOwningUser()).append("\",").append("\"")
					.append(hpnData.getHpnRevOwningGroup()).append("\",").append("\"")
					.append(hpnData.getHpnRevLastModUser()).append("\",").append("\"")
					.append(hpnData.getHpnRevLastModDate()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4HonSourceSystem()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4StrategicBusinessGrp()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4StrategicBusinessUnit()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4GoldBusinessEnterprise()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4LineOfBusiness()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4DesignAuthority()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4ProductHierarchy()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4SapLabOffice()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4ProductFamily()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4AccoladeCombinedIds()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4MngLoc()).append("\",").append("\"")
					.append(hpnData.getHpnRevGovClassification()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4PartType()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4PartCategory()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4SapTransferStatus()).append("\",").append("\"")
					.append(hpnData.getHpnRevItemId()).append("\",").append("\"")
					.append(hpnData.getHpnRevProjectsList()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4MrpData()).append("\",").append("\"").append(hpnData.getHpnRevH4Plant())
					.append("\",").append("\"").append(hpnData.getHpnRevReleaseStatusList()).append("\",").append("\"")
					.append(hpnData.getHpnRevDateReleased()).append("\",").append("\"")
					.append(hpnData.getHpnRevOwningProject()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4AccoladeProjectTr()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4TypeOfPartObject()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4EcadMaturity()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4MaterialGroup()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4HoneywellRequirements()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4SapTransferRequired()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4IsEcadComponent()).append("\",").append("\"")
					.append(hpnData.getHpnRevH4AgencyApprovalReq()).append("\",").append("\"")
					.append(hpnData.getHpnRevPuid()).append("\",");

			ModelObject hpnItem = comPart.getPropertyObject("items_tag").getModelObjectValue();

			if (hpnItem != null) {
				EDWDataManagementSevice.getProperties(new ModelObject[] { hpnItem },
						new String[] { "uom_tag", "h4_reg_impact", "h4_hec_code", "h4_tec_code" });
				hpnData.setHpnUomTag(escapeDoubleQuotes(hpnItem.getPropertyDisplayableValue("uom_tag")));

				List<String> regImpact = hpnItem.getPropertyObject("h4_reg_impact").getDisplayableValues();


				hpnData.setHpnH4RegImpact(regImpact.stream()
	                    .filter(s -> s != null && !s.isEmpty())
	                    .collect(Collectors.joining(",")));
				
				hpnData.setHpnH4HecCode(escapeDoubleQuotes(hpnItem.getPropertyDisplayableValue("h4_hec_code")));
				hpnData.setHpnH4TecCode(escapeDoubleQuotes(hpnItem.getPropertyDisplayableValue("h4_tec_code")));

				// HPN Details

				eDWDataBuilder.append("\"").append(hpnData.getHpnUomTag()).append("\",").append("\"")
						.append(hpnData.getHpnH4RegImpact()).append("\",").append("\"")
						.append(hpnData.getHpnH4HecCode()).append("\",").append("\"").append(hpnData.getHpnH4TecCode())
						.append("\",");

				EDWDataManagementSevice.getProperties(new ModelObject[] { comPart },
						new String[] { "cm0AuthoringChangeRevision" });
				ModelObject cm0AuthoringChangeRevision = comPart.getPropertyObject("cm0AuthoringChangeRevision")
						.getModelObjectValue();

				if (isECN_AMPL_MPN == 0 && cm0AuthoringChangeRevision != null) {
					EDWDataManagementSevice.getProperties(new ModelObject[] { cm0AuthoringChangeRevision },
							new String[] { "item_id", "CMMaturity" });
					hpnData.setHpnEcnReleaseNo(cm0AuthoringChangeRevision.getPropertyDisplayableValue("item_id"));
					hpnData.setHpnEcnStatus(cm0AuthoringChangeRevision.getPropertyDisplayableValue("CMMaturity"));

					eDWDataBuilder.append("\"").append(hpnData.getHpnEcnReleaseNo()).append("\",").append("\"")
							.append(hpnData.getHpnEcnStatus()).append("\",");
				} else {
					eDWDataBuilder.append("\"").append("").append("\",").append("\"").append("").append("\",");
				}
				hpnData.setLinkToFile(getLinkValue(hpnData));
				eDWDataBuilder.append("\"").append(hpnData.getLinkToFile()).append("\"\n");
			}

		} catch (NotLoadedException e) {
			e.printStackTrace();
		}
	}
	
	public static String escapeDoubleQuotes(String input) {
		String out = "";
		if (input == null) {
			return out;
		}
		out = input.replace("\n", " ").replace("\"", "");
		return out;
	}

	public static String nullCheckInt(String nullCheck) {

		if (nullCheck.equalsIgnoreCase("") || nullCheck == null) {
			return "0";
		} else {
			return nullCheck;
		}
	}

	public List<ModelObject> getPrimaryObjects(ModelObject affectedItem) {

		List<ModelObject> primaryObjects = new ArrayList<>();

		RelationAndTypesFilter typeFilter = new RelationAndTypesFilter();
		typeFilter.relationTypeName = "H4_AMPLItems"; // relation;

		ExpandGRMRelationsPref2 relationPref = new ExpandGRMRelationsPref2();
		relationPref.expItemRev = true;// false
		relationPref.returnRelations = true;
		relationPref.info = new RelationAndTypesFilter[] { typeFilter };

		ExpandGRMRelationsResponse2 relationResp = EDWDataManagementSevice
				.expandGRMRelationsForSecondary(new ModelObject[] { affectedItem }, relationPref);
		ModelObject SecModelObj = null;

		for (ExpandGRMRelationsOutput2 relationOut : relationResp.output) {
			ModelObject SecInputObj = relationOut.inputObject;

			for (ExpandGRMRelationsData2 relationData : relationOut.relationshipData) {
				for (ExpandGRMRelationship relation1 : relationData.relationshipObjects) {

					if (relation1.relation instanceof ImanRelation) {

						SecModelObj = relation1.otherSideObject;
						EDWDataManagementSevice.getProperties(new ModelObject[] { SecModelObj },
								new String[] { "object_type", "object_string" });

						try {
							String objType = SecModelObj.getPropertyDisplayableValue("object_type");

							if (objType.equalsIgnoreCase("Engineering Change Notice Revision")) {
								primaryObjects.add(SecModelObj);
							}

						} catch (NotLoadedException e) {

							e.printStackTrace();
						}
					}
				}
			}
		}
		return primaryObjects;
	}

	// for link preference manager
	public String getLinkValue(HPNData hpnData) {

		PreferenceManagementService preferenceService = PreferenceManagementService
				.getService(AppXSession.getConnection());
		GetPreferencesResponse preferenceSearchResponse = preferenceService
				.getPreferences(new String[] { "ActiveWorkspaceHosting.URL" }, false);

		String[] allPrefValues = null;
		String value = "";

		int prefFlag = preferenceSearchResponse.response.length;
		if (prefFlag > 0) {
			CompletePreference[] completeDetails = preferenceSearchResponse.response;

			for (CompletePreference prefDetails : completeDetails) {
				PreferenceValue prefValue = prefDetails.values;
				allPrefValues = prefValue.values;
				System.out.println(allPrefValues[0]);
			}
			value += allPrefValues[0];
			value += "/#/com.siemens.splm.clientfx.tcui.xrt.showObject?page=Attachments&pageId=attachments&uid=";
			value += hpnData.getHpnRevPuid();
			return value;
		} else {
			EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
					"Cannot find the preference value in Teamcenter, Please contact Teamcenter administrator to create preference...\n"));
		}
		return value;
	}

}