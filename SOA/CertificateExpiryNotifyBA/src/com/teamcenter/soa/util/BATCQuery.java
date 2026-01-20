package com.teamcenter.soa.util;
//==================================================
//
//  Copyright 2017 Siemens Product Lifecycle Management Software Inc. All Rights Reserved.
//
//==================================================




import java.io.IOException;
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
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.ImanQuery;
import com.teamcenter.soa.client.model.strong.Item;
import com.teamcenter.soa.client.model.strong.ItemRevision;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.soa.loader.Connect;

@SuppressWarnings("unused")
public class BATCQuery
{

    /**
     * Perform a simple query of the database
     * @throws IOException 
     *
     */
	
	    public static ArrayList<ModelObject> PerformSearchQuery(String sQueryName, String[] sEntries, String[] sValues) throws IOException
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
	   				System.out.println("ERROR Perform Search is getting Failed ... Partial Errors Recieved ...\n");
	   				
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
	               Connect.writer.write("ERROR: Perform Search failed - " + e.getMessage() + "\n");
	           }
	   		
	           return BusinessObjectList;
	   	}	
	 
	    public static User findUser_fromName(String userName)
	    {
	        ImanQuery query = null;
	        User user = null;
	        

	        SavedQueryService queryService = SavedQueryService.getService(AppXSession.getConnection());
	        DataManagementService dmService= DataManagementService.getService(AppXSession.getConnection());
	        
	        try
	        {
	            GetSavedQueriesResponse savedQueries = queryService.getSavedQueries();
	            if (savedQueries.queries.length == 0)
	            {
	                System.out.println("There are no saved queries in the system.");
	                return user;
	            }
	            
	            for (int i = 0; i < savedQueries.queries.length; i++)
	            {

	                if (savedQueries.queries[i].name.equals("__WEB_find_user_from_name")) //__WEB_find_user
	                {
	                    query = savedQueries.queries[i].query;
	                    break;
	                }
	            }
	        }
	        catch (ServiceException e)
	        {
	            System.out.println("GetSavedQueries service request failed.");
	            System.out.println(e.getMessage());
	            return user;
	        }

	        if (query == null)
	        {
	            System.out.println("There is not an '__WEB_find_user_from_name' query.");
	            return user;
	        }

	        try
	        {
	            QueryInput savedQueryInput[] = new QueryInput[1];
	            savedQueryInput[0] = new QueryInput();
	            savedQueryInput[0].query = query;
	            savedQueryInput[0].entries = new String[1];
	            savedQueryInput[0].values = new String[1];
	            savedQueryInput[0].entries[0] = "Name" ;
	            savedQueryInput[0].values[0] = userName;
	            savedQueryInput[0].maxNumToReturn = 25;
	            
	            SavedQueriesResponse savedQueryResult = queryService.executeSavedQueries(savedQueryInput);
	            QueryResults found = savedQueryResult.arrayOfResults[0];

	            int length = found.objectUIDS.length;
	            
	            String[] uids = new String[length];
	            for( int ii = 0; ii < length; ii++)
	            {
	                uids[ii]= found.objectUIDS[ii];
	            }
	            ServiceData sd = dmService.loadObjects(uids);
	            ModelObject[] foundObjs = new ModelObject[ sd.sizeOfPlainObjects()];
	            for( int k =0; k< sd.sizeOfPlainObjects(); k++)
	            {
	                foundObjs[k] = sd.getPlainObject(k);
	            }
	            user = (User) foundObjs[0];   
	        }
	        catch (Exception e)
	        {
	            System.out.println("ExecuteSavedQuery service request failed.");
	            System.out.println(e.getMessage());
	        }            
	        return user;
	    }
	
}
