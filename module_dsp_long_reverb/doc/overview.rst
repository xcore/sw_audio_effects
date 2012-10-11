Overview
========

This is a function library for creating a long reverb-like effect. The input audio stream is fed into a complex DSP function that uses multiple echos, 
equalisation and gain-shaping, to create an example reverberation effect.

The dsp_function returns an output stream that is a mix of the input and effect signal. The proportion of Dry/Effect signal is configurable.
