Long-Reverb Function Library
============================

:scope: Early Development
:description: A long 'reverb-like' effect is created using a combination of delays, filtering and gain-shaping, 
 capable of processing up to 4 channels of 48 KHz audio in parallel.
:keywords: audio, dsp, reverb, echo, SDRAM, Memory-slice
:boards: XA-SK-AUDIO, XA-SK-SDRAM

Features
--------

   * Offers user-programmable room-size, high frequency damping, and 'bring-up' gain
   * Offers user-programmable mix-levels of: dry signal, effect signal, left-right cross-mix, reverb attenuation/feedback mix
   * From 4 channels at 48 KHz to 1 channel at 192 KHz
   * Uses double-buffering to 'hide' latency of reading/writing to SDRAM
