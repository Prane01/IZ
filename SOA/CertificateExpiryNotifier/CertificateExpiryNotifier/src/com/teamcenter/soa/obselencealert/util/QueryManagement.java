package com.teamcenter.soa.obselencealert.util;

import java.io.FileWriter;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.schemas.soa._2006_03.exceptions.ServiceException;
import com.teamcenter.services.strong.query.SavedQueryService;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.ExecuteSavedQueriesResponse;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.SavedQueryInput;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.SavedQueryResults;
import com.teamcenter.services.strong.query._2010_04.SavedQuery.FindSavedQueriesCriteriaInput;
import com.teamcenter.services.strong.query._2010_04.SavedQuery.FindSavedQueriesResponse;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.ImanQuery;
import com.teamcenter.soa.client.model.strong.Person;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.services.strong.query._2006_03.SavedQuery.GetSavedQueriesResponse;
import com.teamcenter.services.strong.query._2007_09.SavedQuery.SavedQueriesResponse;
import com.teamcenter.services.strong.query._2008_06.SavedQuery.QueryInput;
import com.teamcenter.services.strong.query._2007_09.SavedQuery.QueryResults;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.soa.client.model.ServiceData;



public class QueryManagement 
{
	public static ModelObject[] queryObjects(String sQueryName, String[] sEntries, String[] sValues, FileWriter writer)
	{
		try
		{
			SavedQueryService queryService  = SavedQueryService.getService(AppXSession.getConnection());
			SavedQueryInput[] queryInputs = new SavedQueryInput[1];

			FindSavedQueriesCriteriaInput[] criteriaInput = new FindSavedQueriesCriteriaInput[1];
			criteriaInput[0] = new FindSavedQueriesCriteriaInput();
			criteriaInput[0].queryNames = new String[] {sQueryName};
			
			FindSavedQueriesResponse findQueryResponse = queryService.findSavedQueries(criteriaInput);
			ImanQuery query = findQueryResponse.savedQueries[0];

			queryInputs[0] = new SavedQueryInput();
			queryInputs[0].entries = sEntries;
			queryInputs[0].values = sValues;
			queryInputs[0].query = query;
			
			ExecuteSavedQueriesResponse savedQueryResponse = queryService.executeSavedQueries(queryInputs);
			if(savedQueryResponse.serviceData.sizeOfPartialErrors() > 0)
			{
				System.out.println("\nERROR:Failed while Executing a Query"+savedQueryResponse.serviceData.getPartialError(0).getErrorValues()[0].getMessage());
				writer.write("\nERROR:Failed while Executing a Query"+savedQueryResponse.serviceData.getPartialError(0).getErrorValues()[0].getMessage());
			}
			else
			{
				SavedQueryResults[] queryResults = savedQueryResponse.arrayOfResults;
				writer.write("\nInfo:Total number of objects from Query "+queryResults.length);
				//System.out.println("\nInfo:Total number of objects from Query "+queryResults.length);
				//return queryResults[0].objects;
				for(SavedQueryResults queryResult : queryResults)
				{
					return queryResult.objects;
				}
			}
		}catch(Exception e) {e.printStackTrace();}
		return null;
	}
	
	public static User findUser(String userId)
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

                if (savedQueries.queries[i].name.equals("__WEB_find_user")) //__WEB_find_user
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
            System.out.println("There is not an '__WEB_find_user' query.");
            return user;
        }

        try
        {
            QueryInput savedQueryInput[] = new QueryInput[1];
            savedQueryInput[0] = new QueryInput();
            savedQueryInput[0].query = query;
            savedQueryInput[0].entries = new String[1];
            savedQueryInput[0].values = new String[1];
            savedQueryInput[0].entries[0] = "User ID" ;
            savedQueryInput[0].values[0] = userId;
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
