.. _sec_api:

API
===

.. _sec_conf_defines:

Configuration Defines
---------------------
.. doxygendefine:: DEF_FILT_MODE
.. doxygendefine:: DEF_SAMP_FREQ 
.. doxygendefine:: DEF_SIG_FREQ 
.. doxygendefine:: QUAL_BITS 
.. doxygendefine:: DEF_QUAL_FACT 

Functions
---------

Data Types
++++++++++
.. doxygentypedef:: S32_T

Data Structures
+++++++++++++++
.. doxygenstruct:: BIQUAD_PARAM_TAG

Configuration Functions
+++++++++++++++++++++++
.. doxygenfunction:: config_biquad_filter

Receive Functions
+++++++++++++++++

Transmit Functions
++++++++++++++++++

DSP Processing Functions
++++++++++++++++++++++++
.. doxygenfunction:: use_biquad_filter
