===========================
The ``Coincidence`` program
===========================

``boucler.sh`` and ``launch.sh`` are used to send jobs to GridEngine for different runs, time window options etc.

Coincidence.cxx :
-----------------

Need to be computed with Bayeux and SNFEE libraries
CMakeLists.txt is present

This program is launched as :

.. code:: bash

	  $ ./Coincidence --input-file your_sorted_root_tree.root \\
	                  --output-file your_output_root_file.root \\
			  --ct-window 100 \\ (the coincidence window in ns)
			  --output-list hit_list.lis \\ (your output list ofstream, see below)


Given a sorted ROOT tree of all events in a run (crd2root -> SortTree -> Coincidence) :

For every OM, the program will loop over the entire tree. Once an event in the current OM is found it stores the charge of the event in the **charge_q1** variable and will look the following tree entries.

If a following entry is not in the same OM and is in the coincidence window, the corresponding bin in the **coincHits[]** array is incremented and then there is 3 options :

    -> *Same SAMLONG* :
    The charge is stored in the **charge_q2** variable, the ratio (coincidence charge)/(reference charge) is stored in the **division** variable, and if not NaN, filled into the **histo_charge_samlong** TH1D. 
    
    -> *3 other closest neighbours* (left && right && (above || down)) :
    Same as before but with the **charge_q3** variable and the ratio is filled into the **histo_charge_neighbour** TH1D

    -> *Other OMs* :
    Same as before but with the **charge_q4** variable and the ratio is filled into the **histo_charge_other** TH1D

When an event is outside the coincidence window, the program goes to the next OM.

The commented part was used to look in the backward coincidence window but was removed to avoid double counting.

|

Then the coincidence probability is computed per OM (this time identified with a number from 0 to 260 instead of column and row) and stored in the **prob** variable.

The *number of coincidence hits* and *probability of coincidence* histograms (respectively **output_histo_hits** and **output_histo_prob**) are finally filled.

hit_list ofstream
-----------------

This ofstream is used to keep track of events considered as coincidences and later, extract the waveform (see waveformDrawer.cxx).
It outputs a text file per run in the following structure :

| ===== column of reference OM . row of reference OM ===== +/- time window ns =====
| reference hit number, column, row, time of arrival in ns and raw charge 
| every coincidence hit number, columne, row, time and raw charge (one per line)
|
| next reference hit number, column, row, time of arrival in ns and raw charge
| every coincidence hit number, columne, row, time and raw charge (one per line)
|
| ===== next reference OM =====

etc.


plot.cxx
--------

Is used to plot 4 coincidence hits TH2D obtained with Coincidence.cxx like the following :

.. image:: images/plot4.png
	   :width: 600
charge_ratio.cxx
----------------

This program is a ROOT macro used to rewrite the charge ratio histograms obtained with Coincidence.cxx, clear NaN values and normalize them
