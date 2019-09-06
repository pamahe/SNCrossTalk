#!/bin/bash 

RUN_LIST="138_s0 138_s1 139_s0 139_s1" #List of run to process (run 139 has the two walls)

TRIGGER_LIST="HT" #Trigger condition      HT -> High-Threshold hits only, LT -> Low-Threshold hits only, noT -> no condition

PROCESS_LIST="processBasic processFull"  #Two kinds of processing (see crd2root README)

OUTPUT_PATH="/sps/nemo/scratch/pamahe/SNCrossTalk/crd2root/results" #folder for final root files 

for run in ${RUN_LIST} # for each run
do
    for trig in ${TRIGGER_LIST} # for each trigger option
    do
	for process in ${PROCESS_LIST} # fo full and basic process
	do
	    
	    if [[ ${process} == "processBasic" ]]
	    then
		PROCESS_OPTION="" 
		RESULTS="basic" #Differentiate processBasic and processFull results files
		NAME="B"        #Differentiate jobs in GridEngine
	    fi

	    if [[ ${process} == "processFull" ]]
	    then
		PROCESS_OPTION="--process-full" #Add branches to resulting tree (see crd2root README)
		RESULTS="full"
		NAME="F"
	    fi
   
	    INPUT_LIST="/sps/nemo/scratch/pamahe/SNCrossTalk/CRD_list/CRD_files_lis_run-${run}.lis" # use the CRD files list for a run
	                                                                                            # CCIN2P3 path is somthing like : /sps/nemo/snemo_data/raw_data/RHD/run104/CRD_files_list
	    OUTPUT_FILE="${OUTPUT_PATH}/${RESULTS}/run-${run}_${trig}_${RESULTS}.root" # name of the output file (including path)

	    # translate previous paths into launch option for crd2root
	    INPUT_OPTION="--input-list ${INPUT_LIST}"
	    OUTPUT_OPTION="--output-file ${OUTPUT_FILE}"
        
	    if [[ ${trig} == "HT" ]] #Trigger option of crd2root (HT, noT) LT is not supported here
	    then
		TRIGGER_OPTION="--high-threshold"
	    fi
	    	    
	    if [[ ${trig} == "noT" ]]
	    then
		TRIGGER_OPTION=""
	    fi

	    PROCESS_NAME="${NAME}${trig}_${run}" #Name of Gridengine job

	    #submit launch.sh (containing crd2root executable) to GridEngine for each run, trigger condition etc.
	    qsub -P P_nemo -N ${PROCESS_NAME} launch.sh ${INPUT_OPTION} ${OUTPUT_OPTION} ${TRIGGER_OPTION} ${PROCESS_OPTION}
	done
    done
done
