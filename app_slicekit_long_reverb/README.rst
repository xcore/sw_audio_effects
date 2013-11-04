Long-Reverb sliceKIT Audio Demo
===============================

:scope: Example
:description: A 'reverb-like' effect with a long decay time is created by using the building blocks of delay, equalization and non-linear gain.
:keywords: reverb, delay, echo, audio, dsp, sliceKIT
:boards: XA-SK-AUDIO

Toggles between Dry and Effect signals. 

Applies reverb to audio stream.
The Audio_IO uses 1 logical core (aka thread).
The SDRAM_IO uses 1 logical core.
This DSP functions use 3 logical cores,

* one for Equalization, 
* one for Gain-control (Loudness), 
* the remaining logical core handles control and delay functions.

The audio is written to the SDRAM Memory-Slice to create up to 20 seconds of delay.
