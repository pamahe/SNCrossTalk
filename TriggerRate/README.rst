===========================
The ``TriggerRate`` program
===========================

This program is used to compute the Trigger Rate of each individual OM, each SAMLONG and each FPGA.
Both with the registered Trigger Rate and the True Trigger Rate (including the non registered events counter caused by saturation).


Given a list of run, the program will compute the number of hits per OM as well as the run duration (aka the time of arrival of the last events) and will fill the 3 histograms accordingly.
