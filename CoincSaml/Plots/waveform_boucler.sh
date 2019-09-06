#$ -q long
#$ -l sps=1
#$ -N WD
#$ -V
#$ -e /sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/WD.log
#$ -o /sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml/WD.log
cd /sps/nemo/scratch/pamahe/SNCrossTalk/CoincSaml
time root -l -b -q waveformDrawer.cxx
