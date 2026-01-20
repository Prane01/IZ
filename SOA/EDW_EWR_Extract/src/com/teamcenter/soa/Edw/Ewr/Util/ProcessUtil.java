package com.teamcenter.soa.Edw.Ewr.Util;

import java.io.FileWriter;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.time.format.DateTimeParseException;

import com.teamcenter.clientx.AppXSession;

import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.soa.Edw.Ewr.EDWDataMgmt;
import com.teamcenter.soa.Edw.Ewr.model.EWRData;
import com.teamcenter.soa.Edw.Ewr.model.SKUdata;
import com.teamcenter.soa.Edw.Ewr.model.TaskData;
import com.teamcenter.soa.Edw.Ewr.model.WFData;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.exceptions.NotLoadedException;


@SuppressWarnings("unused")
public class ProcessUtil {

	public DataManagementService EDWDataManagementSevice = null;
	public DateTimeFormatter inputFormatter = DateTimeFormatter.ofPattern("dd-MMM-yyyy HH:mm:ss");
	public DateTimeFormatter outputFormatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss.SSS");
	public String formatedDate = "";
	public String nullCheck = "";

	public void LoadAndPrintProperties(ModelObject revObject, StringBuilder EDWDataBuilder) {

		EDWDataManagementSevice = EDWDataMgmt.EDWDataManagementSevice;

		try {

			EWRData EWRDataObj = new EWRData();
			List<SKUdata> skuDataList = new ArrayList<SKUdata>();

			EDWDataManagementSevice.getProperties(new ModelObject[] { revObject },
					new String[] { "puid", "item_id", "object_name", "object_type", "creation_date",
							"gov_classification", "h4_Reference_Links", "h4_release_status", "date_released",
							"owning_user", "owning_group", "last_mod_user", "h4_EWR_SW_Only", "h4_sbg",
							"h4_strategic_business_unit", "h4_gold_business_enterprise", "h4_line_of_business",
							"h4_Estimated_Engg_Effort", "h4_Product_Category", "h4_Product_Family",
							"h4_EWR_Origin_Region", "h4_EWR_Factory_Site", "h4_EWR_Status", "h4_Assigned_Engineer",
							"h4_EWR_Reviewer", "h4_Reqs_Completion_Date", "h4_Revenue_Impact", "h4_Margin_Impact",
							"h4_Customer_Contact", "h4_BOM_Change_Customer_Req", "h4_Contract_Number",
							"h4_Reason_for_Notification", "h4_accolade_project_tr", "h4_Request_Category",
							"h4_Request_Sub_Category", "h4_Owning_Function", "h4_Effort", "h4_Opportunity",
							"h4_Strategic_Fit", "h4_Rev_Impact", "h4_Severity_Frequency", "h4_Cust_Response_Field_Imp",
							"h4_EWRPriorityScore", "h4_EWRCOPQ", "h4_COPQ_Field_Impact", "h4_EWRYield_loss",
							"h4_EWRDPMO", "h4_Latest_Comments", "h4_Issue_Description", "h4_Business_Just_Impact", "h4_Changes_Required", "h4_Risks", "owning_project", "fnd0AllWorkflows" });

			EWRDataObj.setStrPUID(escapeDoubleQuotes(revObject.getUid()));
			EWRDataObj.setStrItemID(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("item_id")));
			EWRDataObj.setStrObjName(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("object_name")));
			EWRDataObj.setStrObjType(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("object_type")));
			formatedDate = revObject.getPropertyDisplayableValue("creation_date");

			if (formatedDate != null && formatedDate.length() > 0) {
				formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
			}
			EWRDataObj.setStrCreationDate(formatedDate);
			EWRDataObj.setStrGovClassification(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("gov_classification")));
			EWRDataObj.setStrRefLinks(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Reference_Links")));
			EWRDataObj.setStrReleaseStatus(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_release_status")));

			formatedDate = revObject.getPropertyDisplayableValue("date_released");
			if (formatedDate != null && formatedDate.length() > 0) {
				formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
			}
			EWRDataObj.setStrDateReleased(formatedDate);
			EWRDataObj.setStrOwningUser(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("owning_user")));
			EWRDataObj.setStrOwningGrp(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("owning_group")));
			EWRDataObj.setStrLastModUser(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("last_mod_user")));

			String bValue = revObject.getPropertyDisplayableValue("h4_EWR_SW_Only");
			if (bValue.equalsIgnoreCase("False") || bValue.equalsIgnoreCase("") || bValue == null) {
				EWRDataObj.setStrEWRSWOnly("0");
			} else if (bValue.equalsIgnoreCase("True")) {
				EWRDataObj.setStrEWRSWOnly("1");
			}

			EWRDataObj.setStrSBG(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_sbg")));
			EWRDataObj
					.setStrSBU(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_strategic_business_unit")));
			EWRDataObj.setStrGoldBusEnterprise(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_gold_business_enterprise")));
			EWRDataObj.setStrLOB(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_line_of_business")));

			EWRDataObj.setStrEstimatedEngEffort(
					nullCheckInt(revObject.getPropertyDisplayableValue("h4_Estimated_Engg_Effort")));

			EWRDataObj.setStrProdCategory(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Product_Category")));
			EWRDataObj.setStrProdFamily(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Product_Family")));
			EWRDataObj.setStrEWROriginReg(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_EWR_Origin_Region")));
			EWRDataObj.setStrEWRfactorySite(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_EWR_Factory_Site")));
			EWRDataObj.setStrEWRStatus(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_EWR_Status")));
			EWRDataObj.setStrAssignedEngineer(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Assigned_Engineer")));
			List<String> reviewerList = revObject.getPropertyDisplayableValues("h4_EWR_Reviewer");

			EWRDataObj.setStrEWRReviewer(String.join(";", reviewerList));

			formatedDate = revObject.getPropertyDisplayableValue("h4_Reqs_Completion_Date");
			if (formatedDate != null && formatedDate.length() > 0) {
				formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
			}
			EWRDataObj.setStrReqCompDate(formatedDate);

			EWRDataObj.setStrRevenueImpact(nullCheckInt(revObject.getPropertyDisplayableValue("h4_Revenue_Impact")));

			EWRDataObj.setStrMarginImpact(nullCheckInt(revObject.getPropertyDisplayableValue("h4_Margin_Impact")));

			EWRDataObj.setStrCustomerContact(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Customer_Contact")));
			EWRDataObj.setStrBOMchangeCustomerReq(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_BOM_Change_Customer_Req")));
			EWRDataObj
					.setStrContractNum(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Contract_Number")));
			EWRDataObj.setStrReasonForNotif(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Reason_for_Notification")));
			ModelObject propObject = revObject.getPropertyObject("h4_accolade_project_tr").getModelObjectValue();

			if (propObject != null) {
				EDWDataManagementSevice.getProperties(new ModelObject[] { propObject },
						new String[] { "h4_project_id_name" });

				EWRDataObj.setStrAccoladePrjTR(
						escapeDoubleQuotes(propObject.getPropertyDisplayableValue("h4_project_id_name")));
			}

			EWRDataObj.setStrReqstCategory(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Request_Category")));
			EWRDataObj.setStrReqstSubCategory(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Request_Sub_Category")));
			EWRDataObj.setStrOwningFunction(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Owning_Function")));

			EWRDataObj.setStrEffort(nullCheckInt(revObject.getPropertyDisplayableValue("h4_Effort")));

			EWRDataObj.setStrOppurtunity(nullCheckInt(revObject.getPropertyDisplayableValue("h4_Opportunity")));

			EWRDataObj.setStrStrategicFit(nullCheckInt(revObject.getPropertyDisplayableValue("h4_Strategic_Fit")));

			EWRDataObj.setStrRevImpact(nullCheckInt(revObject.getPropertyDisplayableValue("h4_Rev_Impact")));

			EWRDataObj.setStrSeverityFreq(nullCheckInt(revObject.getPropertyDisplayableValue("h4_Severity_Frequency")));
			EWRDataObj.setStrCustRespoFieldImp(
					nullCheckInt(revObject.getPropertyDisplayableValue("h4_Cust_Response_Field_Imp")));
			EWRDataObj.setStrEWRpriortyScore(nullCheckInt(Integer.toString(get_priority_score(revObject)))); // function
																												// returns
			EWRDataObj.setStrEwrCOPQ(nullCheckInt(revObject.getPropertyDisplayableValue("h4_EWRCOPQ")));
			EWRDataObj
					.setStrCOPQfieldImpact(nullCheckInt(revObject.getPropertyDisplayableValue("h4_COPQ_Field_Impact")));
			EWRDataObj.setStrEWRYieldLoss(nullCheckInt(revObject.getPropertyDisplayableValue("h4_EWRYield_loss")));
			EWRDataObj.setStrEwrDPMO(nullCheckInt(revObject.getPropertyDisplayableValue("h4_EWRDPMO")));
			EWRDataObj.setStrLatestComments(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Latest_Comments")));
			EWRDataObj.setStrIssueDescription(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Issue_Description")));
			EWRDataObj.setStrBusinessJustImpact(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Business_Just_Impact")));
			EWRDataObj.setStrChangesRequired(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Changes_Required")));
			EWRDataObj.setStrRisks(escapeDoubleQuotes(revObject.getPropertyDisplayableValue("h4_Risks")));
			EWRDataObj.setStrOwningProject(
					escapeDoubleQuotes(revObject.getPropertyDisplayableValue("owning_project")));
			skuDataList = getSKUDetails(revObject);

			ModelObject[] workflows = revObject.getPropertyObject("fnd0AllWorkflows").getModelObjectArrayValue();

			if (workflows.length > 0) {

				EDWDataManagementSevice.getProperties(workflows,
						new String[] { "object_name", "job_name", "task_template", "fnd0TaskPathName", "task_result",
								"state", "comments", "fnd0Priority", "fnd0StartDate", "fnd0EndDate", "due_date",
								"h4_escalation_level", "the_task_duration", "creation_date", "last_mod_date",
								"owning_user", "child_tasks", "fnd0WorkflowInitiator" });

				for (ModelObject WFObject : workflows) {

					String strWorkflowName = escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("object_name"));

					if (strWorkflowName.equalsIgnoreCase("HON EWR WorkFlow")) {

						WFData wfDataObj = new WFData();

						wfDataObj.setStrWFJobName(escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("job_name")));

						ModelObject objs = WFObject.getPropertyObject("task_template").getModelObjectValue();
						EDWDataManagementSevice.getProperties(new ModelObject[] { objs },
								new String[] { "object_string" });

						wfDataObj.setStrWFTaskTemp(objs.getPropertyDisplayableValue("object_string"));
						wfDataObj.setStrWFTaskPathName(
								escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("fnd0TaskPathName")));
						wfDataObj.setStrWFTaskResult(
								escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("task_result")));
						wfDataObj.setStrWFState(escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("state")));
						wfDataObj
								.setStrWFComments(escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("comments")));
						wfDataObj.setStrWFPriority(
								escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("fnd0Priority")));

						formatedDate = WFObject.getPropertyDisplayableValue("fnd0StartDate");
						if (formatedDate != null && formatedDate.length() > 0) {
							formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
						}
						wfDataObj.setStrWFstartedDate(formatedDate);
						formatedDate = WFObject.getPropertyDisplayableValue("fnd0EndDate");
						if (formatedDate != null && formatedDate.length() > 0) {
							formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
						}
						wfDataObj.setStrWFEndDate(formatedDate);
						formatedDate = WFObject.getPropertyDisplayableValue("due_date");
						if (formatedDate != null && formatedDate.length() > 0) {
							formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
						}
						wfDataObj.setStrWFDueDate(formatedDate);
						wfDataObj.setStrWFEsclationLevel(
								escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("h4_escalation_level")));
						wfDataObj.setStrWFTaskDuration(
								escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("the_task_duration")));

						formatedDate = WFObject.getPropertyDisplayableValue("creation_date");
						if (formatedDate != null && formatedDate.length() > 0) {
							formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
						}
						wfDataObj.setStrWFCreationDate(formatedDate);

						formatedDate = WFObject.getPropertyDisplayableValue("last_mod_date");
						if (formatedDate != null && formatedDate.length() > 0) {
							formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
						}
						wfDataObj.setStrWFLastModDate(formatedDate);
						wfDataObj.setStrWFOwningUser(
								escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("owning_user")));
						wfDataObj.setStrWFInitiator(
								escapeDoubleQuotes(WFObject.getPropertyDisplayableValue("fnd0WorkflowInitiator")));

						ModelObject[] childTasks = WFObject.getPropertyObject("child_tasks").getModelObjectArrayValue();

						if (childTasks.length > 0) {

							EDWDataManagementSevice.getProperties(childTasks,
									new String[] { "task_template", "fnd0TaskPathName", "task_result", "state",
											"comments", "fnd0Priority", "fnd0StartDate", "fnd0EndDate", "due_date",
											"h4_escalation_level", "the_task_duration", "creation_date",
											"last_mod_date", "task_type", "resp_party", "fnd0Performer" });

							for (ModelObject taskModelObject : childTasks) {

								TaskData taskDataObj = new TaskData();

								ModelObject objs1 = taskModelObject.getPropertyObject("task_template")
										.getModelObjectValue();
								EDWDataManagementSevice.getProperties(new ModelObject[] { objs1 },
										new String[] { "object_string" });

								taskDataObj.setStrTaskTaskTemp(
										escapeDoubleQuotes(objs1.getPropertyDisplayableValue("object_string")));
								taskDataObj.setStrTaskPathName(escapeDoubleQuotes(
										taskModelObject.getPropertyDisplayableValue("fnd0TaskPathName")));
								taskDataObj.setStrTaskResult(
										escapeDoubleQuotes(taskModelObject.getPropertyDisplayableValue("task_result")));
								taskDataObj.setStrTaskState(
										escapeDoubleQuotes(taskModelObject.getPropertyDisplayableValue("state")));
								taskDataObj.setStrTaskCOmments(
										escapeDoubleQuotes(taskModelObject.getPropertyDisplayableValue("comments")));
								taskDataObj.setStrTaskPriority(escapeDoubleQuotes(
										taskModelObject.getPropertyDisplayableValue("fnd0Priority")));

								formatedDate = taskModelObject.getPropertyDisplayableValue("fnd0StartDate");
								if (formatedDate != null && formatedDate.length() > 0) {
									formatedDate = LocalDateTime.parse(formatedDate, inputFormatter)
											.format(outputFormatter);
								}
								taskDataObj.setStrTaskStartDate(formatedDate);

								formatedDate = taskModelObject.getPropertyDisplayableValue("fnd0EndDate");
								if (formatedDate != null && formatedDate.length() > 0) {
									formatedDate = LocalDateTime.parse(formatedDate, inputFormatter)
											.format(outputFormatter);
								}
								taskDataObj.setStrTaskEndDate(formatedDate);

								formatedDate = taskModelObject.getPropertyDisplayableValue("due_date");
								if (formatedDate != null && formatedDate.length() > 0) {
									formatedDate = LocalDateTime.parse(formatedDate, inputFormatter)
											.format(outputFormatter);
								}
								taskDataObj.setStrTaskDueDate(formatedDate);

								taskDataObj.setStrTaskEsclLevel(escapeDoubleQuotes(
										taskModelObject.getPropertyDisplayableValue("h4_escalation_level")));
								taskDataObj.setStrTaskDuration(escapeDoubleQuotes(
										taskModelObject.getPropertyDisplayableValue("the_task_duration")));

								formatedDate = taskModelObject.getPropertyDisplayableValue("creation_date");
								if (formatedDate != null && formatedDate.length() > 0) {
									formatedDate = LocalDateTime.parse(formatedDate, inputFormatter)
											.format(outputFormatter);
								}
								taskDataObj.setStrTaskCreationDate(formatedDate);

								formatedDate = taskModelObject.getPropertyDisplayableValue("last_mod_date");
								if (formatedDate != null && formatedDate.length() > 0) {
									formatedDate = LocalDateTime.parse(formatedDate, inputFormatter)
											.format(outputFormatter);
								}
								taskDataObj.setStrTaskLastModDate(formatedDate);
								taskDataObj.setStrTaskType(
										escapeDoubleQuotes(taskModelObject.getPropertyDisplayableValue("task_type")));
								taskDataObj.setStrTaskRespParty(
										escapeDoubleQuotes(taskModelObject.getPropertyDisplayableValue("resp_party")));
								taskDataObj.setStrTaskPerfomer(escapeDoubleQuotes(
										taskModelObject.getPropertyDisplayableValue("fnd0Performer")));
								if (skuDataList != null && !skuDataList.isEmpty()) {
									if (skuDataList.size() > 0) {

										for (int i = 0; i < skuDataList.size(); i++) {

											appendEwr(EWRDataObj, EDWDataBuilder);

											// each SKU properties
											EDWDataBuilder.append("\"").append(skuDataList.get(i).getStrSKUItemID())
													.append("\",").append("\"")
													.append(skuDataList.get(i).getStrSKUObjName()).append("\",")
													.append("\"").append(skuDataList.get(i).getStrSKUAccoladeCombIDs())
													.append("\",").append("\"")
													.append(skuDataList.get(i).getStrSKUAccoladeProjTR()).append("\",")
													.append("\"").append(wfDataObj.getStrWFJobName()).append("\",")
													.append("\"").append(wfDataObj.getStrWFTaskTemp()).append("\",")
													.append("\"").append(wfDataObj.getStrWFTaskPathName()).append("\",")
													.append("\"").append(wfDataObj.getStrWFTaskResult()).append("\",")
													.append("\"").append(wfDataObj.getStrWFState()).append("\",")
													.append("\"").append(wfDataObj.getStrWFComments()).append("\",")
													.append("\"").append(wfDataObj.getStrWFPriority()).append("\",")
													.append("\"").append(wfDataObj.getStrWFstartedDate()).append("\",")
													.append("\"").append(wfDataObj.getStrWFEndDate()).append("\",")
													.append("\"").append(wfDataObj.getStrWFDueDate()).append("\",")
													.append("\"").append(wfDataObj.getStrWFEsclationLevel())
													.append("\",").append("\"").append(wfDataObj.getStrWFTaskDuration())
													.append("\",").append("\"").append(wfDataObj.getStrWFCreationDate())
													.append("\",").append("\"").append(wfDataObj.getStrWFLastModDate())
													.append("\",").append("\"").append(wfDataObj.getStrWFOwningUser())
													.append("\",").append("\"").append(wfDataObj.getStrWFInitiator())
													.append("\",").append("\"").append(taskDataObj.getStrTaskTaskTemp())
													.append("\",").append("\"").append(taskDataObj.getStrTaskPathName())
													.append("\",").append("\"").append(taskDataObj.getStrTaskResult())
													.append("\",").append("\"").append(taskDataObj.getStrTaskState())
													.append("\",").append("\"").append(taskDataObj.getStrTaskCOmments())
													.append("\",").append("\"").append(taskDataObj.getStrTaskPriority())
													.append("\",").append("\"").append(taskDataObj.getStrTaskStartDate())
													.append("\",").append("\"").append(taskDataObj.getStrTaskEndDate())
													.append("\",").append("\"").append(taskDataObj.getStrTaskDueDate())
													.append("\",").append("\"").append(taskDataObj.getStrTaskEsclLevel())
													.append("\",").append("\"").append(taskDataObj.getStrTaskDuration())
													.append("\",").append("\"")
													.append(taskDataObj.getStrTaskCreationDate()).append("\",")
													.append("\"").append(taskDataObj.getStrTaskLastModDate()).append("\",")
													.append("\"").append(taskDataObj.getStrTaskType()).append("\",")
													.append("\"").append(taskDataObj.getStrTaskRespParty()).append("\",")
													.append("\"").append(taskDataObj.getStrTaskPerfomer()).append("\"")
													.append("\n");

											if (taskDataObj.getStrTaskType().equalsIgnoreCase("EPMReviewTask")) {

												EDWDataManagementSevice.getProperties(
														new ModelObject[] { taskModelObject },
														new String[] { "child_tasks" });

												ModelObject[] subChildTasks = taskModelObject
														.getPropertyObject("child_tasks").getModelObjectArrayValue();

												for (ModelObject subTask : subChildTasks) {

													printSubChildTasks(subTask, wfDataObj, skuDataList, EWRDataObj,
															EDWDataBuilder);

												}
											}

										}
									}
								} else {
									appendEwr(EWRDataObj, EDWDataBuilder);// ewr

									EDWDataBuilder.append("\"").append("").append("\",").append("\"").append("")
											.append("\",").append("\"").append("").append("\",").append("\"").append("")
											.append("\","); // for SKU

									EDWDataBuilder.append("\"").append(wfDataObj.getStrWFJobName()).append("\",")// WF
											.append("\"").append(wfDataObj.getStrWFTaskTemp()).append("\",")
											.append("\"").append(wfDataObj.getStrWFTaskPathName()).append("\",")
											.append("\"").append(wfDataObj.getStrWFTaskResult()).append("\",")
											.append("\"").append(wfDataObj.getStrWFState()).append("\",").append("\"")
											.append(wfDataObj.getStrWFComments()).append("\",").append("\"")
											.append(wfDataObj.getStrWFPriority()).append("\",").append("\"")
											.append(wfDataObj.getStrWFstartedDate()).append("\",").append("\"")
											.append(wfDataObj.getStrWFEndDate()).append("\",").append("\"")
											.append(wfDataObj.getStrWFDueDate()).append("\",").append("\"")
											.append(wfDataObj.getStrWFEsclationLevel()).append("\",").append("\"")
											.append(wfDataObj.getStrWFTaskDuration()).append("\",").append("\"")
											.append(wfDataObj.getStrWFCreationDate()).append("\",").append("\"")
											.append(wfDataObj.getStrWFLastModDate()).append("\",").append("\"")
											.append(wfDataObj.getStrWFOwningUser()).append("\",").append("\"").append(wfDataObj.getStrWFInitiator()).append("\",");

									EDWDataBuilder.append("\"").append(taskDataObj.getStrTaskTaskTemp()).append("\",")
											.append("\"").append(taskDataObj.getStrTaskPathName()).append("\",")
											.append("\"").append(taskDataObj.getStrTaskResult()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskState()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskCOmments()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskPriority()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskStartDate()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskEndDate()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskDueDate()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskEsclLevel()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskDuration()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskCreationDate()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskLastModDate()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskType()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskRespParty()).append("\",").append("\"")
											.append(taskDataObj.getStrTaskPerfomer()).append("\"").append("\n");

									if (taskDataObj.getStrTaskType().equalsIgnoreCase("EPMReviewTask")) {

										EDWDataManagementSevice.getProperties(new ModelObject[] { taskModelObject },
												new String[] { "child_tasks" });

										ModelObject[] subChildTasks = taskModelObject.getPropertyObject("child_tasks")
												.getModelObjectArrayValue();

										for (ModelObject subTask : subChildTasks) {

											printSubChildTasks(subTask, wfDataObj, skuDataList, EWRDataObj,
													EDWDataBuilder);

										}
									}

								}

							}

						}

					}

				}
			} else {
				if (skuDataList != null && !skuDataList.isEmpty()) {
					if (skuDataList.size() > 0) {

						for (int i = 0; i < skuDataList.size(); i++) {

							appendEwr(EWRDataObj, EDWDataBuilder);

							// each SKU properties
							EDWDataBuilder.append("\"").append(skuDataList.get(i).getStrSKUItemID()).append("\",");
							EDWDataBuilder.append("\"").append(skuDataList.get(i).getStrSKUObjName()).append("\",");
							EDWDataBuilder.append("\"").append(skuDataList.get(i).getStrSKUAccoladeCombIDs())
									.append("\",");
							EDWDataBuilder.append("\"").append(skuDataList.get(i).getStrSKUAccoladeProjTR())
									.append("\",");

							// Appending WF and TaskProps
							EDWDataBuilder.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",");

							EDWDataBuilder.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\",")
									.append("\"").append("").append("\",").append("\"").append("").append("\"")
									.append("\n");

						}

					}
				} else {
					appendEwr(EWRDataObj, EDWDataBuilder);

					// each SKU properties
					EDWDataBuilder.append("\"").append("").append("\",").append("\"").append("").append("\",")
							.append("\"").append("").append("\",").append("\"").append("").append("\",");

					// Appending WF and TaskProps
					EDWDataBuilder.append("\"").append("").append("\",").append("\"").append("").append("\",")
							.append("\"").append("").append("\",").append("\"").append("").append("\",").append("\"")
							.append("").append("\",").append("\"").append("").append("\",").append("\"").append("")
							.append("\",").append("\"").append("").append("\",").append("\"").append("").append("\",")
							.append("\"").append("").append("\",").append("\"").append("").append("\",").append("\"")
							.append("").append("\",").append("\"").append("").append("\",").append("\"").append("")
							.append("\",").append("\"").append("").append("\",").append("\"").append("").append("\",");

					EDWDataBuilder.append("\"").append("").append("\",").append("\"").append("").append("\",")
							.append("\"").append("").append("\",").append("\"").append("").append("\",").append("\"")
							.append("").append("\",").append("\"").append("").append("\",").append("\"").append("")
							.append("\",").append("\"").append("").append("\",").append("\"").append("").append("\",")
							.append("\"").append("").append("\",").append("\"").append("").append("\",").append("\"")
							.append("").append("\",").append("\"").append("").append("\",").append("\"").append("")
							.append("\",").append("\"").append("").append("\",").append("\"").append("").append("\"")
							.append("\n");

				}

			}
		} catch (Exception e) {
			e.printStackTrace();
			EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR",
					"EDW EWR EXTARCTION APPLICATION is failed with error meaasge  :[" + e.getMessage() + "]...\n"));
		}

	}

	public String escapeDoubleQuotes(String input) {
		String out = "";
		if (input == null || input.isEmpty()) {
			return out;
		}
		out = input.replace("\n", " ").replace("\"", "");
		return out;
	}

	public String nullCheckInt(String nullCheck) {

		if (nullCheck.equalsIgnoreCase("") || nullCheck == null) {
			return "0";
		} else {
			return nullCheck;
		}
	}

	public void printSubChildTasks(ModelObject subTask, WFData wfData, List<SKUdata> skuData, EWRData ewrData,
			StringBuilder eDWDataBuilder) {

		EDWDataManagementSevice.getProperties(new ModelObject[] { subTask },
				new String[] { "task_template", "fnd0TaskPathName", "task_result", "state", "comments", "fnd0Priority",
						"fnd0StartDate", "fnd0EndDate", "due_date", "h4_escalation_level", "the_task_duration",
						"creation_date", "last_mod_date", "task_type", "resp_party", "fnd0Performer" });

		try {

			ModelObject objs = subTask.getPropertyObject("task_template").getModelObjectValue();
			EDWDataManagementSevice.getProperties(new ModelObject[] { objs }, new String[] { "object_string" });
			String taskTemp = escapeDoubleQuotes(objs.getPropertyDisplayableValue("object_string"));
			String taskPathName = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("fnd0TaskPathName"));
			String taskResult = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("task_result"));
			String taskState = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("state"));

			String priority = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("fnd0Priority"));

			formatedDate = subTask.getPropertyDisplayableValue("fnd0StartDate");
			if (formatedDate != null && formatedDate.length() > 0) {
				formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
			}
			String startDate = formatedDate;

			formatedDate = subTask.getPropertyDisplayableValue("fnd0EndDate");
			if (formatedDate != null && formatedDate.length() > 0) {
				formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
			}
			String EndDate = formatedDate;

			formatedDate = subTask.getPropertyDisplayableValue("due_date");
			if (formatedDate != null && formatedDate.length() > 0) {
				formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
			}
			String dueDate = formatedDate;

			String esclationLev = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("h4_escalation_level"));
			String taskDuration = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("the_task_duration"));

			formatedDate = subTask.getPropertyDisplayableValue("creation_date");
			if (formatedDate != null && formatedDate.length() > 0) {
				formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
			}
			String creationDate = formatedDate;

			formatedDate = subTask.getPropertyDisplayableValue("last_mod_date");
			if (formatedDate != null && formatedDate.length() > 0) {
				formatedDate = LocalDateTime.parse(formatedDate, inputFormatter).format(outputFormatter);
			}
			String lastModDate = formatedDate;

			String TaskType = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("task_type"));
			String RespParty = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("resp_party"));
			String comment = null;
			String perfomer = null;

			if (TaskType.equalsIgnoreCase("EPMPerformSignoffTask")) {

				EDWDataManagementSevice.getProperties(new ModelObject[] { subTask }, new String[] { "valid_signoffs" });

				ModelObject[] modelObjectArrayValue = subTask.getPropertyObject("valid_signoffs")
						.getModelObjectArrayValue();

				if (modelObjectArrayValue.length > 0) {

					for (ModelObject object : modelObjectArrayValue) {

						EDWDataManagementSevice.getProperties(modelObjectArrayValue, new String[] { "comments" });
						comment = escapeDoubleQuotes(object.getPropertyDisplayableValue("comments"));
					}

				}
				perfomer = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("valid_signoffs"));
				perfomer = perfomer.split("-")[0].trim();
			} else {
				perfomer = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("fnd0Performer"));
				comment = escapeDoubleQuotes(subTask.getPropertyDisplayableValue("comments"));
			}

			appendEwr(ewrData, eDWDataBuilder);// ewr

			eDWDataBuilder.append("\"").append("").append("\",").append("\"").append("").append("\",").append("\"")
					.append("").append("\",").append("\"").append("").append("\",").append("\"").append(wfData.getStrWFJobName())
					.append("\",")// WF
					.append("\"").append(wfData.getStrWFTaskTemp()).append("\",").append("\"")
					.append(wfData.getStrWFTaskPathName()).append("\",").append("\"")
					.append(wfData.getStrWFTaskResult()).append("\",").append("\"").append(wfData.getStrWFState())
					.append("\",").append("\"").append(wfData.getStrWFComments()).append("\",").append("\"")
					.append(wfData.getStrWFPriority()).append("\",").append("\"").append(wfData.getStrWFstartedDate())
					.append("\",").append("\"").append(wfData.getStrWFEndDate()).append("\",").append("\"")
					.append(wfData.getStrWFDueDate()).append("\",").append("\"").append(wfData.getStrWFEsclationLevel())
					.append("\",").append("\"").append(wfData.getStrWFTaskDuration()).append("\",").append("\"")
					.append(wfData.getStrWFCreationDate()).append("\",").append("\"")
					.append(wfData.getStrWFLastModDate()).append("\",").append("\"").append(wfData.getStrWFOwningUser())
					.append("\",").append("\"").append(wfData.getStrWFInitiator())
					.append("\",").append("\"").append(taskTemp).append("\",").append("\"").append(taskPathName)
					.append("\",").append("\"").append(taskResult).append("\",").append("\"").append(taskState)
					.append("\",").append("\"").append(comment).append("\",").append("\"").append(priority)
					.append("\",").append("\"").append(startDate).append("\",").append("\"").append(EndDate)
					.append("\",").append("\"").append(dueDate).append("\",").append("\"").append(esclationLev)
					.append("\",").append("\"").append(taskDuration).append("\",").append("\"").append(creationDate)
					.append("\",").append("\"").append(lastModDate).append("\",").append("\"").append(TaskType)
					.append("\",").append("\"").append(RespParty).append("\",").append("\"").append(perfomer)
					.append("\"").append("\n");

		} catch (NotLoadedException e) {
			e.printStackTrace();
		}

	}

	public List<SKUdata> getSKUDetails(ModelObject revObject) {

		try {

			EDWDataManagementSevice.getProperties(new ModelObject[] { revObject },
					new String[] { "CMHasImpactedItem" });

			List<SKUdata> dSkUdata = new ArrayList<SKUdata>();
			ModelObject[] secObjs = revObject.getPropertyObject("CMHasImpactedItem").getModelObjectArrayValue();

			EDWDataManagementSevice.getProperties(secObjs,
					new String[] { "item_id", "object_name", "h4_accolade_combined_ids", "h4_accolade_project_tr" });
			if (secObjs.length > 0) {
				for (ModelObject object : secObjs) {

					SKUdata skUdata = new SKUdata();
					skUdata.setStrSKUItemID(object.getPropertyDisplayableValue("item_id"));
					skUdata.setStrSKUObjName(escapeDoubleQuotes(object.getPropertyDisplayableValue("object_name")));
					skUdata.setStrSKUAccoladeCombIDs(object.getPropertyDisplayableValue("h4_accolade_combined_ids"));
					skUdata.setStrSKUAccoladeProjTR(object.getPropertyDisplayableValue("h4_accolade_project_tr"));
					dSkUdata.add(skUdata);
				}
				return dSkUdata;
			}

		} catch (Exception e) {
			e.printStackTrace();
		}

		return null;
	}

	public void appendEwr(EWRData ewrData, StringBuilder eDWDataBuilder) {

		eDWDataBuilder.append("\"").append(ewrData.getStrPUID()).append("\",").append("\"")
				.append(ewrData.getStrItemID()).append("\",").append("\"").append(ewrData.getStrObjName()).append("\",")
				.append("\"").append(ewrData.getStrObjType()).append("\",").append("\"")
				.append(ewrData.getStrCreationDate()).append("\",").append("\"")
				.append(ewrData.getStrGovClassification()).append("\",").append("\"").append(ewrData.getStrRefLinks())
				.append("\",").append("\"").append(ewrData.getStrReleaseStatus()).append("\",").append("\"")
				.append(ewrData.getStrDateReleased()).append("\",").append("\"").append(ewrData.getStrOwningUser())
				.append("\",").append("\"").append(ewrData.getStrOwningGrp()).append("\",").append("\"")
				.append(ewrData.getStrLastModUser()).append("\",").append("\"").append(ewrData.getStrEWRSWOnly())
				.append("\",").append("\"").append(ewrData.getStrSBG()).append("\",").append("\"")
				.append(ewrData.getStrSBU()).append("\",").append("\"").append(ewrData.getStrGoldBusEnterprise())
				.append("\",").append("\"").append(ewrData.getStrLOB()).append("\",").append("\"")
				.append(ewrData.getStrEstimatedEngEffort()).append("\",").append("\"")
				.append(ewrData.getStrProdCategory()).append("\",").append("\"").append(ewrData.getStrProdFamily())
				.append("\",").append("\"").append(ewrData.getStrEWROriginReg()).append("\",").append("\"")
				.append(ewrData.getStrEWRfactorySite()).append("\",").append("\"").append(ewrData.getStrEWRStatus())
				.append("\",").append("\"").append(ewrData.getStrAssignedEngineer()).append("\",").append("\"")
				.append(ewrData.getStrEWRReviewer()).append("\",").append("\"").append(ewrData.getStrReqCompDate())
				.append("\",").append("\"").append(ewrData.getStrRevenueImpact()).append("\",").append("\"")
				.append(ewrData.getStrMarginImpact()).append("\",").append("\"").append(ewrData.getStrCustomerContact())
				.append("\",").append("\"").append(ewrData.getStrBOMchangeCustomerReq()).append("\",").append("\"")
				.append(ewrData.getStrContractNum()).append("\",").append("\"").append(ewrData.getStrReasonForNotif())
				.append("\",").append("\"").append(ewrData.getStrAccoladePrjTR()).append("\",").append("\"")
				.append(ewrData.getStrReqstCategory()).append("\",").append("\"")
				.append(ewrData.getStrReqstSubCategory()).append("\",").append("\"")
				.append(ewrData.getStrOwningFunction()).append("\",").append("\"").append(ewrData.getStrEffort())
				.append("\",").append("\"").append(ewrData.getStrOppurtunity()).append("\",").append("\"")
				.append(ewrData.getStrStrategicFit()).append("\",").append("\"").append(ewrData.getStrRevImpact())
				.append("\",").append("\"").append(ewrData.getStrSeverityFreq()).append("\",").append("\"")
				.append(ewrData.getStrCustRespoFieldImp()).append("\",").append("\"")
				.append(ewrData.getStrEWRpriortyScore()).append("\",").append("\"").append(ewrData.getStrEwrCOPQ())
				.append("\",").append("\"").append(ewrData.getStrCOPQfieldImpact()).append("\",").append("\"")
				.append(ewrData.getStrEWRYieldLoss()).append("\",").append("\"").append(ewrData.getStrEwrDPMO())
				.append("\",").append("\"").append(ewrData.getStrLatestComments()).append("\",").append("\"").append(ewrData.getStrIssueDescription())
				.append("\",").append("\"").append(ewrData.getStrBusinessJustImpact()).append("\",").append("\"")
				.append(ewrData.getStrChangesRequired()).append("\",").append("\"").append(ewrData.getStrRisks()).append("\",")
				.append("\"").append(ewrData.getStrOwningProject()).append("\",");

	}

	public int get_priority_score(ModelObject revObj) {

		Integer iEWRProp1 = 0;
		Integer iEWRProp2 = 0;
		Integer iEWRProp3 = 0;

		EDWDataManagementSevice.getProperties(new ModelObject[] { revObj },
				new String[] { "h4_Opportunity", "h4_COPQ_Field_Impact", "h4_Severity_Frequency", "h4_Strategic_Fit",
						"h4_Effort", "h4_Request_Category", "h4_Request_Sub_Category", "h4_Owning_Function",
						"h4_Rev_Impact" });

		try {
			String reqCategory = revObj.getPropertyDisplayableValue("h4_Request_Category");
			String reqSubCategory = revObj.getPropertyDisplayableValue("h4_Request_Sub_Category");
			String owningFunction = revObj.getPropertyDisplayableValue("h4_Owning_Function");

			if (reqCategory.equalsIgnoreCase("Regulatory/Agency Compliance")
					&& (reqSubCategory.equalsIgnoreCase("New Agency Regulation/Marketing Request")
							|| reqSubCategory.equalsIgnoreCase("Update Agency Regulation/Directive")
							|| reqSubCategory.equalsIgnoreCase("Agency Certification update")
							|| reqSubCategory.equalsIgnoreCase("New Stewardship Regulation")
							|| reqSubCategory.equalsIgnoreCase("Update Stewardship Regulation")
							|| reqSubCategory.equalsIgnoreCase("Stewardship Certification update"))
					&& (owningFunction.equalsIgnoreCase("Regulatory")
							|| owningFunction.equalsIgnoreCase("Product Stewardship"))) {

				String qString = revObj.getPropertyDisplayableValue("h4_Rev_Impact");
				String wString = revObj.getPropertyDisplayableValue("h4_Effort");
				String eString = revObj.getPropertyDisplayableValue("h4_Strategic_Fit");

				if (qString != null && qString.length() > 0)
					iEWRProp1 = Integer.valueOf(qString);

				if (wString != null && wString.length() > 0)
					iEWRProp2 = Integer.valueOf(wString);

				if (eString != null && eString.length() > 0)
					iEWRProp3 = Integer.valueOf(eString);

			} else if (reqCategory.equalsIgnoreCase("Customer Complaint/Field issue")
					&& (reqSubCategory.equalsIgnoreCase("Severe") || reqSubCategory.equalsIgnoreCase("High")
							|| reqSubCategory.equalsIgnoreCase("Medium") || reqSubCategory.equalsIgnoreCase("Low"))
					&& owningFunction.equalsIgnoreCase("Customer Quality")) {

				String qString = revObj.getPropertyDisplayableValue("h4_Rev_Impact");
				String wString = revObj.getPropertyDisplayableValue("h4_Severity_Frequency");
				String eString = revObj.getPropertyDisplayableValue("h4_Cust_Response_Field_Imp");

				if (qString != null && qString.length() > 0)
					iEWRProp1 = Integer.valueOf(qString);

				if (wString != null && wString.length() > 0)
					iEWRProp2 = Integer.valueOf(wString);

				if (eString != null && eString.length() > 0)
					iEWRProp3 = Integer.valueOf(eString);

			} else if (reqCategory.equalsIgnoreCase("Factory Issue")
					&& (reqSubCategory.equalsIgnoreCase("Quality defects")
							|| reqSubCategory.equalsIgnoreCase("Yield Issues")
							|| reqSubCategory.equalsIgnoreCase("Scrap/Rework")
							|| reqSubCategory.equalsIgnoreCase("Quality Hold"))
					&& owningFunction.equalsIgnoreCase("Site Quality")) {

				String qString = revObj.getPropertyDisplayableValue("h4_Rev_Impact");
				String wString = revObj.getPropertyDisplayableValue("h4_Severity_Frequency");
				String eString = revObj.getPropertyDisplayableValue("h4_COPQ_Field_Impact");

				if (qString != null && qString.length() > 0)
					iEWRProp1 = Integer.valueOf(qString);

				if (wString != null && wString.length() > 0)
					iEWRProp2 = Integer.valueOf(wString);

				if (eString != null && eString.length() > 0)
					iEWRProp3 = Integer.valueOf(eString);

			} else if (reqCategory.equalsIgnoreCase("Supplier Change (non-CE)")
					&& (reqSubCategory.equalsIgnoreCase("Material Specification change")
							|| reqSubCategory.equalsIgnoreCase("Second Source Qualification")
							|| reqSubCategory.equalsIgnoreCase("Localization")
							|| reqSubCategory.equalsIgnoreCase("Material Shortage")
							|| reqSubCategory.equalsIgnoreCase("Alternate Parts")
							|| reqSubCategory.equalsIgnoreCase("Part Obsolescence")
							|| reqSubCategory.equalsIgnoreCase("Tooling changes"))
					&& owningFunction.equalsIgnoreCase("Sourcing")) {

				String qString = revObj.getPropertyDisplayableValue("h4_Rev_Impact");
				String wString = revObj.getPropertyDisplayableValue("h4_Severity_Frequency");
				String eString = revObj.getPropertyDisplayableValue("h4_Effort");

				if (qString != null && qString.length() > 0)
					iEWRProp1 = Integer.valueOf(qString);

				if (wString != null && wString.length() > 0)
					iEWRProp2 = Integer.valueOf(wString);

				if (eString != null && eString.length() > 0)
					iEWRProp3 = Integer.valueOf(eString);

			} else if (reqCategory.equalsIgnoreCase("Re-engineering Requirement")
					&& reqSubCategory.equalsIgnoreCase("Private Labelling")
					&& owningFunction.equalsIgnoreCase("Offering Management")) {

				String qString = revObj.getPropertyDisplayableValue("h4_Effort");
				String wString = revObj.getPropertyDisplayableValue("h4_Opportunity");
				String eString = revObj.getPropertyDisplayableValue("h4_Strategic_Fit");

				if (qString != null && qString.length() > 0)
					iEWRProp1 = Integer.valueOf(qString);

				if (wString != null && wString.length() > 0)
					iEWRProp2 = Integer.valueOf(wString);

				if (eString != null && eString.length() > 0)
					iEWRProp3 = Integer.valueOf(eString);

			} else if (reqCategory.equalsIgnoreCase("Re-engineering Requirement")
					&& reqSubCategory.equalsIgnoreCase("Packaging")
					&& owningFunction.equalsIgnoreCase("Offering Management")) {

				String qString = revObj.getPropertyDisplayableValue("h4_Rev_Impact");
				String wString = revObj.getPropertyDisplayableValue("h4_Effort");
				String eString = revObj.getPropertyDisplayableValue("h4_Strategic_Fit");

				if (qString != null && qString.length() > 0)
					iEWRProp1 = Integer.valueOf(qString);

				if (wString != null && wString.length() > 0)
					iEWRProp2 = Integer.valueOf(wString);

				if (eString != null && eString.length() > 0)
					iEWRProp3 = Integer.valueOf(eString);

			} else if (reqCategory.equalsIgnoreCase("Re-engineering Requirement")
					&& (reqSubCategory.equalsIgnoreCase("BOM/ Drawing errors")
							|| reqSubCategory.equalsIgnoreCase("Literature changes"))
					&& (owningFunction.equalsIgnoreCase("Sustaining Engineering")
							|| owningFunction.equalsIgnoreCase("Offering Management"))) {

				String qString = revObj.getPropertyDisplayableValue("h4_Rev_Impact");
				String wString = revObj.getPropertyDisplayableValue("h4_Severity_Frequency");
				String eString = revObj.getPropertyDisplayableValue("h4_Effort");

				if (qString != null && qString.length() > 0)
					iEWRProp1 = Integer.valueOf(qString);

				if (wString != null && wString.length() > 0)
					iEWRProp2 = Integer.valueOf(wString);

				if (eString != null && eString.length() > 0)
					iEWRProp3 = Integer.valueOf(eString);

			} else if ((reqCategory.equalsIgnoreCase("Product Change Request (PCR)")
					|| reqCategory.equalsIgnoreCase("Internal Repositioning"))
					&& (reqSubCategory.equalsIgnoreCase("Product Change Request")
							|| reqSubCategory.equalsIgnoreCase("Brand Transition"))
					&& owningFunction.equalsIgnoreCase("Offering Management")) {

				String qString = revObj.getPropertyDisplayableValue("h4_Effort");
				String wString = revObj.getPropertyDisplayableValue("h4_Opportunity");
				String eString = revObj.getPropertyDisplayableValue("h4_Strategic_Fit");

				if (qString != null && qString.length() > 0)
					iEWRProp1 = Integer.valueOf(qString);

				if (wString != null && wString.length() > 0)
					iEWRProp2 = Integer.valueOf(wString);

				if (eString != null && eString.length() > 0)
					iEWRProp3 = Integer.valueOf(eString);

			}
		} catch (NotLoadedException e) {

			e.printStackTrace();
			EDWDataMgmt.EDWLogBuilder.append(
					EDWMessageUtil.LogMessage("ERROR", "get_priority_score has errors :[" + e.getMessage() + "]...\n"));

		}

		return iEWRProp1 * iEWRProp2 * iEWRProp3;
	}

}