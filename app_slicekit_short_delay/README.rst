Short-Delay sliceKIT Audio Demo
===============================

:scope: Example
:description: Delays an a number of audio channels paasing through the sliceKIT board creating an echo effect
:keywords: delay, echo, audio, dsp, sliceKIT
:boards: XA-SK-AUDIO

Toggles between Dry and Effect signals. 
A 4-tap delay-line is used to create 4 echos which are mixed with the original signal.

The Audio_IO uses 1 logical core (aka thread).
The DSP Delay-line function uses 1 logical core.
