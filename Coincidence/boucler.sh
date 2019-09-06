#!/bin/bash

TRIGGER_LIST="HT"   #HT
#RUN_LIST="100 101 102 104 106 107 108 109 110 111 112 113 114 115 116 119 120 121 122 127 128 129 130 131 132 133 134 135"      # 100 04 107 108 109 110 111 112 113 114 115 116 119 120 121 122 123 127 128 129 130 131 132 133 134
#CT_WINDOW_LIST="1 10 100 1e3 1e4 1e5 1e6"

RUN_LIST="139"
CT_WINDOW_LIST="1 10 100 1e3 1e4 1e5 1e6 1e7 1e8"

for run in ${RUN_LIST}
do
    for trig in ${TRIGGER_LIST}
    do
	for ct in ${CT_WINDOW_LIST}
	do
	    INPUT_FILE="/sps/nemo/scratch/pamahe/SNCrossTalk/SortTree/results/full/run-${run}_s0_${trig}s_full.root"
	    #OUTPUT_FILE="/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/results/run_${run}/CT_run-${run}_${trig}_${ct}.root"
	    OUTPUT_FILE="/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/results_NS/run_${run}/Coinc_run-${run}_${ct}.root"

	    INPUT_OPTION="--input-file ${INPUT_FILE}"
	    OUTPUT_OPTION="--output-file ${OUTPUT_FILE}"
	    CT_OPTION="--ct-window ${ct}"

	    OUTPUT_LIST="/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/hit_lists_NS/run_${run}/hit_list_run-${run}_${trig}_${ct}.lis"
	    #OUTPUT_FILE="/sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/list_qsub_test_r100_ct100.lis"
	    LIST_OPTION="--output-list ${OUTPUT_LIST}"

	    SCRIPT_NAME="NS_${trig}${run}_${ct}"
	    
	    qsub -P P_nemo -N ${SCRIPT_NAME} launch.sh $INPUT_OPTION $OUTPUT_OPTION $CT_OPTION ${LIST_OPTION}
	    
	done
    done
done
