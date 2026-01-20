package com.teamcenter.soa.datasetexport.util;

import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

import com.teamcenter.services.strong.core.DataManagementService;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.services.strong.query.SavedQueryService;
import com.teamcenter.soa.client.model.strong.ImanQuery;
import com.teamcenter.soa.datasetexport.loader.TestConnect;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.ExecuteSavedQueriesResponse;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.SavedQueryInput;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.SavedQueryResults;
import com.teamcenter.services.strong.query._2010_04.SavedQuery.FindSavedQueriesCriteriaInput;
import com.teamcenter.services.strong.query._2010_04.SavedQuery.FindSavedQueriesResponse;

public class InitiatialPros {

	public String csvFile = "D:\\SOA\\ReadFile\\CreateOrUpdateItemOutput.csv";
	public StringBuilder sb = new StringBuilder();
	
	public InitiatialPros() {
		// TODO Auto-generated constructor stub
	}

	public void initiateProcess(FileWriter writer) {

		try {		
			System.out.println("\nProcess Initiated !!!");
			writer.write("\nProcess Initiated !!!");
			FileWriter writer2 = new FileWriter(csvFile);
			ArrayList<String> updateUID = new ArrayList<String>();
			ArrayList<String> createObject = new ArrayList<String>();

			List<String> lines = Files.readAllLines(Paths.get("D:\\SOA\\ReadFile\\CreateOrUpdateItem.txt"));
			for (String line : lines) {
                System.out.println(line);
            }
			sb.append("ID").append(",").append("Object String").append(",").append("Type").append("\n");
			checkIfObjectExists(lines, writer, writer2, updateUID, createObject);
			writer2.write(sb.toString());
			writer2.close();
			
			System.out.println("*** Count of objects to be Updated ***"+updateUID.size());
			System.out.println("*** Count of objects to be Created ***"+createObject.size());
			
			if(updateUID.size()>0) {
				System.out.println("*** UPDATE ***");
				writer.write("\n*** UPDATING ***");
				UpdateItems.updateObjects(lines, updateUID, writer);
			} 
			if(createObject.size()>0) {
				System.out.println("*** CREATE ***");
				writer.write("\n*** CREATING ***");
				CreateItems.itemsCreate(createObject, writer);
			}
				
//			for(int i=0; i<lines.size(); i++) {
//				sb.append("ID").append(",").append("Object String").append(",").append("Type").append("\n");;
//				boolean isPresent = checkIfObjectExists(lines.get(i), uid, writer);
//				writer2.write(sb.toString());
//				writer2.close();
//				if (isPresent) {
//					System.out.println("*** UPDATE ***");
//					writer.write("\n*** UPDATING ***");
//					UpdateItems.updateObjects(lines.get(i), uid, writer);
//				} 
//				else {
//					System.out.println("*** CREATE ***");
//					writer.write("\n*** CREATING ***");
//					CreateItems.itemsCreate(lines.get(i), writer);
//				}
//			}
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void checkIfObjectExists(List<String> lines, FileWriter writer, FileWriter writer2, ArrayList<String> updateUID, ArrayList<String> createObject) {
		try {
			TestConnect.dmService = DataManagementService.getService(AppXSession.getConnection());
			SavedQueryService sqService = SavedQueryService.getService(AppXSession.getConnection());

			FindSavedQueriesCriteriaInput[] criteriaInput = new FindSavedQueriesCriteriaInput[1];
			criteriaInput[0] = new FindSavedQueriesCriteriaInput();
			criteriaInput[0].queryNames = new String[] { "Item..." };

			FindSavedQueriesResponse savedQryResponse = sqService.findSavedQueries(criteriaInput);
			ImanQuery query = savedQryResponse.savedQueries[0];

			int noOfLines = lines.size();
			
			SavedQueryInput[] qryInput = new SavedQueryInput[noOfLines];
			
			for(int j=0; j<noOfLines; j++) {	
				qryInput[j] = new SavedQueryInput();
				qryInput[j].entries = new String[] { "Item ID" };
				qryInput[j].values = new String[] { lines.get(j) };
				qryInput[j].query = query;
			}

			ExecuteSavedQueriesResponse executeResponse = sqService.executeSavedQueries(qryInput);
			
			ModelObject[] resultModelObjects = null;
			String objectID = "";
			String objectString = "";
			String objectType = "";
						
			if (executeResponse.serviceData.sizeOfPartialErrors() > 0) {
				System.out.println("\nERROR:Failed while Executing a Query"
						+ executeResponse.serviceData.getPartialError(0).getErrorValues()[0].getMessage());
			} else {
				SavedQueryResults[] results = executeResponse.arrayOfResults;
				System.out.println("** No.of executeResponse "+results.length);
				for(int k=0; k<results.length; k++) {
					SavedQueryResults result = results[k];
					if(result.numOfObjects != 0) {
						resultModelObjects = (ModelObject[]) result.objects;
						TestConnect.dmService.getProperties(resultModelObjects, new String[] {"item_id","object_string", "object_type"});
						for(int i=0; i<resultModelObjects.length; i++) {
							objectID = resultModelObjects[i].getPropertyDisplayableValue("item_id");
							System.out.println("** objectID "+objectID);
							objectString = resultModelObjects[i].getPropertyDisplayableValue("object_string");
							System.out.println("** objectString "+objectString);
							objectType = resultModelObjects[i].getPropertyDisplayableValue("object_type");
							System.out.println("** objectType "+objectType);
							sb.append(objectID).append(",").append(objectString).append(",").append(objectType).append("\n");
							updateUID.add(resultModelObjects[i].getUid());
						}
					}
					else {
						System.out.println("result.numOfObjects == 0"+results.toString());
						createObject.add(lines.get(k));
					}
				}
			}
		} catch (Exception e) {
			// TODO: handle exception
		}
	}
	
//	public boolean checkIfObjectExists(String id, StringBuilder uid, FileWriter writer) {
//
//		try {
//			TestConnect.dmService = DataManagementService.getService(AppXSession.getConnection());
//			SavedQueryService sqService = SavedQueryService.getService(AppXSession.getConnection());
//
//			FindSavedQueriesCriteriaInput[] criteriaInput = new FindSavedQueriesCriteriaInput[1];
//			criteriaInput[0] = new FindSavedQueriesCriteriaInput();
//			criteriaInput[0].queryNames = new String[] { "Item..." };
//
//			FindSavedQueriesResponse savedQryResponse = sqService.findSavedQueries(criteriaInput);
//			ImanQuery query = savedQryResponse.savedQueries[0];
//
//			SavedQueryInput[] qryInput = new SavedQueryInput[1];
//			qryInput[0] = new SavedQueryInput();
//			qryInput[0].entries = new String[] { "Item ID" };
//			qryInput[0].values = new String[] { id };
//			qryInput[0].query = query;
//
//			ExecuteSavedQueriesResponse executeResponse = sqService.executeSavedQueries(qryInput);
//			ModelObject[] resultModelObjects = null;
//			String objectID = "";
//			String objectString = "";
//			if (executeResponse.serviceData.sizeOfPartialErrors() > 0) {
//				System.out.println("\nERROR:Failed while Executing a Query"
//						+ executeResponse.serviceData.getPartialError(0).getErrorValues()[0].getMessage());
//			} else {
//				SavedQueryResults[] results = executeResponse.arrayOfResults;
//				
//				for(SavedQueryResults result : results) {
//					resultModelObjects = (ModelObject[]) result.objects;
//				}
//				TestConnect.dmService.getProperties(resultModelObjects, new String[] {"item_id","object_string"});
//				System.out.println("** No.of executeResponse "+results.length);
//				if (results.length > 0) {
//					for(int i=0; i<results.length; i++) {
//						objectID = resultModelObjects[i].getPropertyDisplayableValue("item_id");
//						System.out.println("** objectID "+objectID);
//						objectString = resultModelObjects[i].getPropertyDisplayableValue("object_string");
//						System.out.println("** objectString "+objectString);
//						uid.append(resultModelObjects[i].getUid());
//						return true;
//					}
//					
//				}
//			}
//		} catch (Exception e) {
//			// TODO: handle exception
//		}
//		return false;
//	}
}
