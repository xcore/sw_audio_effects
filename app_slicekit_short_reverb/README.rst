Short-Reverb sliceKIT Audio Demo
================================

:scope: Example
:description: A 'reverb-like' effect with a short decay time is created by using the building blocks of delay, equalization and non-linear gain.
:keywords: reverb, delay, echo, audio, dsp, sliceKIT
:boards: XA-SK-AUDIO

Features
--------

   * Automatically Toggles between "Dry" and "Effect" signals. 
   * Applies a reverberation effect to an audio stream.
   * Audio_IO uses 1 logical core.
   * The DSP functions use 3 logical cores as show below

DSP Logical Core Usage
----------------------

   * one for Equalization
   * one for Gain-control (Loudness)
   * one for control and delay functions
