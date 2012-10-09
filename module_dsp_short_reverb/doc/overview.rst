Overview
========

This application creates a reverb-like effect.
The input audio stream is fed into a complex DSP function that uses multiple echos, 
equalisation and gain-shaping, to create a 'reverb-like' effect.
The dsp_function returns an output stream that is a mix of the input and effect signal.
The proportion of Dry/Effect signal is configurable.
