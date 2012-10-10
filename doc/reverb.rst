Reverb Audio Effect 
'''''''''''''''''''

This module is a audio reverb using 3 threads for DSP, and 1 for Audio I/O.

API
===

Symbolic constants
------------------

.. doxygendefine:: 

Structures
----------

.. doxygenstruct:: 

Functions
---------

.. doxygenfunction:: 

Example
=======

This function is designed to be called from a *.xc file

.. literalinclude:: module_dsp_short_reverb/src/reverb.c
  :start-after: //::declaration
  :end-before: //::

