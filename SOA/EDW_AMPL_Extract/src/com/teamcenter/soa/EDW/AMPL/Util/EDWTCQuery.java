package com.teamcenter.soa.EDW.AMPL.Util;
//==================================================
//
//  Copyright 2017 Siemens Product Lifecycle Management Software Inc. All Rights Reserved.
//
//==================================================




import java.util.ArrayList;
import java.util.Arrays;


import com.teamcenter.clientx.AppXSession;
import com.teamcenter.schemas.soa._2006_03.exceptions.ServiceException;
import com.teamcenter.services.strong.core.DataManagementService;
//Include the Saved Query Service Interface
import com.teamcenter.services.strong.query.SavedQueryService;
// Input and output structures for the service operations
// Note: the different namespace from the service interface
import com.teamcenter.services.strong.query._2006_03.SavedQuery.GetSavedQueriesResponse;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.ExecuteSavedQueriesResponse;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.SavedQueryInput;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.SavedQueryResults;
import com.teamcenter.services.strong.query._2007_09.SavedQuery.QueryResults;
import com.teamcenter.services.strong.query._2007_09.SavedQuery.SavedQueriesResponse;
import com.teamcenter.services.strong.query._2008_06.SavedQuery.QueryInput;
import com.teamcenter.services.strong.query._2010_04.SavedQuery.FindSavedQueriesCriteriaInput;
import com.teamcenter.services.strong.query._2010_04.SavedQuery.FindSavedQueriesResponse;
import com.teamcenter.soa.EDW.AMPL.EDWDataMgmt;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.ImanQuery;
import com.teamcenter.soa.client.model.strong.Item;
import com.teamcenter.soa.client.model.strong.ItemRevision;

@SuppressWarnings("unused")
public class EDWTCQuery
{

    /**
     * Perform a simple query of the database
     *
     */
	
	    public static ArrayList<ModelObject> PerformSearchQuery(String sQueryName, String[] sEntries, String[] sValues)
	   	{
	   		ArrayList<ModelObject> BusinessObjectList = new ArrayList<ModelObject>();
	   		try
	   		{
	   		    SavedQueryService SavedQueryServObj = SavedQueryService.getService(AppXSession.getConnection());

	   			//Find Query
	   			FindSavedQueriesCriteriaInput[] FindSavedQueryCriteriaInputList = new FindSavedQueriesCriteriaInput[1];
	   			FindSavedQueryCriteriaInputList[0] = new FindSavedQueriesCriteriaInput();
	   			FindSavedQueryCriteriaInputList[0].queryNames = new String[] { sQueryName };

	   			FindSavedQueriesResponse FindSavedQueryResponse = SavedQueryServObj.findSavedQueries(FindSavedQueryCriteriaInputList);
	   			ImanQuery query = FindSavedQueryResponse.savedQueries[0];
	   			
	            SavedQueryInput[] SavedQueryInputList = new SavedQueryInput[1];
	   			
	   			SavedQueryInputList[0] = new SavedQueryInput();
	   			SavedQueryInputList[0].entries = sEntries; 
	   			SavedQueryInputList[0].values = sValues;
	   			SavedQueryInputList[0].query = query;
	   			//SavedQueryInputList[0].maxNumToReturn = -1;
	   			
	   			ExecuteSavedQueriesResponse SavedQueryResponse = SavedQueryServObj.executeSavedQueries(SavedQueryInputList);
	   			if(SavedQueryResponse.serviceData.sizeOfPartialErrors() > 0)
	   			{
	   				EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR", "Perform Search is getting Failed ... Partial Errors Recieved ...\n"));
	   				EDWDataMgmt.EDWLogBuilder.append(EDWMessageUtil.LogMessage("ERROR", SavedQueryResponse.serviceData.getPartialError(0).getErrorValues()[0].getMessage()+"\n"));
	   			}
	   			else
	   			{
	   				SavedQueryResults[] SavedQueryResultList = SavedQueryResponse.arrayOfResults;
	   				
	   				for(SavedQueryResults SavedQueryResult : SavedQueryResultList)
	   				{
	   					if (SavedQueryResult.numOfObjects > 0)
	   					{
	   						BusinessObjectList.addAll(Arrays.asList(SavedQueryResult.objects));
	   					}
	   				}
	   			}
	   			
	   		}
	           catch(Exception e) 
	           {
	               e.printStackTrace();
	           }
	   		
	           return BusinessObjectList;
	   	}	
	 

	
}
