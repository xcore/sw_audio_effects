.. _sec_api:

API
===

.. _sec_conf_defines:

Configuration Defines
---------------------
.. doxygendefine:: SWAP_NUM 
.. doxygendefine:: FADE_BITS 

Functions
---------

Data Types
++++++++++
.. doxygentypedef:: S32_T

Data Structures
+++++++++++++++
.. doxygenstruct:: FIX_POINT_TAG

Configuration Functions
+++++++++++++++++++++++

Receive Functions
+++++++++++++++++

Transmit Functions
++++++++++++++++++

DSP Processing Functions
++++++++++++++++++++++++
.. doxygenfunction:: cross_fade_sample
.. doxygenfunction:: fade_in_sample
.. doxygenfunction:: fade_out_sample
.. doxygenfunction:: get_word_address
.. doxygenfunction:: increment_circular_offset
