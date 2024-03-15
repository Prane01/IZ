#include<iostream>
#include<tccore/custom.h>
#include<tccore/method.h>
#include<tccore/aom.h>
#include<tccore/aom_prop.h>
#include<bom/bom_msg.h>
#include<pom/pom/pom.h>
#include<fclasses/tc_string.h>
#include<bom/bom.h>
#define DLLAPI _declspec(dllexport)
//#define PLM_error (EMH_USER_error_base +3)
using namespace std;

extern "C" {

	METHOD_id_t tmethod_id1;
	METHOD_id_t tmethod_id2;
	METHOD_id_t tmethod_id3;
	tag_t tWindow = NULLTAG;
	tag_t tTopLine = NULLTAG;
	int status = 0;
	bool cutIs = false;
	char* cutBOMLine = NULL;

	// Declaration of function
	extern DLLAPI int DLLProject01_register_callbacks();
	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv);
	extern DLLAPI int PLM_add_postAction(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_cut_preAction(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_update_property(METHOD_message_t* msg, va_list args);
	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv);
	void setProperty1(tag_t &parent);
	void setAllParentsProperty1(tag_t &parent);
	void setProperty(tag_t tParent);
	int setAllParentsProperty(tag_t tChild);

	// Definition of function
	extern DLLAPI int DLLProject01_register_callbacks() {
		status = CUSTOM_register_exit("DLLProject01", "USER_init_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback1);
		status = CUSTOM_register_exit("DLLProject01", "USER_exit_module", (CUSTOM_EXIT_ftn_t)PLM_execute_callback2);
		return status;
	}

	extern DLLAPI int PLM_execute_callback1(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "* * * Teamcenter Login Success * * *\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";

		status = METHOD_find_method("BOMLine", "BOMLine_add", &tmethod_id1);
		status = METHOD_add_action(tmethod_id1, METHOD_post_action_type,(METHOD_function_t)PLM_add_postAction, NULL);
		status = METHOD_find_method("BOMLine", "BOMLine_cut", &tmethod_id2);
		status = METHOD_add_action(tmethod_id2, METHOD_pre_action_type,(METHOD_function_t)PLM_cut_preAction, NULL);
		status = METHOD_find_method("A2BOM_ItemRevision", "IMAN_save", &tmethod_id3);
		status = METHOD_add_action(tmethod_id3, METHOD_post_action_type, (METHOD_function_t)PLM_update_property, NULL);

		return status;
	}

	extern DLLAPI int PLM_add_postAction(METHOD_message_t* msg, va_list args) {
		tag_t tParent = NULLTAG;
		tParent = va_arg(args, tag_t);
		tag_t item = NULLTAG;
		item = va_arg(args, tag_t);
		tag_t source_rev = NULLTAG;
		source_rev = va_arg(args, tag_t);
		setProperty1(tParent);
		return status;
	}

	extern DLLAPI int PLM_cut_preAction(METHOD_message_t* msg, va_list args) {
		tag_t bomLine = NULLTAG;
		bomLine = va_arg(args, tag_t);
		cutIs = true;
		AOM_ask_value_string(bomLine, "bl_indented_title", &cutBOMLine);
		cout << cutBOMLine << endl;
		setAllParentsProperty1(bomLine);
		return status;
	}

	extern DLLAPI int PLM_update_property(METHOD_message_t* msg, va_list args) {
		tag_t object = msg->object_tag;
		char* object_string = NULL;
		char* object_type = NULL;

		AOM_ask_value_string(object, "object_type", &object_type);
		if (tc_strcmp(object_type, "A2BOM_Item") != 0) {
			AOM_ask_value_string(object, "object_string", &object_string);
			if (object_string != NULL) {
				cout << object_string << endl;
				setAllParentsProperty(object);
			}
		}
		return 0;
	}

	void setProperty(tag_t tParent) {
		int iCountOfBOMChildLines = 0;
		tag_t* tChildBOMLines = NULL;
		char* cPropertyValue = NULL;
		char* cPropertyValue1 = NULL;
		char* cText = NULL;
		int iFail = ITK_ok;
		tag_t tRev = NULLTAG;
		bool isTrue = false;
		char* item_id = NULL;
		char* rev_id = NULL;

		BOM_create_window(&tWindow);
		BOM_set_window_top_line(tWindow, NULLTAG, tParent, NULLTAG, &tTopLine);
		BOM_line_ask_all_child_lines(tTopLine, &iCountOfBOMChildLines, &tChildBOMLines);
		cout << iCountOfBOMChildLines << endl;
		AOM_ask_value_string(tTopLine, "bl_item_item_id", &item_id);
		AOM_ask_value_string(tTopLine, "bl_rev_item_revision_id", &rev_id);
		ITEM_find_rev(item_id, rev_id, &tRev);
		if (tRev != NULLTAG) {
			for (int i = 0; i < iCountOfBOMChildLines; i++)
			{
				iFail = AOM_ask_value_string(tChildBOMLines[i], "bl_A2BOM_ItemRevision_a2bom_property", &cPropertyValue);
				if (iFail == ITK_ok) {
					cout << cPropertyValue << endl;
					if (tc_strcmp(cPropertyValue, "TRUE") == 0) {
						cout << "In IF Block" << endl;
						isTrue = true;
					}
				}
			}
			BOM_close_window(tWindow);
			if (isTrue) {
				AOM_refresh(tRev, true);
				iFail = AOM_set_value_string(tRev, "a2bom_property", cPropertyValue);
				if (iFail == ITK_ok) {
					AOM_save_with_extensions(tRev);
					cout << "Object Saved!" << endl;
					AOM_refresh(tRev, false);
				}
				else {
					EMH_ask_error_text(iFail, &cText);
					cout << "Object not Saved - " << cText << endl;
				}
			}
			if (isTrue == false) {
				AOM_refresh(tRev, true);
				iFail = AOM_set_value_string(tRev, "a2bom_property", "");
				if (iFail == ITK_ok) {
					AOM_save_with_extensions(tRev);
					cout << "Object Saved!" << endl;
					AOM_refresh(tRev, false);
				}
				else {
					EMH_ask_error_text(iFail, &cText);
					cout << "Object not Saved - " << cText << endl;
				}
			}
			AOM_ask_value_string(tRev, "a2bom_property", &cPropertyValue1);
			cout << cPropertyValue1 << endl;
		}
	}
	int setAllParentsProperty(tag_t tChild) {
		int n_parents = 0;
		int* levels = NULL;
		tag_t* parents = NULL;
		char* cKnowParent = NULL;
		tag_t class_id = NULLTAG;
		char* class_name = NULL;
		char* super_class_name = NULL;
		int n_ids = 0;
		tag_t* superclass_ids = NULL;
		PS_where_used_all(tChild, 1, &n_parents, &levels, &parents);
		if (parents[0] != NULLTAG) {
			POM_class_of_instance(parents[0], &class_id);
			POM_name_of_class(class_id, &class_name);
			cout << "class_name : " << class_name << endl;
			POM_superclasses_of_class(class_id, &n_ids, &superclass_ids);
			POM_name_of_class(superclass_ids[0], &super_class_name);
			cout << "super_class_name : " << super_class_name << endl;
			if (tc_strcmp(class_name, "ItemRevision") == 0 || tc_strcmp(super_class_name, "ItemRevision") == 0) {
				AOM_ask_value_string(parents[0], "object_string", &cKnowParent);
				cout << "Parent : " << cKnowParent << endl;
				setProperty(parents[0]);
			}
		}
		return 0;
	}
	void setProperty1(tag_t &tParent) {
		int iCountOfBOMChildLines = 0;
		tag_t* tChildBOMLines = NULL;
		char* cPropertyValue = NULL;
		bool bFlag = false;
		int counter = 0;
		char* checkCutLine = NULL;
		BOM_line_ask_all_child_lines(tParent, &iCountOfBOMChildLines, &tChildBOMLines);
		cout << iCountOfBOMChildLines << endl;
		for (int i = 0; i < iCountOfBOMChildLines; i++)
		{
			AOM_ask_value_string(tChildBOMLines[i], "bl_A2BOM_ItemRevision_a2bom_property", &cPropertyValue);
			cout << cPropertyValue << endl;
			AOM_ask_value_string(tChildBOMLines[i], "bl_indented_title", &checkCutLine);
			cout << checkCutLine << endl;
			if (tc_strcmp(checkCutLine, cutBOMLine) != 0)
			{
				if (tc_strcmp(cPropertyValue, "TRUE") == 0) {
					AOM_refresh(tParent, true);
					AOM_set_value_string(tParent, "bl_A2BOM_ItemRevision_a2bom_property", cPropertyValue);
					AOM_save_without_extensions(tParent);
					AOM_refresh(tParent, false);
					bFlag = true;
				}
				if (bFlag)
				{
					setAllParentsProperty1(tParent);
					break;
				}
				else {
					counter++;
				}
				if (cutIs && counter == iCountOfBOMChildLines-1)
				{
					AOM_refresh(tParent, true);
					AOM_set_value_string(tParent, "bl_A2BOM_ItemRevision_a2bom_property", "");
					AOM_save_without_extensions(tParent);
					AOM_refresh(tParent, false);
					setAllParentsProperty1(tParent);
				}
			}
		}
	}

	void setAllParentsProperty1(tag_t &tParent) {
		tag_t tParentOfP = NULLTAG;
		char* cKnowParent = NULL;
		AOM_ask_value_tag(tParent, "bl_parent", &tParentOfP);
		if (tParentOfP != NULLTAG) {
			AOM_ask_value_string(tParentOfP, "bl_indented_title", &cKnowParent);
			cout << "Parent : " << cKnowParent << endl;
			setProperty1(tParentOfP);
		}
	}

	extern DLLAPI int PLM_execute_callback2(int* decision, va_list argv) {
		*decision = ALL_CUSTOMIZATIONS;
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		cout << "***** Teamcenter Logout Success *****\n\n";
		cout << "* * * * * * * * * * * * * * * * * * * * \n\n";
		return status;
	}
}