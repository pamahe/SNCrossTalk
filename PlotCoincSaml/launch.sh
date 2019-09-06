#$ -q long
#$ -l sps=1
#$ -V
#$ -e /sps/nemo/scratch/pamahe/SNCrossTalk/PlotCoincSaml/wfD_errors.log
#$ -o /sps/nemo/scratch/pamahe/SNCrossTalk/PlotCoincSaml/wfD_output.log
cd /sps/nemo/scratch/pamahe/SNCrossTalk/PlotCoincSaml
root -l -b -q plotCoincSaml.cxx
