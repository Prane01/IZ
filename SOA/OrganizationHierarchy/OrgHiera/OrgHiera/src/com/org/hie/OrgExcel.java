package com.org.hie;

import org.apache.poi.ss.usermodel.*;
import org.apache.poi.ss.util.CellRangeAddress;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;
import org.w3c.dom.*;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.*;
import java.util.*;

public class OrgExcel {

    static class GroupEntry {
        String group;
        String subGroup;
        List<String> roles;

        public GroupEntry(String group, String subGroup, List<String> roles) {
            this.group = group;
            this.subGroup = subGroup;
            this.roles = roles;
        }
    }

    public static void main(String[] args) {
    	
//        String xmlPath = "D:\\Siemens\\Teamcenter2406\\Export\\Output\\OrgData2\\ADMINISTRATION_DATA\\Organization\\Organization.xml";
//        String excelPath = "D:\\Siemens\\Teamcenter2406\\Export\\Output\\OrgData2\\ADMINISTRATION_DATA\\Organization\\Organization_Hierarchy.xlsx";

        String xmlPath = args[0];
        String excelPath = args[1];
        
        try {
            // Setup XML parsing
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            DocumentBuilder builder = factory.newDocumentBuilder();
            Document doc = builder.parse(new File(xmlPath));
            doc.getDocumentElement().normalize();

            Map<String, String> roleIdToLabel = new HashMap<>();
            Map<String, String> groupMemberRoleMap = new HashMap<>();
            Set<String> unresolvedRoleIds = new HashSet<>();

            // 1. Build RoleInOwningGroup map
            NodeList allRoles = doc.getElementsByTagName("RoleInOwningGroup");
            for (int i = 0; i < allRoles.getLength(); i++) {
                Element roleElem = (Element) allRoles.item(i);
                String roleRef = roleElem.getAttribute("role_ref").trim();
                NodeList identities = roleElem.getElementsByTagName("GSIdentity");
                if (identities.getLength() > 0) {
                    String label = ((Element) identities.item(0)).getAttribute("label").trim();
                    roleIdToLabel.put(roleRef, label);
                }
            }

            // 2. Build fallback role map from GroupMember
            NodeList groupMembers = doc.getElementsByTagName("GroupMember");
            for (int i = 0; i < groupMembers.getLength(); i++) {
                Element gm = (Element) groupMembers.item(i);
                String roleRef = gm.getAttribute("role").trim();
                NodeList ids = gm.getElementsByTagName("GSIdentity");
                if (ids.getLength() > 0) {
                    String label = ((Element) ids.item(0)).getAttribute("label").trim();
                    if (!label.isEmpty()) {
                        String[] parts = label.split("\\|");
                        String lastPart = parts[parts.length - 1].trim();
                        groupMemberRoleMap.put(roleRef, lastPart);
                    }
                }
            }

            // 3. Process <Group> entries
            List<GroupEntry> groupEntries = new ArrayList<>();
            Set<String> allResolvedRoles = new HashSet<>();
            NodeList groupNodes = doc.getElementsByTagName("Group");

            for (int i = 0; i < groupNodes.getLength(); i++) {
                Element groupElem = (Element) groupNodes.item(i);

                NodeList identityList = groupElem.getElementsByTagName("GSIdentity");
                if (identityList.getLength() == 0) continue;

                Element identity = (Element) identityList.item(0);
                String label = identity.getAttribute("label").trim();

                String group = "";
                String subGroup = "";

                String[] parts = label.split("\\|");
                List<String> cleaned = new ArrayList<>();
                for (String part : parts) {
                    if (!part.trim().isEmpty()) {
                        cleaned.add(part.trim());
                    }
                }

                if (cleaned.size() == 1) {
                    group = cleaned.get(0);
                } else if (cleaned.size() >= 2) {
                    subGroup = cleaned.get(0);
                    group = cleaned.get(1);
                }

                String listOfRoles = groupElem.getAttribute("list_of_role").trim();
                List<String> roles = new ArrayList<>();

                if (!listOfRoles.isEmpty()) {
                    String[] roleIds = listOfRoles.split(",");
                    for (String roleId : roleIds) {
                        roleId = roleId.trim();
                        String labelValue = roleIdToLabel.get(roleId);

                        // If not found, use fallback
                        if (labelValue == null || labelValue.isEmpty()) {
                            labelValue = groupMemberRoleMap.get(roleId);
                        }

                        if (labelValue != null && !labelValue.isEmpty()) {
                            roles.add(labelValue);
                            allResolvedRoles.add(labelValue);
                        } else {
                            unresolvedRoleIds.add(roleId);
                        }
                    }
                }

                groupEntries.add(new GroupEntry(group, subGroup, roles));
            }

            // 4. Create Excel file
            Workbook workbook = new XSSFWorkbook();
            Sheet sheet = workbook.createSheet("Organization Hierarchy");

            CellStyle boldStyle = workbook.createCellStyle();
            Font boldFont = workbook.createFont();
            boldFont.setBold(true);
            boldStyle.setFont(boldFont);

            int rowIdx = 0;

            Row titleRow = sheet.createRow(rowIdx++);
            Cell titleCell = titleRow.createCell(0);
            titleCell.setCellValue("Organization Hierarchy (Group, Sub-Group, Role)");
            titleCell.setCellStyle(boldStyle);
            sheet.addMergedRegion(new CellRangeAddress(0, 0, 0, 2));

            Row header = sheet.createRow(rowIdx++);
            String[] headers = {"Group", "Sub-Group", "Role"};
            for (int i = 0; i < headers.length; i++) {
                Cell cell = header.createCell(i);
                cell.setCellValue(headers[i]);
                cell.setCellStyle(boldStyle);
            }

            Map<String, List<GroupEntry>> groupMap = new LinkedHashMap<>();
            for (GroupEntry entry : groupEntries) {
                groupMap.computeIfAbsent(entry.group, k -> new ArrayList<>()).add(entry);
            }

            for (Map.Entry<String, List<GroupEntry>> groupBlock : groupMap.entrySet()) {
                String group = groupBlock.getKey();
                List<GroupEntry> entries = groupBlock.getValue();

                boolean groupPrinted = false;

                for (GroupEntry entry : entries) {
                    if (!groupPrinted) {
                        Row groupRow = sheet.createRow(rowIdx++);
                        groupRow.createCell(0).setCellValue(group);
                        groupPrinted = true;
                    }

                    if (entry.subGroup != null && !entry.subGroup.isEmpty()) {
                        Row subGroupRow = sheet.createRow(rowIdx++);
                        subGroupRow.createCell(1).setCellValue(entry.subGroup);
                    }

                    for (String role : entry.roles) {
                        Row roleRow = sheet.createRow(rowIdx++);
                        roleRow.createCell(2).setCellValue(role);
                    }
                }
            }

            for (int i = 0; i < 3; i++) {
                sheet.autoSizeColumn(i);
            }

            try (FileOutputStream fos = new FileOutputStream(excelPath)) {
                workbook.write(fos);
            }
            workbook.close();

            System.out.println("Excel generated at: " + excelPath);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
