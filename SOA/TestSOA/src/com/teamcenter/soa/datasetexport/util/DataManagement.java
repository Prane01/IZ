package com.teamcenter.soa.datasetexport.util;

import java.io.FileWriter;
import java.util.HashMap;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core._2007_01.DataManagement.VecStruct;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;

public class DataManagement
{
	public static void loadProperties(ModelObject[] mObjectsToLoad, String[] sProperties, FileWriter writer)
	{
		try
		{
			DataManagementService dmService = DataManagementService.getService(AppXSession.getConnection());
			ServiceData loadResponse = dmService.getProperties(mObjectsToLoad, sProperties);
			if(loadResponse.sizeOfPartialErrors()>0)
			{
				writer.write("Failed while loading properties - "+loadResponse.getPartialError(0).getErrorValues()[0].getMessage());
			}
		}catch(Exception e) {e.printStackTrace();}
	}
	
	public static void updateTcProperties(ModelObject mTargetObject, String sPropertyName, String sPropertyValue, FileWriter writer)
	{
		try
		{
			DataManagementService dmService = DataManagementService.getService(AppXSession.getConnection());
			
			java.util.Map<String, VecStruct> updateMapper = new HashMap<String,VecStruct>();
			VecStruct vecStr = new VecStruct();
			vecStr.stringVec = new String[] {sPropertyValue};
			updateMapper.put(sPropertyName, vecStr);
			
			ServiceData setPropertyResponse = dmService.setProperties(new ModelObject[] {mTargetObject}, updateMapper);
			if(setPropertyResponse.sizeOfPartialErrors()>0)
			{
				writer.write("\nERROR: Failed while setting property to Accolade project "+setPropertyResponse.getPartialError(0).getErrorValues()[0].getMessage());
			}
			
		}catch(Exception e) {e.printStackTrace();}
	}

}
