#!/bin/bash
if [ -z "$1" ]; then
  echo "Usage: ./decompile.sh /path/to/cfr.jar"
  exit 1
fi
CFR=$1
JAR=../lib/graco_tstk_translator.jar
OUT=../src/main/java
echo "Decompiling $JAR ..."
java -jar "$CFR" "$JAR" --outputdir "$OUT"
