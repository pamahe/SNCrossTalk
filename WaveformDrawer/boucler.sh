#!/bin/bash

TRIGGER_LIST="HT"
RUN_LIST="104"

for run in ${RUN_LIST}
do
    for trig in ${TRIGGER_LIST}
    do
	    
	    INPUT_FILE="/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/full/run-${run}_${trig}s_full.root"
	    OUTPUT_FILE="/sps/nemo/scratch/pamahe/SNCrossTalk/WaveformDrawer/results/wf_plot_run-${run}_${trig}_CS.root"
	  
	    INPUT_OPTION="--input-file ${INPUT_FILE}"
	    OUTPUT_OPTION="--output-file ${OUTPUT_FILE}"
	    INPUT_LIST="--input-list /sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/hit_list/list_CS_run-${run}_${trig}.lis"
	    
	    SCRIPT_NAME="${trig}_${run}_CS_plot"

	    qsub -P P_nemo -N ${SCRIPT_NAME} launch.sh $INPUT_OPTION $OUTPUT_OPTION $INPUT_LIST
    done
    
done
