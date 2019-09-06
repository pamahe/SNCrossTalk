#$ -q long
#$ -l sps=1
#$ -V
#$ -e /sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/CoincSaml_errors.log
#$ -o /sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/CoincSaml_output.log
cd /sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml
./_install.d/CoincSaml $1 $2 $3 $4 $5 $6
