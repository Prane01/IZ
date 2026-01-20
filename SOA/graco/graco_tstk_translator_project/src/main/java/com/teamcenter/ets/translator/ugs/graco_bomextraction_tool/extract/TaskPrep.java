/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  com.teamcenter.ets.extract.DefaultTaskPrep
 *  com.teamcenter.ets.soa.ConnectionManager
 *  com.teamcenter.ets.soa.SoaHelper
 *  com.teamcenter.services.strong.cad.StructureManagementService
 *  com.teamcenter.services.strong.cad._2007_01.StructureManagement$CreateBOMWindowsOutput
 *  com.teamcenter.services.strong.cad._2007_01.StructureManagement$CreateBOMWindowsResponse
 *  com.teamcenter.services.strong.cad._2007_01.StructureManagement$ExpandPSData
 *  com.teamcenter.services.strong.cad._2007_01.StructureManagement$ExpandPSOneLevelInfo
 *  com.teamcenter.services.strong.cad._2007_01.StructureManagement$ExpandPSOneLevelOutput
 *  com.teamcenter.services.strong.cad._2007_01.StructureManagement$ExpandPSOneLevelPref
 *  com.teamcenter.services.strong.cad._2007_01.StructureManagement$ExpandPSOneLevelResponse
 *  com.teamcenter.services.strong.cad._2007_01.StructureManagement$GetRevisionRulesResponse
 *  com.teamcenter.services.strong.cad._2007_01.StructureManagement$RevisionRuleConfigInfo
 *  com.teamcenter.services.strong.cad._2007_01.StructureManagement$RevisionRuleInfo
 *  com.teamcenter.services.strong.cad._2019_06.StructureManagement$CreateWindowsInfo3
 *  com.teamcenter.services.strong.core.DataManagementService
 *  com.teamcenter.soa.client.Connection
 *  com.teamcenter.soa.client.model.ModelObject
 *  com.teamcenter.soa.client.model.Property
 *  com.teamcenter.soa.client.model.strong.BOMLine
 *  com.teamcenter.soa.client.model.strong.ItemRevision
 *  com.teamcenter.soa.client.model.strong.Person
 *  com.teamcenter.soa.client.model.strong.RevisionRule
 *  com.teamcenter.soa.client.model.strong.User
 *  com.teamcenter.translationservice.task.TranslationTask
 */
package com.teamcenter.ets.translator.ugs.graco_bomextraction_tool.extract;

import com.teamcenter.ets.extract.DefaultTaskPrep;
import com.teamcenter.ets.soa.ConnectionManager;
import com.teamcenter.ets.soa.SoaHelper;
import com.teamcenter.services.strong.cad.StructureManagementService;
import com.teamcenter.services.strong.cad._2007_01.StructureManagement;
import com.teamcenter.services.strong.cad._2019_06.StructureManagement;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.soa.client.Connection;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.Property;
import com.teamcenter.soa.client.model.strong.BOMLine;
import com.teamcenter.soa.client.model.strong.ItemRevision;
import com.teamcenter.soa.client.model.strong.Person;
import com.teamcenter.soa.client.model.strong.RevisionRule;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.translationservice.task.TranslationTask;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class TaskPrep
extends DefaultTaskPrep {
    public TranslationTask prepareTask() throws Exception {
        ModelObject[] zPrimaryObjs = this.request.getPropertyObject("primaryObjects").getModelObjectArrayValue();
        this.request.getPropertyObject("providerName").getStringValue();
        this.request.getPropertyObject("serviceName").getStringValue();
        DataManagementService.getService((Connection)ConnectionManager.getActiveConnection());
        ItemRevision zPrimaryItemRevision = (ItemRevision)zPrimaryObjs[0];
        SoaHelper.getProperties((ModelObject)zPrimaryItemRevision, (String[])new String[]{"current_id", "object_string"});
        String scCurrentID = zPrimaryItemRevision.get_current_id();
        String scCurrentName = zPrimaryItemRevision.get_object_string();
        String scUID = zPrimaryItemRevision.getUid();
        String ExtractionInputfilename = String.valueOf(scCurrentID);
        ExtractionInputfilename = String.valueOf(String.valueOf(String.valueOf(ExtractionInputfilename))) + "_Input";
        File fECNInputTextFile = new File(String.valueOf(String.valueOf(String.valueOf(this.stagingLoc))) + File.separatorChar + ExtractionInputfilename + ".txt");
        fECNInputTextFile.createNewFile();
        String SolutionItemDetailsfilename = String.valueOf(scCurrentID);
        SolutionItemDetailsfilename = String.valueOf(String.valueOf(String.valueOf(ExtractionInputfilename))) + "_SolutionItems";
        File fSolInputTextFile = new File(String.valueOf(String.valueOf(String.valueOf(this.stagingLoc))) + File.separatorChar + SolutionItemDetailsfilename + ".txt");
        fSolInputTextFile.createNewFile();
        File fUIDTextFile = new File(String.valueOf(String.valueOf(String.valueOf(this.stagingLoc))) + File.separatorChar + "UID.txt");
        fUIDTextFile.createNewFile();
        FileWriter UIDWriter = new FileWriter(fUIDTextFile);
        User owningUser = (User)zPrimaryItemRevision.getPropertyObject("owning_user").getModelObjectValue();
        SoaHelper.getProperties((ModelObject)owningUser, (String[])new String[]{"person"});
        Person person = owningUser.get_person();
        SoaHelper.getProperties((ModelObject)person, (String[])new String[]{"PA9"});
        String MailID = person.get_PA9();
        try {
            FileWriter ECNWriter = new FileWriter(fECNInputTextFile);
            ECNWriter.write("uid=" + scUID + "\n");
            ECNWriter.write("ECNID=" + scCurrentID + "\n");
            ECNWriter.write("ECNName=" + scCurrentName + "\n");
            ECNWriter.write("mailID=" + MailID + "\n");
            ECNWriter.close();
            UIDWriter.write(String.valueOf(String.valueOf(scUID)) + "\n");
            SoaHelper.getProperties((ModelObject)zPrimaryItemRevision, (String)"CMHasSolutionItem");
            SoaHelper.getProperties((ModelObject)zPrimaryItemRevision, (String)"CMHasImpactedItem");
            Property pSecondaryObject = null;
            pSecondaryObject = zPrimaryItemRevision.getPropertyObject("CMHasSolutionItem");
            ModelObject[] mSecondaryModelObjectSol = pSecondaryObject.getModelObjectArrayValue();
            if (mSecondaryModelObjectSol.length == 0) {
                System.out.println("***" + scCurrentID + "has Impacted Items only. May be Mass Update Flow. ***\n");
                pSecondaryObject = zPrimaryItemRevision.getPropertyObject("CMHasImpactedItem");
            }
            if (pSecondaryObject != null) {
                ModelObject[] mSecondaryModelObject = pSecondaryObject.getModelObjectArrayValue();
                SoaHelper.refresh((ModelObject[])mSecondaryModelObject);
                FileWriter SolItemWriter = new FileWriter(fSolInputTextFile);
                int j = 0;
                while (j < mSecondaryModelObject.length) {
                    String McUID = mSecondaryModelObject[j].getUid();
                    SoaHelper.getProperties((ModelObject)owningUser, (String)"user_name");
                    User solutionowningUser = (User)mSecondaryModelObject[j].getPropertyObject("owning_user").getModelObjectValue();
                    SoaHelper.getProperties((ModelObject)owningUser, (String[])new String[]{"user_name"});
                    SoaHelper.getProperties((ModelObject)mSecondaryModelObject[j], (String[])new String[]{"object_string", "object_type"});
                    String SolutionUserName = solutionowningUser.get_user_name();
                    String SolutionName = mSecondaryModelObject[j].getPropertyDisplayableValue("object_string");
                    String SolutionType = mSecondaryModelObject[j].getPropertyDisplayableValue("object_type");
                    SolItemWriter.write(String.valueOf(String.valueOf(McUID)) + "=" + SolutionName + "=" + SolutionType + "=" + SolutionUserName + "\n");
                    this.TraverseuptoFirstLevelChild((ItemRevision)mSecondaryModelObject[j], UIDWriter);
                    ++j;
                }
                SolItemWriter.close();
                UIDWriter.close();
            }
            System.out.println("File is written and closed");
        }
        catch (IOException e) {
            System.out.println("An error occurred in writing in File");
            e.printStackTrace();
        }
        TranslationTask translatetask = this.prepTransTask(null, null, zPrimaryItemRevision, fECNInputTextFile.getName(), true, true, ".txt", 0, null);
        return this.addRefIdToTask(translatetask, 0);
    }

    void TraverseuptoFirstLevelChild(ItemRevision zPrimaryItemRevision, FileWriter UIDWriter) throws Exception {
        StructureManagementService zStructureManagementService = StructureManagementService.getService((Connection)ConnectionManager.getActiveConnection());
        StructureManagement.CreateWindowsInfo3 zCreateWindowsInfo3 = new StructureManagement.CreateWindowsInfo3();
        zCreateWindowsInfo3.item = zPrimaryItemRevision.get_items_tag();
        zCreateWindowsInfo3.itemRev = zPrimaryItemRevision;
        String sRevisionRule = "Latest Working";
        String sRevisionRuleName = new String();
        if (sRevisionRule != null) {
            StructureManagement.GetRevisionRulesResponse zGetRevisionRulesResponse = zStructureManagementService.getRevisionRules();
            StructureManagement.RevisionRuleInfo[] zRevisionRuleInfo = zGetRevisionRulesResponse.output;
            int j = 0;
            while (j < zRevisionRuleInfo.length) {
                RevisionRule zRevisionRule = zRevisionRuleInfo[j].revRule;
                sRevisionRuleName = zRevisionRule.getPropertyDisplayableValue("object_name");
                if (sRevisionRule.equalsIgnoreCase(sRevisionRuleName)) {
                    StructureManagement.RevisionRuleConfigInfo RevRuleConfigInfo = new StructureManagement.RevisionRuleConfigInfo();
                    RevRuleConfigInfo.revRule = zRevisionRule;
                    RevRuleConfigInfo.props.unitNo = -1;
                    zCreateWindowsInfo3.revRuleConfigInfo = RevRuleConfigInfo;
                }
                ++j;
            }
        }
        StructureManagement.CreateWindowsInfo3[] zcrCreateBOMWindowsInfo = new StructureManagement.CreateWindowsInfo3[]{zCreateWindowsInfo3};
        StructureManagement.CreateBOMWindowsResponse zCreateBOMWindowsResponse = zStructureManagementService.createOrReConfigureBOMWindows(zcrCreateBOMWindowsInfo);
        StructureManagement.CreateBOMWindowsOutput[] zCreateBOMWindowsOutput = zCreateBOMWindowsResponse.output;
        BOMLine blTopLine = zCreateBOMWindowsOutput[0].bomLine;
        BOMLine[] blParentLine = new BOMLine[]{blTopLine};
        StructureManagement.ExpandPSOneLevelInfo zExpandPSOneLevelInfo = new StructureManagement.ExpandPSOneLevelInfo();
        StructureManagement.ExpandPSOneLevelPref zExpandPSOneLevelPref = new StructureManagement.ExpandPSOneLevelPref();
        zExpandPSOneLevelInfo.parentBomLines = blParentLine;
        zExpandPSOneLevelInfo.excludeFilter = "None";
        zExpandPSOneLevelPref.expItemRev = false;
        StructureManagement.ExpandPSOneLevelResponse zExpandPSOneLevelResponse = zStructureManagementService.expandPSOneLevel(zExpandPSOneLevelInfo, zExpandPSOneLevelPref);
        StructureManagement.ExpandPSOneLevelOutput[] zExpandPSOneLevelOutput = zExpandPSOneLevelResponse.output;
        StructureManagement.ExpandPSData[] zExpandPSData = zExpandPSOneLevelOutput[0].children;
        BOMLine[] blChildlBOMines = new BOMLine[zExpandPSData.length];
        System.out.println("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" + zExpandPSData.length);
        int i = 0;
        while (i < zExpandPSData.length) {
            blChildlBOMines[i] = zExpandPSData[i].bomLine;
            ItemRevision zFirstLevelItemRevision = zExpandPSData[i].itemRevOfBOMLine;
            String sFirstLevelUID = zFirstLevelItemRevision.getUid();
            UIDWriter.write(String.valueOf(String.valueOf(sFirstLevelUID)) + "\n");
            ++i;
        }
    }
}

