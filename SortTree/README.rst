=============================================
The ``SortTree`` and ``SortBasicTree`` Macros
=============================================

.. contents::

--------


Usage
=====

``SortTree.cxx`` and ``SortBasicTree.cxx`` are two ROOT macros that can be launch with :

.. code:: bash

	  $ root -l -b -q SortTree.cxx

``SortTree`` require ROOT files obtained with the --process-full option (seeD ``crd2root`` README)

	 > Each event in the tree is stored in an instance of the "Event" structure

	 > Input tree is stored as a vector of "Event" that is sorted with std::sort

	 > The sorted vector is used to fill the output tree


SortBasicTree.cpp -> Used for tree obtained without ``crd2root`` --processFull (only position and time)


SortTree.cpp -> Used for tree obtained with ``crd2root`` --processFull (all info including waveform)
