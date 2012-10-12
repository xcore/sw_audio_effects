Loudness Application For SliceKit Board
=======================================

:scope: Example
:description: Mid-range volume levels are increased without increasing low-level or high-level signals.
:keywords: gain, loudness, audio, dsp, slicekit
:boards: XA-SK-AUDIO

Toggles between Dry and Effect signals. 
A non-linear gain-shaping curve is used to change create an increased output signal level, based on the level of the input signal.

The Audio_IO uses 1 logical core (aka thread).
The DSP Loudness function uses 1 logical core.
