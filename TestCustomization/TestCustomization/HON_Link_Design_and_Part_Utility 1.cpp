#include"UtilityHeader 2.h"


int getPrevRevision(tag_t target, tag_t* prevRevision);
int linkObjects(tag_t target, char* objType, tag_t prevRevision);

std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
}
	
int ITK_user_main(int argc, char *argv[]) {


		int iFail=0;
		string text;

		char* groupName = NULL;
		tag_t groupTag = NULLTAG;


		text = "\n *********************** Entered into the function ************************\n";
		writeToFile(text);

		char* username = ITK_ask_cli_argument("-username=");
		char* group = ITK_ask_cli_argument("-group=");
		char* password= ITK_ask_cli_argument("-password=");
		
		char* targetObjUID = ITK_ask_cli_argument("-targetObjUID=");
		char* includeType =  ITK_ask_cli_argument("-includeType=");

		string targetObjUIDStr = "";
		targetObjUIDStr+=targetObjUID;

		string includeTypeStr="";
		includeTypeStr += includeType;

		text = "\nThe target object UIDs are "+targetObjUIDStr;
		writeToFile(text);

		vector<string> attachmentUIDs = split(targetObjUIDStr,'~');

		text="\n*********** Before the logging in ********************\n";
		writeToFile(text);

		ITK_CALL(iFail = ITK_init_module(username,password,group));

		text="\n*********** Logged in Successfully!! ********************\n";
		writeToFile(text);

		POM_ask_group(&groupName, &groupTag);
		string groupNameStr(groupName);

		text = "\n--------------The login group is -------------"+ groupNameStr;
		writeToFile(text);
		

		ITK_CALL(iFail = ITK_set_bypass(true));
    
			for(int i=0;i<attachmentUIDs.size();i++)
			{
				tag_t tPrevRev = NULLTAG;
				tag_t tAttachment = NULLTAG;
				char* objType = NULL;
				char* object_string = NULL;

				text = "\nThe attachment UID is : "+ attachmentUIDs[i];
				writeToFile(text);

				ITK__convert_uid_to_tag(attachmentUIDs[i].c_str(), &tAttachment);
				ITK_CALL(iFail = AOM_ask_value_string(tAttachment, "object_string", &object_string));
				string object_stringStr(object_string);

				text = "\nThe attachment object string is :"+ object_stringStr;
				writeToFile(text);
					
				ITK_CALL( iFail = WSOM_ask_object_type2	(tAttachment,&objType));
				TC_write_syslog(" Target Type = %s \n", objType);
					
				if(tc_strstr(includeType,objType) != 0) //if not found returns zero if found returns non-zero
				{
					text="\nTarget type is matched";
			 		writeToFile(text);

					ITK_CALL(iFail = getPrevRevision(tAttachment,&tPrevRev));
					TC_write_syslog("tPrevRev is %d \n",tPrevRev);
					text = "\ntPrevRev\n";
					writeToFile(text);
						
					if(tPrevRev != NULLTAG)
					{
						ITK_CALL(iFail = linkObjects(tAttachment,objType,tPrevRev));
						TC_write_syslog("linkedObjects is %d \n",iFail);

						text = "\nlinkedObjects\n";
						writeToFile(text);
					}
				}
					
				HON_free_memory(objType);
			}

		text="\n*********** Exiting from deleteRelationsITKmain ********************\n\n\n\n";
		writeToFile(text);

			ITK_CALL(iFail = ITK_set_bypass(false));

	return iFail;

}

    
/*
    Method Name: getPrevRevision
    Args:
    tag_t target  (I)  - Target attachment
    tag_t* prevRevision (O) - Previous Revision of the target
*/

int getPrevRevision(tag_t target, tag_t* prevRevision)
{
	int iFail = ITK_ok;
	
	int nRevs = 0;
	int *order = NULL;
	int nSortedRevs = 0;
				  
				  
	tag_t tItem = NULLTAG;
	tag_t *tRev = NULL;
	tag_t tType = NULLTAG;
	tag_t tProp = NULLTAG;
	tag_t tBaseProp = NULLTAG;
	tag_t tTypeProp = NULLTAG;
	tag_t *tSortedRevs = NULL;
	string text;
	
	try
	{

		text = "\nEntered into the GetPrevisionRevision function\n";
		writeToFile(text);

        TC_write_syslog("\nEntered into getPrevRevision\n");
		
		ITK_CALL( iFail = ITEM_ask_item_of_rev(target, &tItem));
		TC_write_syslog("tItem is %d \n",tItem);
		
		ITK_CALL( iFail = ITEM_list_all_revs(tItem, &nRevs, &tRev));
		TC_write_syslog("\nNumber of Revisions is %d \n",nRevs);
		
		if(nRevs>1)
		{
			ITK_CALL( iFail = TCTYPE_find_type("ItemRevision", "ItemRevision", &tType));
			TC_write_syslog("Type Tag is %d \n",tType);
			
			ITK_CALL( iFail = TCTYPE_ask_property_by_name(tType, "creation_date", &tProp));
			TC_write_syslog("Property Tag is %d \n",tProp);
			
			ITK_CALL( iFail = PROPDESC_ask_base_descriptor( tProp, &tBaseProp, &tTypeProp));
			TC_write_syslog("Base Property Tag is %d \n",tBaseProp);
			
			order = (int *) MEM_alloc(sizeof(int));
			order[0] = 0;

			ITK_CALL( iFail = AOM_sort_tags_by_properties(nRevs, tRev, 1, &tBaseProp, order, &nSortedRevs, &tSortedRevs));
			TC_write_syslog("\n Number of Sorted Revisions are %d\n",nSortedRevs);
			
			*prevRevision = tSortedRevs[nSortedRevs-2];
		}
		else
		{
			TC_write_syslog("\nOnly one revision present \n");
		}
	}
	catch(...)
	{
		if ( iFail == ITK_ok )
		{
			TC_write_syslog("\nHON_link_Design_and_part_on_release:getPrevRevision: Unhandled Exception.\n");
			iFail = HON_GENERIC_ERROR;
		}
	}
	
	HON_free_memory(tRev);
	HON_free_memory(order);
	HON_free_memory(tSortedRevs);

	text = "\nExited from GetPrevisionRevision function\n";
	writeToFile(text);

	text = "\nIfail value before exit from GetPrevisionRevision";
	writeToFile(text);
	
	return iFail;
}

/*
    Method Name: linkObjects
    Args:
    tag_t target  (I)  - Target attachment
	char* objType  (I)  - Target type
    tag_t prevRevision (I) - Previous Revision of the target
*/

int linkObjects(tag_t target, char* objType, tag_t prevRevision)
{
	int iFail = ITK_ok;
	int nSecObjs = 0;
	int k = 0;
	
	string text;
	char** secObjStatus = NULL;
							
	tag_t tRepByRelationType = NULLTAG;
	tag_t* tSecondaryAttachments = NULL;
	tag_t tRepByOldRelation = NULLTAG;
	tag_t tRepByNewRelation = NULLTAG;
	
	try
	{

		
		text = "\nEntered into linkObjects\n";
		writeToFile(text);


        TC_write_syslog("\nEntered into linkObjects\n");
		
		ITK_CALL( iFail = GRM_find_relation_type("TC_Is_Represented_By", &tRepByRelationType));
		TC_write_syslog("tRepByRelationType is %d \n",tRepByRelationType);
		
		if(tc_strcmp(objType, "H4_Com_Hon_PartRevision") == 0)
		{
			ITK_CALL( iFail = GRM_list_secondary_objects_only(prevRevision,tRepByRelationType,&nSecObjs,&tSecondaryAttachments));
			TC_write_syslog("Number of Drawings/Designs found under Part are %d \n",nSecObjs);
		}
		else if(tc_strcmp(objType, "H4_hon_drawingRevision") == 0 || tc_strcmp(objType, "H4_hon_designRevision") == 0)
		{
			ITK_CALL( iFail = AOM_ask_value_tags(prevRevision, "representation_for", &nSecObjs, &tSecondaryAttachments));
			TC_write_syslog("Number of parts reference to Drawing or Design are %d \n",nSecObjs);
		}
		else
		{
			TC_write_syslog("Target is not a hon part or drawing or design\n");
		}
		
		
		if(nSecObjs>0)
		{
			/*ITK_CALL( iFail = AOM_refresh(prevRevision, TRUE));
			TC_write_syslog("Refreshed PreviousRevision is %d \n",iFail);
			ITK_CALL( iFail = AOM_refresh(target, TRUE));
			TC_write_syslog("Refreshed target is %d \n",iFail);*/
			
			for( k=0; k<nSecObjs;k++)
			{
				tRepByOldRelation = NULLTAG;
				tRepByNewRelation = NULLTAG;
				int num_values=0;
				//ITK_CALL( iFail = AOM_UIF_ask_value(tSecondaryAttachments[k], "h4_release_status", &secObjStatus));
				ITK_CALL( iFail = AOM_ask_displayable_values(tSecondaryAttachments[k], "h4_release_status", &num_values, &secObjStatus));
				TC_write_syslog("Secondary Attachment release status are %s \n",secObjStatus);
				
				//if(tc_strcasecmp(secObjStatus[0], "Obsolete")!=0)
				if((secObjStatus == NULL) || (tc_strcasecmp(secObjStatus[0], "Obsolete")!=0))
				{
					//ITK_CALL( iFail = AOM_refresh(tSecondaryAttachments[k], TRUE));
					//TC_write_syslog("Refreshed Secondary Attachments is %d \n",iFail);
					
					if(tc_strcmp(objType, "H4_Com_Hon_PartRevision") == 0)
					{
						ITK_CALL( iFail = GRM_find_relation(prevRevision,tSecondaryAttachments[k],tRepByRelationType,&tRepByOldRelation));
						TC_write_syslog("tRepByOldRelation is %d \n",tRepByOldRelation);
						
						ITK_CALL( iFail = GRM_delete_relation(tRepByOldRelation));
						TC_write_syslog("Removed Relation is %d \n",iFail);
						
						ITK_CALL( iFail = GRM_find_relation(target,tSecondaryAttachments[k],tRepByRelationType,&tRepByNewRelation));
						TC_write_syslog("tRepByNewRelation is %d \n",tRepByNewRelation);
						
						if(tRepByNewRelation == NULLTAG)
						{
							ITK_CALL( iFail = GRM_create_relation(target,tSecondaryAttachments[k],tRepByRelationType,NULLTAG,&tRepByNewRelation));
							TC_write_syslog("tRepByNewRelation is %d \n",tRepByNewRelation);
							
							ITK_CALL( iFail = GRM_save_relation(tRepByNewRelation));
							TC_write_syslog("Saved new Relation is %d \n",iFail);
						}
						else
						{
							TC_write_syslog("Drawing/Design already exists under Part\n");
						}
						
						
					}
					else if(tc_strcmp(objType, "H4_hon_drawingRevision") == 0 || tc_strcmp(objType, "H4_hon_designRevision") == 0)
					{
						ITK_CALL( iFail = GRM_find_relation(tSecondaryAttachments[k],prevRevision,tRepByRelationType, &tRepByOldRelation));
						TC_write_syslog("tRepByOldRelation is %d \n",tRepByOldRelation);
						
						ITK_CALL( iFail = GRM_delete_relation(tRepByOldRelation));
						TC_write_syslog("Removed Relation is %d \n",iFail);
						
						ITK_CALL( iFail = GRM_find_relation(tSecondaryAttachments[k],target,tRepByRelationType,&tRepByNewRelation));
						TC_write_syslog("tRepByNewRelation is %d \n",tRepByNewRelation);
						
						if(tRepByNewRelation == NULLTAG)
						{
							ITK_CALL( iFail = GRM_create_relation(tSecondaryAttachments[k],target,tRepByRelationType,NULLTAG,&tRepByNewRelation));
							TC_write_syslog("tRepByNewRelation is %d \n",tRepByNewRelation);
							
							ITK_CALL( iFail = GRM_save_relation(tRepByNewRelation));
							TC_write_syslog("Saved new Relation is %d \n",iFail);

						}
						else
						{
							TC_write_syslog("Part already exists under Drawing/Design\n");
							text="\n Part already exists under Drawing/Design\n ";
							writeToFile(text);
						}
					}
					else
					{
						TC_write_syslog("Target is not a part or drawing or design\n");
						text="\n Target is not a part or drawing or design\n ";
					writeToFile(text);
						
					}
					
					//ITK_CALL( iFail = AOM_save(tSecondaryAttachments[k]));
					//TC_write_syslog("Saved Secondary Attachments is %d \n",iFail);
					
					//ITK_CALL( iFail = AOM_refresh(tSecondaryAttachments[k], FALSE));
					//TC_write_syslog("Refreshed Secondary Attachments is %d \n",iFail);
				}
				else
				{
					TC_write_syslog("Secondary object is Obsoleted\n");
					text="\n Secondary object is Obsoleted\n ";
				writeToFile(text);
				}

				HON_free_memory(secObjStatus);
				
			}
		}
		
		/*ITK_CALL( iFail = AOM_refresh(prevRevision, FALSE));
		TC_write_syslog("Refreshed PreviousRevision is %d \n",iFail);
		ITK_CALL( iFail = AOM_refresh(target, FALSE));
		TC_write_syslog("Refreshed target is %d \n",iFail);*/
	
	}
	catch(...)
	{
		if ( iFail == ITK_ok )
		{
			TC_write_syslog("HON_link_Design_and_part_on_release:linkObjects: Unhandled Exception.\n");
			iFail = HON_GENERIC_ERROR;

			text="\n HON_link_Design_and_part_on_release:linkObjects: Unhandled Exception.\n ";
			writeToFile(text);
		}
	}
	
	HON_free_memory(tSecondaryAttachments);

	text="\n iFail value before exit from LinkObjects Function is  ";
	writeToFile(text);

	return iFail;
}