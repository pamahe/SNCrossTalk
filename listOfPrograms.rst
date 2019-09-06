==============================================
List of programs in the ``SNCrossTalk`` folder
==============================================

1. crd2root
   Extract events data from CRD files to ROOT tree

|

2. chargeComputation
   recompute manually the charge like what is done to get RHD files and compare it to the raw_charge data

|

3. SortTree
   sort the ROOT tree obtained with crd2root, basic with only time and position, full to get all data and waveform

|

4. Coincidence
   compute the number of coincidences and probability per OM for a given coincidence window

|

5. 2Dto1D
   transform the probability of coincidences TH2D in a TH1D with the closest OMs to the reference one

|

6. CoincSaml
   compute the number of coincidences in the SAMLONG neigbour per OM as well as the expected number of fortuitous coincidences and statistical errors

|

7. PlotCoincSaml
   plot the waveforms of the reference hit and the corresponding coincidence hit side by side

| 

8. TriggerRate
   Compute the trigger rate (registered and real) per OM, SAMLONG and FPGA

|  

9. cut_charge
   reproduce the bug that was cutting the charge distribution in RHD files

|
   
10. charge_923
    compare the charge and amplitude distribution of OMs 9.2 and 9.3 to the rest of the Freanch wall to understand the very high number of coincidences registered on those 2 OMs

    |
    
11. runDuration
    compute the runDuration for every run given and store them in a single text file

|
    
12. TimeDistrib
    plot the hit time distribution 

|
    
13. FFT
    perform the fast fourier transform of a part of the hit time distribution

|
    
14. C0CM
    plot the ratio : (trigger rate in the first 50ms)/(trigger rate  in the rest of the run) per OM, SAMLONG and FPGA

|
    
15. WaveformDrawer
    allow to plot events waveform, all the run, a specified time window or events list
