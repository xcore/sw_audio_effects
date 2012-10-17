Overview
========

This function library can be used to create long multiple echos by feeding the input audio stream into a delay-line. 
The delay-line returns a set of delayed outputs (taps), with the number of taps being configurable.
This library makes use of the xSOFTip SDRAM controller component, and for an 8 MByte SDRAM, 
can delay a stereo channel by up to 20 seconds (at 48 kHz sample rate)

There is a delay-line that uses only the 64 KByte xCore memory, but only short delays are possible.

See the ``Short Delay Function Library``.
