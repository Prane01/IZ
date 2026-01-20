package com.teamcenter.ets.translator.ugs.graco_bomextraction_tool.load;

import com.teamcenter.ets.load.IDatabaseOperation;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.translationservice.task.TranslationDBMapInfo;
import com.teamcenter.translationservice.task.TranslationTask;
import com.teamcenter.tstk.util.log.ITaskLogger;
import java.util.List;

public class DatabaseOperation implements IDatabaseOperation {
  protected void load(TranslationDBMapInfo zDbMapInfo, List<String> zFileList) throws Exception {
    System.out.println("Nothing to upload\n");
  }
  
  public void init(ModelObject arg0, TranslationTask arg1, String arg2, ITaskLogger arg3) throws Exception {}
  
  public void processTask() throws Exception {}
  
  public void processTaskPost() throws Exception {}
  
  public void processTaskPre() throws Exception {}
  
  public boolean softFailureEncountered() {
    return false;
  }
}
