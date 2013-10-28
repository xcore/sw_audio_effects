Reverb/BiQuad sliceKIT Audio Demo
=================================

:scope: Example
:description: The Long-Reverb or BiQuad effects are selected using the buttons on the General Purpose I/O (GPIO) slice
:keywords: reverb, delay, echo, audio, dsp, sliceKIT
:boards: XA-SK-AUDIO

Applies reverb to audio stream.
The Audio_IO uses 1 logical core.
The SDRAM_IO uses 1 logical core.
The GP_IO uses 1 logical core.
This DSP function uses 3 logical cores,

* one for Equalization, 
* one for Gain-control (Loudness), 
* the remaining logical core handles control and delay functions.

The audio is written to the SDRAM Memory-Slice to create up to 20 seconds of delay.
