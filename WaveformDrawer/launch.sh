#$ -q long 
#$ -l sps=1
#$ -V
#$ -e /sps/nemo/scratch/pamahe/SNCrossTalk/WaveformDrawer/WD_error.log
#$ -o /sps/nemo/scratch/pamahe/SNCrossTalk/WaveformDrawer/WD_output.log
cd /sps/nemo/scratch/pamahe/SNCrossTalk/WaveformDrawer
./_install.d/WaveformDrawer $1 $2 $3 $4 $5 $6
