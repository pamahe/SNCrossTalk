#$ -q long
#$ -l sps=1
#$ -V
#$ -e /sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/test_coincidence_error_NS.log
#$ -o /sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence/test_coincidence_output_NS.log
cd /sps/nemo/scratch/pamahe/SNCrossTalk/Coincidence
./_install.d/Coincidence $1 $2 $3 $4 $5 $6 $7 $8
