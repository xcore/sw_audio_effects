Short-Reverb sliceKIT Audio Demo
================================

:scope: Example
:description: A reverb-like effect is created by using the building blocks of delay, equalisation and 'bring-up' gain.
:keywords: reverb, delay, echo, audio, dsp, sliceKIT
:boards: XA-SK-AUDIO

Features
--------

   * Automatically Toggles between "Dry" and "Effect" signals. 
   * Applies a reverberation effect to anaudio stream.
   * Audio_IO uses 1 logical core.
   * The DSP function uses 3 logical cores as show below

DSP Logical Core Usage
----------------------

   * one for Equalisation
   * one for Gain-control (Loudness)
   * one for control and delay functions
