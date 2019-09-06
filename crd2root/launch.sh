#$ -q long 
#$ -l sps=1
#$ -V
#$ -e /sps/nemo/scratch/pamahe/SNCrossTalk/crd2root/crd2root.log
#$ -o /sps/nemo/scratch/pamahe/SNCrossTalk/crd2root/crd2root.log

#Call the executable for each condition in boucler.sh

cd /sps/nemo/scratch/pamahe/SNCrossTalk/crd2root
./_install.d/crd2root $1 $2 $3 $4 $5 $6
exit 0

