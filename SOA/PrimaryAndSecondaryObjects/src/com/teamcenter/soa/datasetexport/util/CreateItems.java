package com.teamcenter.soa.datasetexport.util;

import java.io.FileWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.soa.client.model.strong.Item;
import com.teamcenter.soa.datasetexport.loader.Connect;
import com.teamcenter.services.strong.core._2008_06.DataManagement.CreateIn;
import com.teamcenter.services.strong.core._2008_06.DataManagement.CreateInput;
import com.teamcenter.services.strong.core._2008_06.DataManagement.CreateResponse;

public class CreateItems {
	public static void itemsCreate(ArrayList<String> createObject, FileWriter writer) {

		try {
			// Get connection
			Connect.dmService = DataManagementService.getService(AppXSession.getConnection());
			// Prepare the input for creating an Item object
			CreateIn[] createInputs = new CreateIn[1];

			createInputs[0] = new CreateIn();
			createInputs[0].clientId = "random";

			// CreateInput for Property inputs
			CreateInput cIn = new CreateInput();

			HashMap<String, String> hmInputMapper = new HashMap<String, String>();

			for (String id : createObject) {
				// properties using map
				hmInputMapper.put("item_id", id);
				hmInputMapper.put("object_name", "SOA Test Item 3");
				hmInputMapper.put("object_desc", "This is a test item created via SOA 3");

				cIn.stringProps = hmInputMapper;
				cIn.boName = "Item";
				createInputs[0].data = cIn;
				// Call the SOA service to create the Item
				CreateResponse itemsResponse = Connect.dmService.createObjects(createInputs);

				// Check the result
				if (itemsResponse.serviceData.sizeOfPartialErrors() > 0) {
					writer.write("\nFailed to create Objects in TC");
					System.out.println("Failed to create Objects in TC");
				} else {
					Item createdItem = (Item) itemsResponse.serviceData.getCreatedObject(0);
					writer.write("\nItem created successfully with UID: " + createdItem.getUid());
					System.out.println("Item created successfully with UID: " + createdItem.getUid());
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}