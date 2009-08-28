#!/bin/bash

##SET DIR
STAGING_DIR=/tmp/dsfinder
BENCHMARKS_DIR=/xtra/benchmark_sources ##
##

/bin/mkdir -p $STAGING_DIR
FILE=$1
export PACKAGE=$2
export PROJECT_ROOT=$STAGING_DIR
export CLASSES_DIR=$FILE

##USE LOCAL DSFINDER
cd ${BENCHMARKS_DIR}/scripts > /dev/null
. ${BENCHMARKS_DIR}/scripts/benchmark.sh staging ds-finder
cd - > /dev/null

/bin/cp ${BENCHMARKS_DIR}/Output/logs/staging.ds-finder.log staging.ds-finder.log
/bin/cp ${BENCHMARKS_DIR}/Output/logs/staging.ds-finder.log-full staging.ds-finder.log-full

/bin/rm -fr ${STAGING_DIR}/*
unset MAIN_CLASS
unset TMP

