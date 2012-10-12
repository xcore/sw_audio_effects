BiQuad Application For SliceKit Board
=====================================

:scope: Example
:description: Applies a BiQuad filter to an number of audio channels on the SliceKit board
:keywords: biquad, filter, equalisation, audio, dsp, slicekit
:boards: XA-SK-AUDIO

Toggles between Dry and Effect signals, each time the Effect is called a different filter is used.
The filter cyles through the follwoing pre-defined types: Low Pass, High Pass, Band Pass, Band Stop, All Pass

   * The Audio_IO uses 1 logical core (aka thread).
   * The DSP BiQuad function uses 1 logical core.
