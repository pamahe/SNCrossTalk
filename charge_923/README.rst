==========================
The ``charge_923`` program
==========================

ROOT macro developed to check the behavior of OM 9.2 and 9.3 of the French wall,
both those OMs registered way more coincidences on the same SAMLONG (see CoincSaml program)
than the others on the French wall.

This program ouput 4 TH1D :

-> charge distribution of OM 9.2 and 9.3
-> charge distribution of all OMs except 9.2 and 9.3

-> amplitude distribution of OM 9.2 and 9.3
-> amplitude distribution of all OMs except 9.2 and 9.3

in order to compare them.

|

Gave us the following reult (for charge distribution) :

.. image:: images/CD.png
	   :width: 600

Showing unexpected behavior.
