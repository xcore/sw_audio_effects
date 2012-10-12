Long-Delay Application For SliceKit Board
=========================================

:scope: Example
:description: Delays a No. of audio channels passing through the SliceKit board creating an long echo effect
:keywords: delay, echo, audio, dsp, slicekit, memory-slice, sdram
:boards: XA-SK-AUDIO

Toggles between Dry and Effect signals. 
A 4-tap delay-line is used to create 4 echos which are mixed with the original signal.
The audio is written to the SDRAM Memory-Slice to create upto 20 seconds of delay.

The Audio_IO uses 1 logical core (aka thread).
The SDRAM_IO uses 1 logical core.
The DSP Delay-line function uses 1 logical core.
