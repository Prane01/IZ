package com.teamcenter.soa.datasetexport.util;

import java.io.FileWriter;
import java.util.HashMap;
import java.util.Map;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core._2007_01.DataManagement.VecStruct;
import com.teamcenter.services.strong.core._2008_06.DataManagement.CreateIn;
import com.teamcenter.services.strong.core._2008_06.DataManagement.CreateInput;
import com.teamcenter.services.strong.core._2008_06.DataManagement.CreateResponse;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.Item;
import com.teamcenter.soa.datasetexport.loader.TestConnect;

public class UpdateItems {
	public static void updateObjects(String id, StringBuilder uid, FileWriter writer) {

		try {
			// Get connection
			TestConnect.dmService = DataManagementService.getService(AppXSession.getConnection());			
			Map<String,VecStruct> hmInputMapper = new HashMap<String, VecStruct>();
			VecStruct vecStr = new VecStruct();
			vecStr.stringVec = new String[] {"Test SOA Update 28/04"};
			
			// properties using map
			hmInputMapper.put("object_desc", vecStr);
			
			ServiceData serviceData = TestConnect.dmService.loadObjects(new String[] {uid.toString()});
			// Check the result
			if (serviceData.sizeOfPlainObjects()>0) {
				ModelObject tcModelObject = serviceData.getPlainObject(0);
				TestConnect.dmService.setProperties(new ModelObject[] {tcModelObject}, hmInputMapper);
				writer.write("\nItem Updated successfully with UID: " + tcModelObject.getUid());
				System.out.println("Item Updated successfully with UID: " + tcModelObject.getUid());
			} else {
				writer.write("\nUnable to find ModelObject with UID: " + uid);
				System.out.println("Unable to find ModelObject with UID: " + uid);
			}

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
