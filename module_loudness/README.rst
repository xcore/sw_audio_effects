Non-linear-gain Function Library
================================

:scope: Early Development
:description: A Non-linear-gain library using 2 multplies/sample/channel, capable of processing around 12 channels of 48 KHz audio in parallel.
:keywords: audio, dsp
:boards: XA-SK-AUDIO

Features
--------

   * Offers user-configurable increased gain, without clipping
   * From 12 channels at 48 KHz to 2 channel at 192 KHz

Description
-----------

This algorithm uses a piece-wise parabolic curve, to limit the number of multiplies required per sample.
Currently 5 parabola's are used, as follows:-
   #. Unity-gain section (Straight line) for low-level signals (e.g. noise)
   #. Transition to maximum gradient (user-configurable). 
   #. Transition from max. gradient to 'max-gain' section.
   #. 'max-gain' section, (Straight line) with minimum gradient (reciprocal of max-gradient)
   #. Transition from min. gradient back to 'unity-gain' to finish.
