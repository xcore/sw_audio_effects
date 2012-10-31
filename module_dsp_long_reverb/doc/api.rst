.. _sec_module_dsp_long_reverb_api:

API
===

.. _sec_module_dsp_long_reverb_conf_defines:

Configuration Defines
---------------------
.. doxygendefine:: NUM_REVERB_TAPS 
.. doxygendefine:: DEF_ROOM_SIZE 
.. doxygendefine:: MIX_BITS 
.. doxygendefine:: DEF_DRY_LVL 
.. doxygendefine:: DEF_FX_LVL 
.. doxygendefine:: DEF_FB_LVL 
.. doxygendefine:: DEF_CROSS_MIX 

Functions
---------

Data Types
++++++++++
.. doxygentypedef:: S32_T

Data Structures
+++++++++++++++
.. doxygenstruct:: MIX_PARAM_TAG
.. doxygenstruct:: REVERB_PARAM_TAG

Configuration Functions
+++++++++++++++++++++++

.. doxygenfunction:: config_sdram_reverb

DSP Processing Functions
++++++++++++++++++++++++

.. doxygenfunction:: use_sdram_reverb
