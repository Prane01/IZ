@echo off
setlocal

:: Set working directory (where this script is located)
set "BASE_DIR=%~dp0"

:: Set input and output paths
set "JAR_PATH=%BASE_DIR%OrgExcel.jar"
set "INPUT_XML=%BASE_DIR%Organization.xml"
set "OUTPUT_DIR=%BASE_DIR%output"
set "OUTPUT_XLSX=%OUTPUT_DIR%\Organization_Hierarchy.xlsx"

:: Create output folder if it doesn't exist
if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
)

:: Run the JAR
echo Running OrgExcel.jar...
java -jar "%JAR_PATH%" "%INPUT_XML%" "%OUTPUT_XLSX%"

echo.
echo Output written to: %OUTPUT_XLSX%
pause
