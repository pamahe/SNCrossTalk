==============================
The ``WaveformDrawer`` program
==============================

Compiled program (CMakeLits.txt is present) used to draw waveforms.
Usage ::

  $ ./WaveformDrawer --input-file your_ROOT_tree.root \\
  $                  --output-file your_output_file.root \\
  $                  --input-list  input_list.lis \\
  $                  --min-tdc 10 \\
  $                  --max-tdc 100

If no input list is given, all events waveforms will be plotted.
If an input list is given, it should be with *one* hit number per line

**min-tdc** and **max-tdc** are used if you want to plot a certain time interval, with no specified option, the entire run waveforms will be plotted.
