#!/bin/bash

##SET DIR
STAGING_DIR=/var/www/localhost/staging
BENCHMARKS_DIR=/home/albiz/dev/
##

FILE=$1
echo $FILE >> ${STAGING_DIR}/tout
unzip $FILE -d ${STAGING_DIR}/ 

TMP=`find ${STAGING_DIR}/ -name *\.class |  sed 's/Binary\ file//g' | sed 's/matches//g' | sed 's/\.class//g' | sed 's/\/home\/albiz\/dev\/staging\///g' | sed 's/\//\./g' | head -1`
echo $TMP >> ${STAGING_DIR}/tout
export MAIN_CLASS=$TMP
export PACKAGE=$2

echo ". benchmark.sh staging ds-finder"

##USE LOCAL DSFINDER
. ${BENCHMARKS_DIR}/benchmark_hacks/benchmark.sh staging ds-finder 

cp ${BENCHMARKS_DIR}/sootOutput/logs/staging.ds-finder.log staging.ds-finder.log
cp ${BENCHMARKS_DIR}/sootOutput/logs/staging.ds-finder.log-full staging.ds-finder.log-full

mv ${STAGING_DIR}/* /tmp/
unset MAIN_CLASS
unset TMP

