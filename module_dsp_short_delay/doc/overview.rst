Overview
========

This function library can be used to create multiple echos.
The input audio stream is fed into a delay-line.
The delay-line returns a set of delayed outputs (taps).
The number of taps is configurable.
The delay-line uses only local memory, so only short delays upto 12,000 samples are possible.
For longer delays see module_dsp_long_delay/doc/overview.rst.
