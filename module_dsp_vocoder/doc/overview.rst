Overview TODO
========

This application creates a 'vocoder-like' effect.
There a 2 mono input audio streams. One stream is used to modulate the other carrier stream
The dsp_function returns a  stereoo Output stream.
The following characteristics of the vocoder are currently configurable:

   * Number of slave cores (each running a vocoder)
   * Number of frequency bands per slave core

The following signal-mix levels are currently configurable:

   * Modulator (input) signal
   * Carrier (input) signal
   * Stereo output signal
