Short-Reverb Application For SliceKit Board
===========================================

:scope: Example
:description: A reverb-like effect is created by using the building blocks of delay, equalisation and 'bring-up' gain.
:keywords: reverb, delay, echo, audio, dsp, slicekit
:boards: XA-SK-AUDIO

Toggles between Dry and Effect signals. 

Applies reverb to audio stream.
The Audio_IO uses 1 logical core (aka thread).
This DSP function uses 3 logical cores,

   * one for Equalisation, 
   * one for Gain-control (Loudness), 
   * the remaining logical core handles control and delay functions.
