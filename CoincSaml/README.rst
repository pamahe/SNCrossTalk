=========================
The ``CoincSaml`` program
=========================

Works in the same ways as ``Coincidence.cxx program`` except that for each reference OM we look for coincidences in the SAMLONG neighbour only and fill the **measured** histogram.

The coincidence window is fixed to 400 ns (duration of an event in memory) and is divided into two part : **ctWindow_low** and **ctWindow_high**. The last one is equal to the **Postrig** value set up for the run. 

After that the program will used the Trigger Rate to compute the number of expected fortuitous coincidences and the statistical error on that value and fill the **expected** histogram.


Again ``boucler.sh`` and ``launch.sh`` are used to send a list of runs to the GridEngine of CCIN2P3.

The **hit_list** that is outputed works in the same way than the one from Coincidence.cxx but is a little different. Only hit number are written so they are easy to retrieve in order to plot the waveform. Example : ::


    | ##### 12.5 ######   // The reference cube is in column 12, row 5
    | 2355                // reference hit number
    | 3466                // coincidence hit number
    | 3556                // another coincidence hit
    |                     // a blank line indicates that we crossed the coincidence window limit
    | 4367                // new reference hit
    | 3533                // etc.


Plots/plot.cxx
--------------

ROOT macro used to represent the previous results with the statistical errors.
Useless due to very high errors.
