.. _sec_audio_long_delay_api:

API
===

.. _sec_audio_long_delay_conf_defines:

Configuration Defines
---------------------
.. doxygendefine:: DEF_SAMP_FREQ 
.. doxygendefine:: MEM_SAMPS 
.. doxygendefine:: MAX_TAPS 
.. doxygendefine:: DEF_TAPS 

Functions
---------

Data Structures
+++++++++++++++
.. doxygenstruct:: DELAY_PARAM_TAG

Configuration Functions
+++++++++++++++++++++++
.. doxygenfunction:: config_sdram_delay

DSP Processing Functions
++++++++++++++++++++++++
.. doxygenfunction:: use_sdram_delay
