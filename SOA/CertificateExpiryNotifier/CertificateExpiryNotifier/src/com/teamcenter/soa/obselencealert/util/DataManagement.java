package com.teamcenter.soa.obselencealert.util;

import java.io.FileWriter;
import java.util.HashMap;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.cad.StructureManagementService;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core._2007_01.DataManagement.VecStruct;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.Type;
import com.teamcenter.soa.client.model.strong.ImanAliasList;
import com.teamcenter.soa.client.model.strong.RevisionRule;

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
	public static void refreshObject(ModelObject[] mObj)
	{
		try
		{
			DataManagementService dmService = DataManagementService.getService(AppXSession.getConnection());
			dmService.refreshObjects(mObj);
			
		}catch(Exception e) {e.printStackTrace();}
	}
	public static void updateTcProperties(ModelObject mTargetObject, String sPropertyName, String sPropertyValue, FileWriter writer)
	{
		try
		{
			
			//ImanAliasList aliasList = new ImanAliasList(Type, "");
			DataManagementService dmService = DataManagementService.getService(AppXSession.getConnection());
			
			java.util.Map<String, VecStruct> updateMapper = new HashMap<String,VecStruct>();
			VecStruct vecStr = new VecStruct();
			vecStr.stringVec = new String[] {sPropertyValue};
			updateMapper.put(sPropertyName, vecStr);
			
			ServiceData setPropertyResponse = dmService.setProperties(new ModelObject[] {mTargetObject}, updateMapper);
			if(setPropertyResponse.sizeOfPartialErrors()>0)
			{
				writer.write("\nERROR: Failed while setting property to project "+setPropertyResponse.getPartialError(0).getErrorValues()[0].getMessage());
			} else {
				writer.write("\nINFO :: Proerty "+sPropertyName+ " updated with value "+sPropertyValue);
			}
			
		}catch(Exception e) {e.printStackTrace();}
	}
	public static RevisionRule getRevisionRule(String currRevRule)
	{
		try
		{
			DataManagementService dmService = DataManagementService.getService(AppXSession.getConnection());
			StructureManagementService smService = StructureManagementService.getService(AppXSession.getConnection());
			StructureManagement.GetRevisionRulesResponse revRuleResp = smService.getRevisionRules();
			if(revRuleResp != null)
			{
				StructureManagement.RevisionRuleInfo ruleInfo[] = revRuleResp.output;
				for(int j=0 ; j<ruleInfo.length ; j++)
				{
					RevisionRule revRule = ruleInfo[j].revRule;
					dmService.getProperties(new ModelObject[]{revRule}, new String[]{"object_name"});
					String sRevRule = revRule.get_object_name();
					if(sRevRule.equals(currRevRule))
						return revRule;
				}
			}
		}
		catch (Exception e) 
		{
			System.out.println("Exception in getRevisionRule - "+e);
			e.printStackTrace();
		}
		return null;
	}
}
