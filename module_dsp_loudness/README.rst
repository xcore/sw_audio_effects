Non-linear-gain Function Library
================================

:scope: Early Development
:description: The non-linear-gain function increases perceived loudness, without increasing the maximum volume.
 It uses 2 multplies/sample/channel, and is capable of processing around 12 channels of 48 KHz audio in parallel.
:keywords: audio, dsp, loudness, gain, volume
:boards: XA-SK-AUDIO

Features
--------

   * Offers user-configurable increased gain without clipping
   * From 12 channels at 48 KHz to 2 channel at 192 KHz

Description
-----------

This algorithm uses a piece-wise parabolic curve, to limit the number of multiplies required per sample.
Currently 5 parabola's are used, as follows:

   * Unity-gain section (Straight line) for low-level signals (e.g. noise)
   * Transition to maximum gradient (user-configurable). 
   * Transition from max. gradient to 'max-gain' section.
   * 'max-gain' section, (Straight line) with minimum gradient (reciprocal of max-gradient)
   * Transition from min. gradient back to 'unity-gain' at maximum signal level.

Evaluation
----------

Try the "Loudness sliceKIT Audio Demo" to get started with this module.