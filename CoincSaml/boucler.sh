#!/bin/bash

TRIGGER_LIST="HT"   #HT
RUN_LIST="106"    # 100 04 107 108 109 110 111 112 113 114 115 116 119 120 121 122 123 127 128 129 130 131 132 133 134

for run in ${RUN_LIST}
do
    for trig in ${TRIGGER_LIST}
    do
	INPUT_FILE="/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/full/run-${run}_${trig}s_full.root"
	OUTPUT_FILE="/sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/results/CS_run-${run}_${trig}.root"
	
	INPUT_OPTION="--input-file ${INPUT_FILE}"
	OUTPUT_OPTION="--output-file ${OUTPUT_FILE}"
	OUTPUT_LIST="--output-list /sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/hit_list/list_CS_run-${run}_${trig}.lis"
	
	SCRIPT_NAME="CS_${trig}${run}"
	
	qsub -P P_nemo -N ${SCRIPT_NAME} launch.sh $INPUT_OPTION $OUTPUT_OPTION	$OUTPUT_LIST    
    done
done
