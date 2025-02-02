package com.teamcenter.soa.datasetexport.util;

import java.io.FileWriter;
import java.io.IOException;

import com.teamcenter.services.loose.workflow._2007_06.Workflow;

public class CreatePAL {

	public CreatePAL() {
		// TODO Auto-generated constructor stub
	}
	
	public void initiateProcess(FileWriter writer) {
		
		try {
			System.out.println("\nProcess Initiated !!!");
			writer.write("\nProcess Initiated !!!");
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
