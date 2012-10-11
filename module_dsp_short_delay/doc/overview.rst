Overview
========

This function library can be used to create multiple echos by feeding the input audio stream into a delay-line. The delay-line returns a set of delayed outputs (taps), with the number of taps being configurable.

The delay-line uses only local memory, so only short delays upto 12,000 samples are possible. For longer delays see the ``Audio Long Delay Function Library``.
