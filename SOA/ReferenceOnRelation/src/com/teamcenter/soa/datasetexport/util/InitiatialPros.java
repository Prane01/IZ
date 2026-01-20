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
import com.teamcenter.soa.datasetexport.loader.Connect;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.ExecuteSavedQueriesResponse;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.SavedQueryInput;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.SavedQueryResults;
import com.teamcenter.services.strong.query._2010_04.SavedQuery.FindSavedQueriesCriteriaInput;
import com.teamcenter.services.strong.query._2010_04.SavedQuery.FindSavedQueriesResponse;

public class InitiatialPros {

	public String csvFile = "D:\\SOA\\ReadFile\\GetSecondaryOutput.csv";
	public static StringBuilder sb = new StringBuilder();
	
	public InitiatialPros() {
		// TODO Auto-generated constructor stub
	}

	public void initiateProcess(FileWriter writer) {

		try {		
			System.out.println("\nProcess Initiated !!!");
			writer.write("\nProcess Initiated !!!");
			FileWriter writer2 = new FileWriter(csvFile);
			ArrayList<String> foundUID = new ArrayList<String>();

			List<String> lines = Files.readAllLines(Paths.get("D:\\SOA\\ReadFile\\GetSecondaries.txt"));
			for (String line : lines) {
                System.out.println(line);
            }
			
			checkIfObjectExists(lines, writer, writer2, foundUID);		
			
			if(foundUID.size()>0) {
				writer.write("\n*** Found UID ***");
				GetSecondary.getSecondaryObjects(lines, foundUID, writer);
			} 
			writer2.write(sb.toString());
			writer2.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void checkIfObjectExists(List<String> lines, FileWriter writer, FileWriter writer2, ArrayList<String> foundUID ) {
		try {
			Connect.dmService = DataManagementService.getService(AppXSession.getConnection());
			SavedQueryService sqService = SavedQueryService.getService(AppXSession.getConnection());

			FindSavedQueriesCriteriaInput[] criteriaInput = new FindSavedQueriesCriteriaInput[1];
			criteriaInput[0] = new FindSavedQueriesCriteriaInput();
			criteriaInput[0].queryNames = new String[] { "Item Revision..." };

			FindSavedQueriesResponse savedQryResponse = sqService.findSavedQueries(criteriaInput);
			ImanQuery query = savedQryResponse.savedQueries[0];

			int noOfLines = lines.size();
			
			SavedQueryInput[] qryInput = new SavedQueryInput[noOfLines];
			
			for(int j=0; j<noOfLines; j++) {	
				qryInput[j] = new SavedQueryInput();
				qryInput[j].entries = new String[] { "Item ID", "Revision" };
				
				String[] parts = lines.get(j).split(",");

	            if (parts.length != 2) {
	                System.out.println("Invalid format: " + lines.get(j));
	                continue;
	            }
	            else {
					qryInput[j].values = new String[] { parts[0].trim(), parts[1].trim() };
					qryInput[j].query = query;
	            }
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
						Connect.dmService.getProperties(resultModelObjects, new String[] {"object_string", "object_type"});
						for(int i=0; i<resultModelObjects.length; i++) {
							objectString = resultModelObjects[i].getPropertyDisplayableValue("object_string");
							System.out.println("** objectString "+objectString);
							objectType = resultModelObjects[i].getPropertyDisplayableValue("object_type");
							System.out.println("** objectType "+objectType);
							foundUID.add(resultModelObjects[i].getUid());						
						}
					}
				}
			}
		} catch (Exception e) {
			// TODO: handle exception
		}
	}             
}
