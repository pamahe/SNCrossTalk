=================================
The ``chargeComputation`` program
=================================

ROOT macro used to recompute manually the events charge based on the waveform.
Was used to check the correct behaviour of the SN software

|

Can used sorted or unsorted root tree

|

Output 3 TH1D :

-> DC (Data Charge Distribution)     : histogram of the SN software computed charge
-> CC (Computed Charge Distribution) : histogram of the charge recomputed in this program

-> waveform : The waveform of events with integration limits as vertical lines

|
|

Integration limits are defined as follows :

-> Peak sample is the waveform sample with the highest value (out of 2048 samples)
   we shifted the waveform to positive values for easier integration

-> The lower limit is 64 samples before the peak
-> The higher limit is 992 samples after


|
|

Finally histograms are filled, we set a negative sign for computed charge to account for the shift
to positive values
