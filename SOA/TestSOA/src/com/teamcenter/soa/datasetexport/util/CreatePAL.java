package com.teamcenter.soa.datasetexport.util;

import java.io.FileWriter;
import java.io.IOException;

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

public class CreatePAL {

	public CreatePAL() {
		// TODO Auto-generated constructor stub
	}

	public void initiateProcess(FileWriter writer) {

		try {

			System.out.println("\nProcess Initiated !!!");
			writer.write("\nProcess Initiated !!!");
			String itemID = "123456";
			StringBuilder uid = new StringBuilder();
			boolean isPresent = checkIfObjectExists(itemID, uid, writer);
			if (isPresent) {
				System.out.println("*** UPDATE ***");
				UpdateItems.updateObjects(itemID, uid, writer);
			} else {
				System.out.println("*** CREATE ***");
				CreateItems.itemsCreate(itemID, writer);
			}

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

//	public boolean checkIfObjectExists(String id,FileWriter writer) {
//
//		try {
//			
//			DataManagementService dmService = DataManagementService.getService(AppXSession.getConnection());
//			SavedQueryService sqService = SavedQueryService.getService(AppXSession.getConnection());
//
//			FindSavedQueriesCriteriaInput[] criteriaInput = new FindSavedQueriesCriteriaInput[1];
//			criteriaInput[0] = new FindSavedQueriesCriteriaInput();
//			criteriaInput[0].queryNames = new String[] { "Item..." };
//
//			FindSavedQueriesResponse savedQryResponse = sqService.findSavedQueries(criteriaInput);
//			ImanQuery query = savedQryResponse.savedQueries[0];
//
//			SavedQueryInput[] qryInput = new SavedQueryInput[2];
//			qryInput[0] = new SavedQueryInput();
//			qryInput[0].entries = new String[] { "Item ID" };
//			qryInput[0].values = new String[] { id };
//			qryInput[0].query = query;
//			
//			qryInput[1] = new SavedQueryInput();
//			qryInput[1].entries = new String[] { "Item ID" };
//			qryInput[1].values = new String[] { id };
//			qryInput[1].query = query;
//			
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
//				dmService.getProperties(resultModelObjects, new String[] {"item_id","object_string"});
//				System.out.println("** No.of executeResponse "+results.length);
//				for(int i=0; i<results.length; i++) {
//					objectID = resultModelObjects[i].getPropertyDisplayableValue("item_id");
//					System.out.println("** objectID "+objectID);
//					objectString = resultModelObjects[i].getPropertyDisplayableValue("object_string");
//					System.out.println("** objectString "+objectString);
//					if(objectID.length()==0) {
//											
//					}
//				}
//				if (results.length > 0) {
//					return true;
//				}
//			}
//		} catch (Exception e) {
//			// TODO: handle exception
//		}
//		return false;
//	}
	
	public boolean checkIfObjectExists(String id, StringBuilder uid, FileWriter writer) {

		try {
			TestConnect.dmService = DataManagementService.getService(AppXSession.getConnection());
			SavedQueryService sqService = SavedQueryService.getService(AppXSession.getConnection());

			FindSavedQueriesCriteriaInput[] criteriaInput = new FindSavedQueriesCriteriaInput[1];
			criteriaInput[0] = new FindSavedQueriesCriteriaInput();
			criteriaInput[0].queryNames = new String[] { "Item..." };

			FindSavedQueriesResponse savedQryResponse = sqService.findSavedQueries(criteriaInput);
			ImanQuery query = savedQryResponse.savedQueries[0];

			SavedQueryInput[] qryInput = new SavedQueryInput[1];
			qryInput[0] = new SavedQueryInput();
			qryInput[0].entries = new String[] { "Item ID" };
			qryInput[0].values = new String[] { id };
			qryInput[0].query = query;

			ExecuteSavedQueriesResponse executeResponse = sqService.executeSavedQueries(qryInput);
			ModelObject[] resultModelObjects = null;
			String objectID = "";
			String objectString = "";
			if (executeResponse.serviceData.sizeOfPartialErrors() > 0) {
				System.out.println("\nERROR:Failed while Executing a Query"
						+ executeResponse.serviceData.getPartialError(0).getErrorValues()[0].getMessage());
			} else {
				SavedQueryResults[] results = executeResponse.arrayOfResults;
				
				for(SavedQueryResults result : results) {
					resultModelObjects = (ModelObject[]) result.objects;
				}
				TestConnect.dmService.getProperties(resultModelObjects, new String[] {"item_id","object_string"});
				System.out.println("** No.of executeResponse "+results.length);
				if (results.length > 0) {
					for(int i=0; i<results.length; i++) {
						objectID = resultModelObjects[i].getPropertyDisplayableValue("item_id");
						System.out.println("** objectID "+objectID);
						objectString = resultModelObjects[i].getPropertyDisplayableValue("object_string");
						System.out.println("** objectString "+objectString);
						uid.append(resultModelObjects[i].getUid());
						return true;
					}
					
				}
			}
		} catch (Exception e) {
			// TODO: handle exception
		}
		return false;
	}
}
