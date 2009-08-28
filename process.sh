#!/bin/bash

##SET DIR
STAGING_DIR=
BENCHMARKS_DIR= ##
##

FILE=$1
export PACKAGE=$2
export PROJECT_ROOT=$STAGING_DIR
export CLASSES_DIR=$FILE

##USE LOCAL DSFINDER
cd ${BENCHMARKS_DIR}/scripts
. ${BENCHMARKS_DIR}/scripts/benchmark.sh staging ds-finder

cp ${BENCHMARKS_DIR}/Output/logs/staging.ds-finder.log staging.ds-finder.log
cp ${BENCHMARKS_DIR}/Output/logs/staging.ds-finder.log-full staging.ds-finder.log-full
rm -f ${BENCHMARKS_DIR}/Output/logs/staging.ds-finder.log 
rm -f ${BENCHMARKS_DIR}/Output/logs/staging.ds-finder.log-full 

rm -fr ${STAGING_DIR}/*
unset MAIN_CLASS
unset TMP

