//==================================================
//
//  Copyright 2022 Siemens Digital Industries Software
//
//==================================================

package com.teamcenter.soa.datasetexport.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.loose.core._2006_03.FileManagement.DatasetFileInfo;
import com.teamcenter.services.loose.core._2006_03.FileManagement.GetDatasetWriteTicketsInputData;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core._2006_03.DataManagement.CreateDatasetsResponse;
import com.teamcenter.services.strong.core._2008_06.DataManagement.DatasetProperties2;
import com.teamcenter.services.strong.query.SavedQueryService;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.ExecuteSavedQueriesResponse;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.SavedQueryInput;
import com.teamcenter.services.strong.query._2007_06.SavedQuery.SavedQueryResults;
import com.teamcenter.services.strong.query._2010_04.SavedQuery.FindSavedQueriesCriteriaInput;
import com.teamcenter.services.strong.query._2010_04.SavedQuery.FindSavedQueriesResponse;
import com.teamcenter.soa.client.FileManagementUtility;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.ImanQuery;
import com.teamcenter.soa.datasetexport.loader.Connect;

/**
 * Use the FileManagementService to transfer files
 *
 */
public class FileManagement
{
    /** The number of datasets to upload in the multiple file example. */
    static final int NUMBER_OF_DATASETS = 120;

    /** The number of files per dataset to upload in the multiple file example. */
    static final int NUMBER_OF_FILES_PER_DATASET = 3;

    /** Upload some files using the FileManagement utilities. */
    public void uploadFiles(AppXSession session)
    {
        FileManagementUtility fMSFileManagement = new FileManagementUtility(session.getConnection());
        DataManagementService dmService = DataManagementService.getService(session.getConnection());
        try
        {
            uploadSingleFile(fMSFileManagement, dmService);
//            uploadMultipleFiles(fMSFileManagement, dmService);
        }
        finally
        {
            // Close FMS connection when done
            fMSFileManagement.term();
        }
    }

    /** Uploads a single file using the FileManagement utilities. */
    public void uploadSingleFile(final FileManagementUtility fMSFileManagement, final DataManagementService dmService)
    {
        GetDatasetWriteTicketsInputData[] inputs = { getGetDatasetWriteTicketsInputData(dmService) };
        ServiceData response = fMSFileManagement.putFiles(inputs);

        if (response.sizeOfPartialErrors() > 0)
            System.out.println("FileManagementService upload returned partial errors: " + response.sizeOfPartialErrors());

        // Delete all objects created
//        ModelObject [] datasets = { inputs[0].dataset };
//        dmService.deleteObjects(datasets);
    }

    /** Uploads multiple files using the FileManagement utilities. */
    public void uploadMultipleFiles(final FileManagementUtility fMSFileManagement, final DataManagementService dmService)
    {
        GetDatasetWriteTicketsInputData[] inputs = getMultipleGetDatasetWriteTicketsInputData(dmService);
        ServiceData response = fMSFileManagement.putFiles(inputs);

        if (response.sizeOfPartialErrors() > 0)
            System.out.println("FileManagementService multiple upload returned partial errors: " + response.sizeOfPartialErrors());

        // Delete all objects created
        ModelObject[] datasets = new ModelObject[inputs.length];
        for (int i = 0; i < inputs.length; ++i)
        {
            datasets[i] = inputs[i].dataset;
        }
        dmService.deleteObjects(datasets);
    }

    /** @return A single GetDatasetWriteTicketsInputData for uploading ReadMe.txt. */
    private GetDatasetWriteTicketsInputData getGetDatasetWriteTicketsInputData(final DataManagementService dmService)
    {
    	ModelObject revisionObj = checkIfObjectExists("000275", "A");
        // Create a Dataset
        DatasetProperties2 props = new DatasetProperties2();
        props.clientId = "datasetWriteTixTestClientId";
        props.type = "Text";
        props.name = "Test-FMS-Upload";
        props.description = "Testing put File";
        props.relationType = "IMAN_specification";
        props.container = revisionObj;
        DatasetProperties2[] currProps = { props };

        CreateDatasetsResponse resp =  dmService.createDatasets2(currProps);

        // Assume this file is in current dir
        File file1 = new File("C:\\Users\\13351\\Downloads\\install_UPDATER_2509121101_27.txt");

        // Create a file to associate with dataset
        DatasetFileInfo fileInfo = new DatasetFileInfo();
        fileInfo.clientId            = "file_1";
        fileInfo.fileName            = file1.getAbsolutePath();
        fileInfo.namedReferencedName = "Text";
        fileInfo.isText              = true;
        fileInfo.allowReplace        = false;
        DatasetFileInfo[] fileInfos = { fileInfo };

        GetDatasetWriteTicketsInputData inputData = new GetDatasetWriteTicketsInputData();
        inputData.dataset = resp.output[0].dataset;
        inputData.createNewVersion = false;
        inputData.datasetFileInfos = fileInfos;

        return inputData;
    }

    /**
     * @return An array of NUMBER_OF_DATASETS GetDatasetWriteTicketsInputData objects
     * for uploading NUMBER_OF_FILES_PER_DATASET copies of ReadMe.txt to each Dataset.
     */
    private GetDatasetWriteTicketsInputData[] getMultipleGetDatasetWriteTicketsInputData(final DataManagementService dmService)
    {
        GetDatasetWriteTicketsInputData[] inputs = new GetDatasetWriteTicketsInputData[NUMBER_OF_DATASETS];
        DatasetProperties2[] currProps = new DatasetProperties2[inputs.length];

        // Create a bunch of Datasets
        for (int i = 0; i < inputs.length; ++i)
        {
            DatasetProperties2 props = new DatasetProperties2();
            props.clientId = "datasetWriteTixTestClientId " + i;
            props.type = "Text";
            props.name = "Sample-FMS-Upload-" + i;
            props.description = "Testing Multiple put File";
            currProps[i] = props;
        }

        CreateDatasetsResponse resp =  dmService.createDatasets2(currProps);

        // Create files to associate with each Dataset
        for (int i = 0; i < inputs.length; ++i)
        {
            DatasetFileInfo fileInfos[] = new DatasetFileInfo[NUMBER_OF_FILES_PER_DATASET];
            for (int j = 0; j < fileInfos.length; ++j)
            {
                DatasetFileInfo fileInfo = new DatasetFileInfo();

                // We apparently cannot upload two files with the same name to
                //      the same Dataset because this creates a relation name
                //      conflict.
//                File file1 = new File("ReadMe.txt");

                // Create or use this file is in current dir
                File file1 = new File("ReadMeCopy" + j + ".txt");
                assureFileCreated(file1);

                fileInfo.clientId            = "Dataset " + i + " File " + j;
                fileInfo.fileName            = file1.getAbsolutePath();
                fileInfo.namedReferencedName = "Text";
                fileInfo.isText              = true;
                fileInfo.allowReplace        = false;
                fileInfos[j] = fileInfo;
            }

            GetDatasetWriteTicketsInputData inputData = new GetDatasetWriteTicketsInputData();
            inputData.dataset = resp.output[i].dataset;
            inputData.createNewVersion = false;
            inputData.datasetFileInfos = fileInfos;

            inputs[i] = inputData;
        }
        return inputs;
    }

    /**
     * Assures that the file exists on the file system.
     * If not, this method copies "ReadMe.txt" to create the file.
     * @param file1 (File) The file to be created if it does not already exist.
     */
    private void assureFileCreated(final File file1)
    {
        if (file1.exists())
        {
            return;
        }
        FileInputStream fis = null;
        FileOutputStream fos = null;
        try
        {
            try
            {
                byte[] buffer = new byte[16384];

                // Assume this file is in current dir
                fis = new FileInputStream(new File("C:\\Users\\13351\\Downloads\\install_UPDATER_2509121101_27.txt"));
                // Assume that we can copy it in the current dir
                fos = new FileOutputStream(file1);

                while (true)
                {
                    int bytesRead = fis.read(buffer);
                    if (bytesRead <= 0)
                    {
                        // EOF
                        break;
                    }
                    fos.write(buffer, 0, bytesRead);
                }
            }
            finally
            {
                if (fos != null)
                {
                    fos.close();
                }
                if (fis != null)
                {
                    fis.close();
                }
            }
        }
        catch (IOException ioe)
        {
            System.out.println("Could not copy 'ReadMe.txt' to '" + file1.getName()
                + "' - " + ioe.getClass().getSimpleName() + ": " + ioe.getMessage());
        }
    }
    
    public ModelObject checkIfObjectExists(String itemID, String revisionID) {
		try {
			Connect.dmService = DataManagementService.getService(AppXSession.getConnection());
			SavedQueryService sqService = SavedQueryService.getService(AppXSession.getConnection());

			FindSavedQueriesCriteriaInput[] criteriaInput = new FindSavedQueriesCriteriaInput[1];
			criteriaInput[0] = new FindSavedQueriesCriteriaInput();
			criteriaInput[0].queryNames = new String[] { "Item Revision..." };

			FindSavedQueriesResponse savedQryResponse = sqService.findSavedQueries(criteriaInput);
			ImanQuery query = savedQryResponse.savedQueries[0];

			
			SavedQueryInput[] qryInput = new SavedQueryInput[1];
				
			qryInput[0] = new SavedQueryInput();
			qryInput[0].entries = new String[] { "Item ID", "Revision" };
			qryInput[0].values = new String[] { itemID, revisionID };
			qryInput[0].query = query;

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
						Connect.dmService.getProperties(resultModelObjects, new String[] {"item_id","object_string", "object_type"});
						for(int i=0; i<resultModelObjects.length; i++) {
							objectID = resultModelObjects[i].getPropertyDisplayableValue("item_id");
							System.out.println("** objectID "+objectID);
							objectString = resultModelObjects[i].getPropertyDisplayableValue("object_string");
							System.out.println("** objectString "+objectString);
							objectType = resultModelObjects[i].getPropertyDisplayableValue("object_type");
							System.out.println("** objectType "+objectType);
							return resultModelObjects[i];		
						}
					}
				}
			}
		} catch (Exception e) {
			// TODO: handle exception
		}
		return null;
	}
}
