Loudness Audio Effect 
'''''''''''''''''''''

This module is a audio delay called from within a single logical core (aka thread).

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

.. literalinclude:: module_dsp_loudness/src/no_linear_gain.c
  :start-after: //::declaration
  :end-before: //::

