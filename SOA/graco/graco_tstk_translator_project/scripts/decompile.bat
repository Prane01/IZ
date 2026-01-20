@echo off
if "%1"=="" (
  echo Usage: decompile.bat path\to\cfr.jar
  exit /b 1
)
set CFR=%1
set JAR=..\lib\graco_tstk_translator.jar
set OUT=..\src\main\java
echo Decompiling %JAR% ...
java -jar %CFR% %JAR% --outputdir %OUT%
pause
