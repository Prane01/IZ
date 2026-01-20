This batch script automates the execution of the `OrgExcel.jar` Java program that processes an XML file and exports the organizational hierarchy to an Excel file.

Files Included
1. `OrgExcel.jar` – Java program that performs the XML-to-Excel conversion  
2. `runOrgExcelJar.bat` – Batch script to execute the JAR file with required input/output paths

How to Use the Script
1. Make sure `OrgExcel.jar`, `runOrgExcelJar.bat`, and `Organization.xml` are all in the **same folder**.
2. Double-click on `runOrgExcelJar.bat`.
3. The script will:
   - **Read the XML file from**:  
     `Organization.xml` (in the same folder as the script)
   - **Write the output Excel file to**:  
     `output\Organization_Hierarchy.xlsx` (inside a subfolder named `output`)

Customizing Paths
To change the input or output paths:
1. Right-click `runOrgExcelJar.bat` → Click **Edit**  
2. Modify the following lines:
   ```bat
   set "INPUT_XML=%BASE_DIR%Organization.xml"
   set "OUTPUT_XLSX=%OUTPUT_DIR%\Organization_Hierarchy.xlsx"

Requirements:
1.Java must be installed and added to the system PATH.