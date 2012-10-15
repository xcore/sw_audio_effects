DSP Audio Utilities Function Library
====================================

:scope: Early Development
:description: This is a collection of utilities commonly used by the dsp audio modules
:keywords: audio, dsp, util, fade, scale, address, wrap
:boards: XA-SK-AUDIO

Features
--------

   * get_word_address() converts word reference to address
   * increment_circular_offset() increments a circular buffer offset, i.e. wraps if at end of buffer
   * cross_fade_sample() returns cross-faded sample
   * fade_in_sample() returns faded-in sample
   * fade_out_sample() returns faded-out sample
