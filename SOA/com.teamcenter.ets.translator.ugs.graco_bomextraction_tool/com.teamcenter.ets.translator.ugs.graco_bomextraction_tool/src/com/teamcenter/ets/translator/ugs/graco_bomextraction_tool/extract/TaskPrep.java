package com.teamcenter.ets.translator.ugs.graco_bomextraction_tool.extract;


import com.teamcenter.ets.extract.DefaultTaskPrep;
import com.teamcenter.ets.soa.ConnectionManager;
import com.teamcenter.ets.soa.SoaHelper;
import com.teamcenter.services.strong.cad.StructureManagementService;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.Property;
import com.teamcenter.soa.client.model.strong.BOMLine;
import com.teamcenter.soa.client.model.strong.ItemRevision;
import com.teamcenter.soa.client.model.strong.Person;
import com.teamcenter.soa.client.model.strong.RevisionRule;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.translationservice.task.TranslationTask;
//TCUP-167 start
//We import the "CreateWindowsInfo3" class directly to avoid a conflict with "_2007_01.StructureManagement".
//Importing "_2019_06.StructureManagement" would cause a collision with "_2007_01.StructureManagement".
//Disabling the "_2007_01.StructureManagement" class would block access to its defined methods.
import com.teamcenter.services.strong.cad._2019_06.StructureManagement.CreateWindowsInfo3;
////TCUP-167 end
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class TaskPrep extends DefaultTaskPrep {
  public TranslationTask prepareTask() throws Exception {
    ModelObject[] zPrimaryObjs = this.request.getPropertyObject("primaryObjects").getModelObjectArrayValue();
    this.request.getPropertyObject("providerName").getStringValue();
    this.request.getPropertyObject("serviceName").getStringValue();
    DataManagementService.getService(ConnectionManager.getActiveConnection());
    ItemRevision zPrimaryItemRevision = (ItemRevision)zPrimaryObjs[0];
    SoaHelper.getProperties((ModelObject)zPrimaryItemRevision, new String[] { "current_id", "object_string" });
    String scCurrentID = zPrimaryItemRevision.get_current_id();
    String scCurrentName = zPrimaryItemRevision.get_object_string();
    String scUID = zPrimaryItemRevision.getUid();
    String ExtractionInputfilename = String.valueOf(scCurrentID);
    ExtractionInputfilename = String.valueOf(String.valueOf(ExtractionInputfilename)) + "_Input";
    File fECNInputTextFile = new File(String.valueOf(String.valueOf(this.stagingLoc)) + File.separatorChar + ExtractionInputfilename + ".txt");
    fECNInputTextFile.createNewFile();
    String SolutionItemDetailsfilename = String.valueOf(scCurrentID);
    SolutionItemDetailsfilename = String.valueOf(String.valueOf(ExtractionInputfilename)) + "_SolutionItems";
    File fSolInputTextFile = new File(String.valueOf(String.valueOf(this.stagingLoc)) + File.separatorChar + SolutionItemDetailsfilename + ".txt");
    fSolInputTextFile.createNewFile();
    File fUIDTextFile = new File(String.valueOf(String.valueOf(this.stagingLoc)) + File.separatorChar + "UID.txt");
    fUIDTextFile.createNewFile();
    FileWriter UIDWriter = new FileWriter(fUIDTextFile);
    User owningUser = (User)zPrimaryItemRevision.getPropertyObject("owning_user").getModelObjectValue();
    SoaHelper.getProperties((ModelObject)owningUser, new String[] { "person" });
    Person person = owningUser.get_person();
    SoaHelper.getProperties((ModelObject)person, new String[] { "PA9" });
    String MailID = person.get_PA9();
    try {
      FileWriter ECNWriter = new FileWriter(fECNInputTextFile);
      ECNWriter.write("uid=" + scUID + "\n");
      ECNWriter.write("ECNID=" + scCurrentID + "\n");
      ECNWriter.write("ECNName=" + scCurrentName + "\n");
      ECNWriter.write("mailID=" + MailID + "\n");
      ECNWriter.close();
      UIDWriter.write(String.valueOf(scUID) + "\n");
      SoaHelper.getProperties((ModelObject)zPrimaryItemRevision, "CMHasSolutionItem");
      SoaHelper.getProperties((ModelObject)zPrimaryItemRevision, "CMHasImpactedItem");
      Property pSecondaryObject = null;
      pSecondaryObject = zPrimaryItemRevision.getPropertyObject("CMHasSolutionItem");
      ModelObject[] mSecondaryModelObjectSol = pSecondaryObject.getModelObjectArrayValue();
      if (mSecondaryModelObjectSol.length == 0) {
        System.out.println("***" + scCurrentID + "has Impacted Items only. May be Mass Update Flow. ***\n");
        pSecondaryObject = zPrimaryItemRevision.getPropertyObject("CMHasImpactedItem");
      } 
      if (pSecondaryObject != null) {
        ModelObject[] mSecondaryModelObject = pSecondaryObject.getModelObjectArrayValue();
        SoaHelper.refresh(mSecondaryModelObject);
        FileWriter SolItemWriter = new FileWriter(fSolInputTextFile);
        for (int j = 0; j < mSecondaryModelObject.length; j++) {
          String McUID = mSecondaryModelObject[j].getUid();
          SoaHelper.getProperties((ModelObject)owningUser, "user_name");
          User solutionowningUser = (User)mSecondaryModelObject[j].getPropertyObject("owning_user").getModelObjectValue();
          SoaHelper.getProperties((ModelObject)owningUser, new String[] { "user_name" });
          SoaHelper.getProperties(mSecondaryModelObject[j], new String[] { "object_string", "object_type" });
          String SolutionUserName = solutionowningUser.get_user_name();
          String SolutionName = mSecondaryModelObject[j].getPropertyDisplayableValue("object_string");
          String SolutionType = mSecondaryModelObject[j].getPropertyDisplayableValue("object_type");
          SolItemWriter.write(String.valueOf(McUID) + "=" + SolutionName + "=" + SolutionType + "=" + SolutionUserName + "\n");
          TraverseuptoFirstLevelChild((ItemRevision)mSecondaryModelObject[j], UIDWriter);
        } 
        SolItemWriter.close();
        UIDWriter.close();
      } 
      System.out.println("File is written and closed");
    } catch (IOException e) {
      System.out.println("An error occurred in writing in File");
      e.printStackTrace();
    } 
    TranslationTask translatetask = prepTransTask(null, null, zPrimaryItemRevision, fECNInputTextFile.getName(), true, true, ".txt", 0, null);
    return addRefIdToTask(translatetask, 0);
  }
  
  
  void TraverseuptoFirstLevelChild(ItemRevision zPrimaryItemRevision, FileWriter UIDWriter) throws Exception {
    StructureManagementService zStructureManagementService = StructureManagementService.getService(ConnectionManager.getActiveConnection());
  //TCUP-167 start
  //StructureManagement.CreateBOMWindowsInfo zCreateBOMWindowsInfo = new StructureManagement.CreateBOMWindowsInfo();
  //We imported CreateWindowsInfo3 directly, since it eliminates the need to access it via the StructureManagement command.
    CreateWindowsInfo3 zCreateWindowsInfo3 = new CreateWindowsInfo3();
  //TCUP-167 end
    zCreateWindowsInfo3.item = zPrimaryItemRevision.get_items_tag();
    zCreateWindowsInfo3.itemRev = zPrimaryItemRevision;
    String sRevisionRule = "Latest Working";
    String sRevisionRuleName = new String();
    if (sRevisionRule != null) {
      StructureManagement.GetRevisionRulesResponse zGetRevisionRulesResponse = zStructureManagementService.getRevisionRules();
      StructureManagement.RevisionRuleInfo[] zRevisionRuleInfo = zGetRevisionRulesResponse.output;
      for (int j = 0; j < zRevisionRuleInfo.length; j++) {
        RevisionRule zRevisionRule = (zRevisionRuleInfo[j]).revRule;
        sRevisionRuleName = zRevisionRule.getPropertyDisplayableValue("object_name");
        if (sRevisionRule.equalsIgnoreCase(sRevisionRuleName)) {
          StructureManagement.RevisionRuleConfigInfo RevRuleConfigInfo = new StructureManagement.RevisionRuleConfigInfo();
          RevRuleConfigInfo.revRule = zRevisionRule;
          RevRuleConfigInfo.props.unitNo = -1;
          zCreateWindowsInfo3.revRuleConfigInfo = RevRuleConfigInfo;
        } 
      } 
    } 
  //TCUP-167 start
    CreateWindowsInfo3[] zcrCreateBOMWindowsInfo = { zCreateWindowsInfo3 };
    //StructureManagement.CreateBOMWindowsResponse zCreateBOMWindowsResponse = zStructureManagementService.createBOMWindows(zcrCreateBOMWindowsInfo);
   //Replaced the deprecated "createBOMWindows" method with the currently used "createOrReConfigureBOMWindows" method for BOM creation.
    StructureManagement.CreateBOMWindowsResponse zCreateBOMWindowsResponse = zStructureManagementService.createOrReConfigureBOMWindows(zcrCreateBOMWindowsInfo);
  //TCUP-167 end
    StructureManagement.CreateBOMWindowsOutput[] zCreateBOMWindowsOutput = zCreateBOMWindowsResponse.output;
    BOMLine blTopLine = (zCreateBOMWindowsOutput[0]).bomLine;
    BOMLine[] blParentLine = { blTopLine };
    StructureManagement.ExpandPSOneLevelInfo zExpandPSOneLevelInfo = new StructureManagement.ExpandPSOneLevelInfo();
    StructureManagement.ExpandPSOneLevelPref zExpandPSOneLevelPref = new StructureManagement.ExpandPSOneLevelPref();
    zExpandPSOneLevelInfo.parentBomLines = blParentLine;
    zExpandPSOneLevelInfo.excludeFilter = "None";
    zExpandPSOneLevelPref.expItemRev = false;
    StructureManagement.ExpandPSOneLevelResponse zExpandPSOneLevelResponse = zStructureManagementService.expandPSOneLevel(zExpandPSOneLevelInfo, zExpandPSOneLevelPref);
    StructureManagement.ExpandPSOneLevelOutput[] zExpandPSOneLevelOutput = zExpandPSOneLevelResponse.output;
    StructureManagement.ExpandPSData[] zExpandPSData = (zExpandPSOneLevelOutput[0]).children;
    BOMLine[] blChildlBOMines = new BOMLine[zExpandPSData.length];
    System.out.println("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" + zExpandPSData.length);
    for (int i = 0; i < zExpandPSData.length; i++) {
      blChildlBOMines[i] = (zExpandPSData[i]).bomLine;
      ItemRevision zFirstLevelItemRevision = (zExpandPSData[i]).itemRevOfBOMLine;
      String sFirstLevelUID = zFirstLevelItemRevision.getUid();
      UIDWriter.write(String.valueOf(sFirstLevelUID) + "\n");
    } 
  }
}
