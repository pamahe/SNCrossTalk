==========================
The ``cut_charge`` program
==========================

ROOT macro used to reproduce the bug observed using RHD files.

At the beginning of the internship, we used data coming from RHD files and observed a cut in
the charge distribution. It was due to the charge being stored in the wrong type (Int16_t instead
of Int32_t).

We switched to CRD files to extract data to avoid the problem.

This program is used to reproduce the observed overflow.

Output 3 histograms :

-> negCD : bugged charge distribution, only in the negative range
-> allCD : bugged charge distribution, including the positive range
-> trueCD : real charge distribution



