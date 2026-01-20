package com.teamcenter.soa.loader;

import java.io.IOException;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Locale;

import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.ImanRelation;
import com.teamcenter.soa.client.model.strong.Person;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.soa.exceptions.NotLoadedException;
import com.teamcenter.soa.util.CertificateExpiryUtil;
import com.teamcenter.model.BAData;
import com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsData2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsOutput2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsPref2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsResponse2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationship;

public class ProcessCatalog {

	// Global map declaration
	public static Map<ModelObject, Set<ModelObject>> Catalogs = new HashMap<>();
	public static List<ModelObject> regOwners = new ArrayList<ModelObject>();
	public static String noti1 = "";
	public static List<ModelObject> noti2 = new ArrayList<ModelObject>();
	public static List<ModelObject> noti3 = new ArrayList<ModelObject>();
	public static List<ModelObject> regMngrs = new ArrayList<ModelObject>();

	private static Map<String, Set<String>> expired = new HashMap<>();
	private static Map<String, Set<String>> expiringIn3months = new HashMap<>();
	private static Map<String, Set<String>> expiringIn6months = new HashMap<>();
	private static Map<String, Set<String>> expiringIn1Year = new HashMap<>();
	private static Map<String, Set<String>> renewed3monthsago = new HashMap<>();
	private static Set<ModelObject> uniqueSKU = new HashSet<ModelObject>();

	public static BAData baData = new BAData();
	public static CertificateExpiryUtil certExpUtil = new CertificateExpiryUtil();
	public static String certAWCurl = "";

	public static void processCatalogObjects(ArrayList<ModelObject> BACatalogs) throws IOException {
		try {
			Connect.BADataManagementSevice.getProperties(BACatalogs.toArray(new ModelObject[BACatalogs.size()]),
					new String[] { "h4_regulatory_owner", "h4_notifier1", "h4_notifier2", "h4_notifier3",
							"h4_regulatory_manager", "h4_notification_freq", "object_name" });
			
			for (int i = 0; i < BACatalogs.size(); i++) {
				baData = new BAData();
				System.out.println("BACatalogs  : " + (i + 1));
				Connect.writer.write("BACatalogs  : " + (i + 1) + "\n");

				baData.setHbtName(BACatalogs.get(i).getPropertyDisplayableValue("object_name"));
				System.out.println("BACatalogs object_name : " + baData.getHbtName());
				Connect.writer.write("BACatalogs object_name : " + baData.getHbtName() + "\n");

				baData.setHbtNotiFreq(BACatalogs.get(i).getPropertyDisplayableValue("h4_notification_freq"));
				System.out.println("BACatalogs h4_notification_freq : " + baData.getHbtNotiFreq());
				Connect.writer.write("BACatalogs h4_notification_freq : " + baData.getHbtNotiFreq() + "\n");

				List<String> regOwnersList = BACatalogs.get(i).getPropertyDisplayableValues("h4_regulatory_owner");
				baData.setRegOwners(String.join(",", regOwnersList));
				System.out.println("BACatalogs regOwnersStr : " + baData.getRegOwners());
				Connect.writer.write("BACatalogs regOwnersStr : " + baData.getRegOwners() + "\n");

				regOwners = BACatalogs.get(i).getPropertyObject("h4_regulatory_owner").getModelObjectListValue();

				noti1 = BACatalogs.get(i).getPropertyDisplayableValue("h4_notifier1");
				Connect.writer.write("BACatalogs noti1 : " + noti1 + "\n");
				noti2 = BACatalogs.get(i).getPropertyObject("h4_notifier2").getModelObjectListValue();
				noti3 = BACatalogs.get(i).getPropertyObject("h4_notifier3").getModelObjectListValue();
				regMngrs = BACatalogs.get(i).getPropertyObject("h4_regulatory_manager").getModelObjectListValue();

//				for (ModelObject owner : regOwners) {
//					Catalogs.computeIfAbsent(owner, k -> new HashSet<>());
//					Catalogs.get(owner).add(BACatalogs.get(i));
//				}
//				for (ModelObject owner : noti2) {
//					Catalogs.computeIfAbsent(owner, k -> new HashSet<>());
//					Catalogs.get(owner).add(BACatalogs.get(i));
//				}
//				for (ModelObject owner : noti3) {
//					Catalogs.computeIfAbsent(owner, k -> new HashSet<>());
//					Catalogs.get(owner).add(BACatalogs.get(i));
//				}
//				for (ModelObject owner : regMngrs) {
//					Catalogs.computeIfAbsent(owner, k -> new HashSet<>());
//					Catalogs.get(owner).add(BACatalogs.get(i));
//				}
				getCertificateObjects(BACatalogs.get(i));
			}
			// Print results
			System.out.println("***** " + baData.getHbtNotiFreq() + " *****");
			Connect.writer.write("***** " + baData.getHbtNotiFreq() + " *****\n");
//			for (Map.Entry<ModelObject, Set<ModelObject>> entry : Catalogs.entrySet()) {
//				Connect.BADataManagementSevice.getProperties(new ModelObject[] { entry.getKey() },
//						new String[] { "user_name" });
//				String userStr = entry.getKey().getPropertyDisplayableValue("user_name");
//				System.out.print("Owner: " + userStr);
//				Connect.writer.write("Owner: " + userStr);
//				for (ModelObject mo : entry.getValue()) {
//					Connect.BADataManagementSevice.getProperties(new ModelObject[] { mo },
//							new String[] { "object_name" });
//					String valueStr = mo.getPropertyDisplayableValue("object_name");
//					System.out.println("  -> " + valueStr);
//					Connect.writer.write("  -> " + valueStr + "\n");
//				}
//			}
		} catch (NotLoadedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public static void getCertificateObjects(ModelObject BACatalog) throws IOException {
		System.out.println("************* Enter getCertificateObjects ***************");
		Connect.writer.write("************* Enter getCertificateObjects ***************\n");

		try {
			RelationAndTypesFilter typeFilter = new RelationAndTypesFilter();
			typeFilter.relationTypeName = "H4_hon_hbt_catalogs"; // relation;

			ExpandGRMRelationsPref2 relationPref = new ExpandGRMRelationsPref2();
			relationPref.expItemRev = true;// false
			relationPref.returnRelations = true;
			relationPref.info = new RelationAndTypesFilter[] { typeFilter };

			ExpandGRMRelationsResponse2 relationResp = Connect.BADataManagementSevice
					.expandGRMRelationsForSecondary(new ModelObject[] { BACatalog }, relationPref);
			ModelObject SecModelObj = null;

			for (ExpandGRMRelationsOutput2 relationOut : relationResp.output) {
				for (ExpandGRMRelationsData2 relationData : relationOut.relationshipData) {
					for (ExpandGRMRelationship relation1 : relationData.relationshipObjects) {
						if (relation1.relation instanceof ImanRelation) {
							SecModelObj = relation1.otherSideObject;
							Connect.BADataManagementSevice.getProperties(new ModelObject[] { SecModelObj },
									new String[] { "object_type" });
							String objType = SecModelObj.getPropertyDisplayableValue("object_type");
							if (objType.equals("HON Compliance Document Revision")) {
								Connect.BADataManagementSevice.getProperties(new ModelObject[] { SecModelObj },
										new String[] { "uid", "object_type", "object_string", "object_name",
												"h4_document_type", "h4_expiry_date", "h4_withdrawal_date",
												"h4_doc_number", "h4_action_required_timeline",
												"H4_HasComplianceDoc_Rel", "h4_action_required_date", "date_released",
												"h4_action_req_timeline", "h4_action_req_date", "h4_release_status", "h4_renewal_status" });
								baData.setCertName("");
								baData.setDocType("");
								baData.setDocNum("");
								baData.setTimeline("");
								baData.setActnNeededBy("");
								baData.setDateReleased("");
								baData.setAffctdRegSpec("");
								baData.setCertAWCurl("");
								certAWCurl = certExpUtil.getAWCurl();
								certAWCurl += "/awc/#/com.siemens.splm.clientfx.tcui.xrt.showObject?page=Associated%20SKUs&pageId=Associated%20SKUs&uid=";
								certAWCurl += SecModelObj.getUid();
								baData.setCertAWCurl(certAWCurl);
								System.out.println("setCertAWCurl : " + baData.getCertAWCurl());
								Connect.writer.write("setCertAWCurl : " + baData.getCertAWCurl());
//								String certUid = SecModelObj.getUid();
//								if (processedCertificates.contains(certUid)) {
//									System.out.println("Skipping object: " + certUid);
//									Connect.writer.write("Skipping object: " + certUid);
//									continue;
//								}
								baData.setCertName(SecModelObj.getPropertyDisplayableValue("object_string"));
								baData.setDocType(SecModelObj.getPropertyDisplayableValue("h4_document_type"));
								baData.setDocNum(SecModelObj.getPropertyDisplayableValue("h4_doc_number"));
								baData.setTimeline(
										SecModelObj.getPropertyDisplayableValue("h4_action_required_timeline"));
								baData.setActnNeededBy(
										SecModelObj.getPropertyDisplayableValue("h4_action_required_date"));
								baData.setDateReleased(SecModelObj.getPropertyDisplayableValue("date_released"));
								List<String> renewalStatus = SecModelObj.getPropertyDisplayableValues("h4_renewal_status");
								
								int length = relationData.relationshipObjects.length;
								System.out.println("Certificates length : " + length);
								Connect.writer.write("Certificates length : " + length + "\n");
								System.out.println("CERT Object String: " + baData.getCertName());
								Connect.writer.write("CERT Object String: " + baData.getCertName() + "\n");
								System.out.println("CERT Object Type: " + objType);
								Connect.writer.write("CERT Object Type: " + objType + "\n");
								System.out.println("CERT Object h4_document_type: " + baData.getDocType());
								Connect.writer.write("CERT Object h4_document_type: " + baData.getDocType() + "\n");
								String relStat = SecModelObj.getPropertyDisplayableValue("h4_release_status");
								System.out.println("CERT Object relStat: " + relStat);
								Connect.writer.write("CERT Object relStat: " + relStat + "\n");
								System.out.println("CERT Object renewalStatus: " + String.join(",", renewalStatus));
								Connect.writer.write("CERT Object renewalStatus: " + String.join(",", renewalStatus) + "\n");
								if (relStat.equalsIgnoreCase("Superseded") || relStat.equalsIgnoreCase("Obsolete")) {
									continue;
								}
								if(renewalStatus.contains("No Action Required")) {
									continue;
								}
								if (baData.getDocType().equals("Certification Scheme")) {
									baData.setExpOrWithdDate("");
									baData.setExpOrWithdDate(SecModelObj.getPropertyDisplayableValue("h4_expiry_date"));
									System.out.println("CERT Object h4_expiry_date: " + baData.getExpOrWithdDate());
									Connect.writer
											.write("CERT Object h4_expiry_date: " + baData.getExpOrWithdDate() + "\n");
									getCertificateObjectsForPrimary(SecModelObj, BACatalog);
									getSKUObjects(SecModelObj, BACatalog);
								}
								if (baData.getDocType().equals("Regulatory Specification")) {
									baData.setExpOrWithdDate("");
									baData.setExpOrWithdDate(
											SecModelObj.getPropertyDisplayableValue("h4_withdrawal_date"));
									System.out.println("CERT Object h4_withdrawal_date: " + baData.getExpOrWithdDate());
									Connect.writer.write(
											"CERT Object h4_withdrawal_date: " + baData.getExpOrWithdDate() + "\n");
									getCertificateObjectsForPrimary(SecModelObj, BACatalog);
								}
							}
						}
					}
				}
			}
			System.out.println("************* Exit getCertificateObjects ***************");
			Connect.writer.write("************* Exit getCertificateObjects ***************\n");
		} catch (NotLoadedException e) {
			e.printStackTrace();
		}
	}

	public static void getCertificateObjectsForPrimary(ModelObject primaryObj, ModelObject BACatalog)
			throws IOException {
		System.out.println("************* Enter getCertificateObjectsForRegulatorySpec ***************");
		Connect.writer.write("************* Enter getCertificateObjectsForRegulatorySpec ***************\n");

		try {

			Connect.BADataManagementSevice.getProperties(new ModelObject[] { primaryObj },
					new String[] { "object_type", "h4_document_type" });
			String objType = primaryObj.getPropertyDisplayableValue("object_type");
			String h4_document_type = primaryObj.getPropertyDisplayableValue("h4_document_type");
			if (objType.equals("HON Compliance Document Revision")) {
				if (h4_document_type.equalsIgnoreCase("Certification Scheme")) {
					Connect.BADataManagementSevice.getProperties(new ModelObject[] { primaryObj },
							new String[] { "H4_HasComplianceDoc_Rel" });
					List<String> affectedSpecs = primaryObj.getPropertyObject("H4_HasComplianceDoc_Rel")
							.getDisplayableValues();
					baData.setAffctdRegSpec("");
					baData.setAffctdRegSpec(String.join("\n", affectedSpecs));
					System.out.println("CERT Object setAffctdRegSpec: " + baData.getAffctdRegSpec());
					Connect.writer.write("CERT Object setAffctdRegSpec: " + baData.getAffctdRegSpec() + "\n");
				}
				if (h4_document_type.equalsIgnoreCase("Regulatory Specification")) {
					getCertificateObjectsForRegSpec(primaryObj);
				}
			}
			System.out.println("************* Exit getCertificateObjectsForRegulatorySpec ***************");
			Connect.writer.write("************* Exit getCertificateObjectsForRegulatorySpec ***************\n");
		} catch (NotLoadedException e) {
			e.printStackTrace();
		}
	}

	public static void getCertificateObjectsForRegSpec(ModelObject primaryObj) throws IOException {
		System.out.println("************* Enter getCertificateObjectsForRegSpec ***************");
		Connect.writer.write("************* Enter getCertificateObjectsForRegSpec ***************\n");
		String affctdCertScheme = "";
		try {
			RelationAndTypesFilter typeFilter = new RelationAndTypesFilter();
			typeFilter.relationTypeName = "H4_HasComplianceDoc_Rel"; // relation;

			ExpandGRMRelationsPref2 relationPref = new ExpandGRMRelationsPref2();
			relationPref.expItemRev = true;// false
			relationPref.returnRelations = true;
			relationPref.info = new RelationAndTypesFilter[] { typeFilter };

			ExpandGRMRelationsResponse2 relationResp = Connect.BADataManagementSevice
					.expandGRMRelationsForSecondary(new ModelObject[] { primaryObj }, relationPref);
			ModelObject SecModelObj = null;

			for (ExpandGRMRelationsOutput2 relationOut : relationResp.output) {
				for (ExpandGRMRelationsData2 relationData : relationOut.relationshipData) {
					for (ExpandGRMRelationship relation1 : relationData.relationshipObjects) {
						if (relation1.relation instanceof ImanRelation) {
							SecModelObj = relation1.otherSideObject;
							Connect.BADataManagementSevice.getProperties(new ModelObject[] { SecModelObj },
									new String[] { "object_type" });
							String objType = SecModelObj.getPropertyDisplayableValue("object_type");
							if (objType.equals("HON Compliance Document Revision")) {
								Connect.BADataManagementSevice.getProperties(new ModelObject[] { SecModelObj },
										new String[] { "h4_document_type", "object_string" });
								String objStr = SecModelObj.getPropertyDisplayableValue("object_string");
								affctdCertScheme += objStr;
								affctdCertScheme += "\n";
								String docType = SecModelObj.getPropertyDisplayableValue("h4_document_type");
								System.out.println("CERT Object getCertificateObjectsForRegSpec: " + objStr);
								Connect.writer.write("CERT Object getCertificateObjectsForRegSpec: " + objStr + "\n");
								if (docType != null && docType.equalsIgnoreCase("Certification Scheme")) {
									getSKUObjectsForRegCert(SecModelObj);
								}
							}
						}
					}
				}
			}
			if (affctdCertScheme.endsWith("\n")) {
				affctdCertScheme = affctdCertScheme.substring(0, affctdCertScheme.length() - 1);
			}
			baData.setAffctdRegSpec(affctdCertScheme);
			baData.setSkuCount(0);
			System.out.println("count of uniqueSKU : " + uniqueSKU.size());
			Connect.writer.write("count of uniqueSKU : " + uniqueSKU.size());
			baData.setSkuCount(uniqueSKU.size());
			uniqueSKU.clear();
			processEmails();
			System.out.println("************* Exit getCertificateObjectsForRegulatorySpec ***************");
			Connect.writer.write("************* Exit getCertificateObjectsForRegulatorySpec ***************\n");
		} catch (NotLoadedException e) {
			e.printStackTrace();
		}
	}

	public static void getSKUObjectsForRegCert(ModelObject primaryCertificateObject) throws IOException {
		System.out.println("************* Enter getSKUObjects ***************");
		Connect.writer.write("************* Enter getSKUObjects ***************\n");
		try {
			Connect.BADataManagementSevice.getProperties(new ModelObject[] { primaryCertificateObject },
					new String[] { "object_string" });
			RelationAndTypesFilter typeFilter = new RelationAndTypesFilter();
			typeFilter.relationTypeName = "H4_cert_doc_rel"; // relation;

			ExpandGRMRelationsPref2 relationPref = new ExpandGRMRelationsPref2();
			relationPref.expItemRev = true;// false
			relationPref.returnRelations = true;
			relationPref.info = new RelationAndTypesFilter[] { typeFilter };

			ExpandGRMRelationsResponse2 relationResp = Connect.BADataManagementSevice
					.expandGRMRelationsForSecondary(new ModelObject[] { primaryCertificateObject }, relationPref);

			for (ExpandGRMRelationsOutput2 relationOut : relationResp.output) {
				for (ExpandGRMRelationsData2 relationData : relationOut.relationshipData) {
					for (ExpandGRMRelationship relation1 : relationData.relationshipObjects) {
						if (relation1.relation instanceof ImanRelation) {
							ModelObject SecModelObj = relation1.otherSideObject;
							Connect.BADataManagementSevice.getProperties(new ModelObject[] { SecModelObj },
									new String[] { "object_string" });
							String objString = SecModelObj.getPropertyDisplayableValue("object_string");
							System.out.println("CERT Object getSKUObjectsForRegCert: " + objString);
							Connect.writer.write("CERT Object getSKUObjectsForRegCert: " + objString + "\n");
							uniqueSKU.add(SecModelObj);
						}
					}
				}
			}
			System.out.println("************* Exit getSKUObjects ***************");
			Connect.writer.write("************* Exit getSKUObjects ***************\n");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void getSKUObjects(ModelObject primaryCertificateObject, ModelObject BACatalog) throws IOException {
		System.out.println("************* Enter getSKUObjects ***************");
		Connect.writer.write("************* Enter getSKUObjects ***************\n");
		try {
			Connect.BADataManagementSevice.getProperties(new ModelObject[] { primaryCertificateObject },
					new String[] { "object_string" });
			RelationAndTypesFilter typeFilter = new RelationAndTypesFilter();
			typeFilter.relationTypeName = "H4_cert_doc_rel"; // relation;

			ExpandGRMRelationsPref2 relationPref = new ExpandGRMRelationsPref2();
			relationPref.expItemRev = true;// false
			relationPref.returnRelations = true;
			relationPref.info = new RelationAndTypesFilter[] { typeFilter };

			ExpandGRMRelationsResponse2 relationResp = Connect.BADataManagementSevice
					.expandGRMRelationsForSecondary(new ModelObject[] { primaryCertificateObject }, relationPref);

			for (ExpandGRMRelationsOutput2 relationOut : relationResp.output) {

				for (ExpandGRMRelationsData2 relationData : relationOut.relationshipData) {
					for (ExpandGRMRelationship relation1 : relationData.relationshipObjects) {
						if (relation1.relation instanceof ImanRelation) {
							ModelObject SecModelObj = relation1.otherSideObject;
							Connect.BADataManagementSevice.getProperties(new ModelObject[] { SecModelObj },
									new String[] { "object_string" });
							String objString = SecModelObj.getPropertyDisplayableValue("object_string");
							System.out.println("CERT Object getSKUObjectsForRegCert: " + objString);
							Connect.writer.write("CERT Object getSKUObjectsForRegCert: " + objString + "\n");
							uniqueSKU.add(SecModelObj);
						}
					}
				}
			}
			baData.setSkuCount(0);
			baData.setSkuCount(uniqueSKU.size());
			uniqueSKU.clear();
			System.out.println("SKU length : " + baData.getSkuCount());
			Connect.writer.write("SKU length : " + baData.getSkuCount() + "\n");
			System.out.println("************* Exit getSKUObjects ***************");
			Connect.writer.write("************* Exit getSKUObjects ***************\n");
			processEmails();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void processEmails() {
		ArrayList<ModelObject> allUsersForACatalog = new ArrayList<ModelObject>();
		allUsersForACatalog.addAll(regOwners);
		allUsersForACatalog.addAll(noti2);
		allUsersForACatalog.addAll(noti3);
		allUsersForACatalog.addAll(regMngrs);

		ArrayList<User> usersList = new ArrayList<User>();
		for (ModelObject obj : allUsersForACatalog) {
			User user = (User) obj;
			usersList.add(user);
		}
		
		Set<String> userEmails = new HashSet<>();
//		userEmails = getEmails(usersList);)
		userEmails.add("cheepirisetti.praneeth@honeywell.com");
		userEmails.add("GUGULOTHU.SAIDHEEP@Honeywell.com");
		
//		if(noti1 != null && !noti1.trim().isEmpty()) {
//			userEmails.add(noti1);
//		}
		System.out.println("userMails");

		for (String email : userEmails) {
			System.out.println(email);
			try {
				Connect.writer.write(email + "\n");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		// Map of emails with list of objects data in respective maps
		System.out.println("categorizeAndStore : " + baData.getExpOrWithdDate());
		if (baData.getExpOrWithdDate() != null && !baData.getExpOrWithdDate().trim().isEmpty()) {
			categorizeAndStore(baData.getExpOrWithdDate(), userEmails, baData.getCertName(), baData.getDocType(),
					baData.getSkuCount() + "", baData.getDocNum(), baData.getAffctdRegSpec(), baData.getTimeline(),
					baData.getActnNeededBy(), baData.getRegOwners(), baData.getDateReleased(), baData.getCertAWCurl());
			printAllMaps();
		}

	}

	public static void printAllMaps() {
		try {
			System.out.println("\n===== Expired =====");
			Connect.writer.write("\n===== Expired =====");
			printMap(expired);

			System.out.println("\n===== Expiring in 0-3 months =====");
			Connect.writer.write("\n===== Expiring in 0-3 months =====");
			printMap(expiringIn3months);

			System.out.println("\n===== Expiring in 3-6 months =====");
			Connect.writer.write("\n===== Expiring in 3-6 months =====");
			printMap(expiringIn6months);

			System.out.println("\n===== Expiring in 6-12 months =====");
			Connect.writer.write("\n===== Expiring in 6-12 months =====");
			printMap(expiringIn1Year);

			System.out.println("\n===== Renewed 3 months ago / Others =====");
			Connect.writer.write("\n===== Renewed 3 months ago / Others =====");
			printMap(renewed3monthsago);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public static void printMap(Map<String, Set<String>> map) {
		try {
			if (map.isEmpty()) {
				System.out.println("No entries");
				Connect.writer.write("No entries\n");
			}
			for (Map.Entry<String, Set<String>> entry : map.entrySet()) {
				String email = entry.getKey();
				Set<String> values = entry.getValue();
				System.out.println("Email: " + email);
				Connect.writer.write("Email: " + email + "\n");
				for (String value : values) {
					System.out.println("Values: " + value);
					System.out.println("------------------------");
					Connect.writer.write("Values: " + value + "\n");
					Connect.writer.write("------------------------\n");
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	// Function to categorize and store
	public static void categorizeAndStore(String expDate, Set<String> userEmails, String CertName, String DocType,
	                                      String SkuCount, String DocNum, String AffctdRegSpec, String Timeline,
	                                      String ActnNeededBy, String RegOwners, String relDate, String awcUrl) {

	    String status = getExpiryStatus(expDate, relDate);
	    System.out.println("STATUS : " + status);

	    String delimiter = "\t";

	    // Build concatenated data (with RegOwners)
	    StringBuilder concatenatedData = new StringBuilder();
	    concatenatedData.append(CertName).append(delimiter).append(DocType).append(delimiter).append(SkuCount)
	            .append(delimiter).append(DocNum).append(delimiter);

	    if (AffctdRegSpec != null && !AffctdRegSpec.isEmpty()) {
	        String[] lines = AffctdRegSpec.split("\\r?\\n");
	        for (String line : lines) {
	            concatenatedData.append(line).append("\n");
	        }
	    }

	    concatenatedData.append(delimiter).append(Timeline).append(delimiter).append(ActnNeededBy).append(delimiter)
	            .append(RegOwners).append(delimiter).append(expDate).append(delimiter).append(awcUrl);

	    String finalData = concatenatedData.toString();
	    System.out.println("*** concatenatedData : " + finalData);

	    if (status != null && !status.trim().isEmpty()) {
	        for (String email : userEmails) {
	            Set<String> currentSet = null;

	            switch (status) {
	                case "Expired":
	                    currentSet = expired.computeIfAbsent(email, k -> new HashSet<>());
	                    break;
	                case "Expiring in 0-3 months":
	                    currentSet = expiringIn3months.computeIfAbsent(email, k -> new HashSet<>());
	                    break;
	                case "Expiring in 3-6 months":
	                    currentSet = expiringIn6months.computeIfAbsent(email, k -> new HashSet<>());
	                    break;
	                case "Expiring in 6-12 months":
	                    currentSet = expiringIn1Year.computeIfAbsent(email, k -> new HashSet<>());
	                    break;
	                case "Renewed 3 months ago":
	                    currentSet = renewed3monthsago.computeIfAbsent(email, k -> new HashSet<>());
	                    break;
	                default:
	                    System.out.println("Unknown status: " + status);
	                    continue;
	            }

	            // Check if an entry with the same CertName already exists for this email
	            boolean alreadyExists = currentSet.stream().anyMatch(data -> data.startsWith(CertName + delimiter));
	            if (!alreadyExists) {
	                currentSet.add(finalData);
	            } else {
	                System.out.println("Skipping duplicate for CertName=" + CertName + " for email=" + email);
	            }
	        }
	    }
	}

	public static String getExpiryStatus(String dateStr, String dateRel) {
	    DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd-MMM-yyyy HH:mm:ss", Locale.ENGLISH);
	    LocalDate today = LocalDate.now();

	    LocalDate inputDate;
	    try {
	        LocalDateTime inputDateTime = LocalDateTime.parse(dateStr, formatter);
	        inputDate = inputDateTime.toLocalDate();
	    } catch (Exception e) {
	        return "Expiry date format invalid";
	    }

	    // Case 1: Already expired
	    if (inputDate.isBefore(today)) {
	        return "Expired";  // Only expiry, no renewal check here
	    }

	    // Case 2: Expiring in 0–12 months
	    LocalDate threeMonthsLater = today.plusMonths(3);
	    LocalDate sixMonthsLater = today.plusMonths(6);
	    LocalDate twelveMonthsLater = today.plusMonths(12);

	    if (!inputDate.isAfter(threeMonthsLater)) {
	        return "Expiring in 0-3 months";
	    } else if (!inputDate.isAfter(sixMonthsLater)) {
	        return "Expiring in 3-6 months";
	    } else if (!inputDate.isAfter(twelveMonthsLater)) {
	        return "Expiring in 6-12 months";
	    }

	    // Case 3: Expiry > 12 months → check renewal only here
	    try {
	        LocalDate relDate = LocalDateTime.parse(dateRel, formatter).toLocalDate();
	        LocalDate threeMonthsAgo = today.minusMonths(3);
	        if (!relDate.isAfter(today) && !relDate.isBefore(threeMonthsAgo)) {
	            return "Renewed 3 months ago";
	        }
	    } catch (Exception ignore) {
	        // invalid/missing renewal → ignore
	    }

	    return null;
	}

	private static Set<String> getEmails(ArrayList<User> users) {
		Set<String> emails = new HashSet<>();
		try {
			// Prepare array of Person objects (for batch property loading)
			Person[] persons = new Person[users.size()];
			for (int i = 0; i < users.size(); i++) {
				persons[i] = users.get(i).get_person();
			}

			// Batch fetch "PA9" property on all Person objects at once
			Connect.BADataManagementSevice.getProperties(persons, new String[] { "PA9" });

			// Now extract email values after loading
			for (int i = 0; i < users.size(); i++) {
				try {
					String email = persons[i].get_PA9();
					if (email != null && !email.isEmpty()) {
						emails.add(email.trim()); // add trimmed email to Set to avoid duplicates
					}
				} catch (NotLoadedException e) {
					e.printStackTrace();
					// Decide if you want to handle missing email differently
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
			// Optionally handle or return empty set on error
		}
		return emails;
	}

	public static Map<String, Map<String, Set<String>>> getAllMaps() {
		Map<String, Map<String, Set<String>>> result = new HashMap<>();
		result.put("Expired", expired);
		result.put("Expiring in 0-3 months", expiringIn3months);
		result.put("Expiring in 3-6 months", expiringIn6months);
		result.put("Expiring in 6-12 months", expiringIn1Year);
		result.put("Renewed 3 months ago", renewed3monthsago);
		return result;
	}
}