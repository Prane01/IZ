package com.teamcenter.soa.datasetexport.util;

import java.io.FileWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.annotation.processing.SupportedSourceVersion;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core._2007_01.DataManagement.VecStruct;
import com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsData2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsOutput2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsPref2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsResponse2;
import com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationship;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.Dataset;
import com.teamcenter.soa.client.model.strong.ImanRelation;
import com.teamcenter.soa.datasetexport.util.QueryManagement;
import com.teamcenter.soa.loader.Connect;

public class GetSecondary {
	public static void getSecondaryObjects(List<String> lines, ArrayList<String> uid, FileWriter writer) {

		try {
			// Get connection
			Connect.dmService = DataManagementService.getService(AppXSession.getConnection());	
			
			ArrayList<ModelObject> secObjects = new ArrayList<ModelObject>();
			
 			ServiceData serviceData = Connect.dmService.loadObjects(uid.toArray(new String[0]));
			
			// Check the result
			if(!(serviceData.sizeOfPartialErrors()>0) && serviceData.sizeOfPlainObjects()>0) {
				ModelObject tcObject = serviceData.getPlainObject(0);
				
				// set the relation
				RelationAndTypesFilter relAndTypeFltr = new RelationAndTypesFilter();
				relAndTypeFltr.relationTypeName = "IMAN_specification";
				
				//expand the preferred level
				ExpandGRMRelationsPref2 expandPref = new ExpandGRMRelationsPref2();
				expandPref.expItemRev = true;
				expandPref.returnRelations = true;
				expandPref.info = new RelationAndTypesFilter[] {relAndTypeFltr};
				
				// expand response for the objects
				ExpandGRMRelationsResponse2 exndResponse = 
						Connect.dmService.expandGRMRelationsForPrimary(new ModelObject[] {tcObject}, expandPref);
				
				String specificUid = "";
				
				if(exndResponse.serviceData.sizeOfPartialErrors() == 0) {
					for(ExpandGRMRelationsOutput2 relOut : exndResponse.output) {
						for(ExpandGRMRelationsData2 relData : relOut.relationshipData) {
							for(ExpandGRMRelationship relShip : relData.relationshipObjects) {
								if(relShip.relation instanceof ImanRelation) {
									if(relShip.otherSideObject instanceof Dataset) {
										System.out.println("Inst of Dataset\n\n");
										Connect.dmService.getProperties(new ModelObject[] {relShip.otherSideObject}, new String[] {"object_name","ref_list"});
										String DatasetName = relShip.otherSideObject.getPropertyDisplayableValue("object_name");
										List<String> refList = relShip.otherSideObject.getPropertyDisplayableValues("ref_list");
										System.out.println("DatasetName : \n\n" + DatasetName);
										for(int i=0; i<refList.size(); i++) {
											System.out.println("refList " + i +" : " + refList.get(i));
										}
									}
								}
							}
						}
					}
				}
				else {
					writer.write("\nERROR:CREATE=" + uid + "#" + "Error: "
					        + exndResponse.serviceData.getPartialError(0).getErrorValues()[0]
					        .getMessage()
					   + "\n");
				}
				
			}else {
				writer.write("\nUnable to find ModelObject with UID: " + uid);
				System.out.println("Unable to find ModelObject with UID: " + uid);
			}

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
