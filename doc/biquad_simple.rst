BiQuad Audio Effect 
'''''''''''''''''''

This module is a audio filter called from within a single logical core (aka thread).

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

.. literalinclude:: module_dsp_biquad/src/biquad_simple.c
  :start-after: //::declaration
  :end-before: //::

