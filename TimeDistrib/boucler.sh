#!/bin/bash

RUN_LIST="100 101 102 104 106 107 108 109 110 111 112 113 114 115 116 119 120 121 122 127 128 129 130 131 132 133 134 135"
INPUT_PATH="/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/basic"
OUTPUT_PATH="/sps/nemo/scratch/pamahe/SNCrossTalk/TimeDistrib/results"

for run in ${RUN_LIST}
do 
    ./_install.d/TimeDistrib --input-file ${INPUT_PATH}/run-${run}_HTs_basic.root --output-file ${OUTPUT_PATH}/HTD_run-${run}_HT.root
done
