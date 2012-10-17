Resource Requirements
=====================

Available Memory
----------------

This application stores the delayed samples in the SDRAM Memory-Slice,
the memory useage is variable depending on the maximum delay requested, and the number of channels being processed. 
A MEM_SAMPS definition is used to indicate the amount of memory available for the delay-line.
Currently this is set at 2 Mega-samples, which is the default size for a 8 MByte SDRAM Memory-slice.

This approximate memory usage for this module is (figures shown in Bytes):

* codememory: 11K
* datamemory: 10K

Computation
-----------

The DSP processing for the delay-line does NOT use multiplication. 
So far a maximum of 4 delay taps have been used successfully, each on 4 channels.
By inspecting timings, the processing time for one tap is about 80 cycles.
For a 48 kHz sample rate and 500 MHz processor, this would allow about 100 taps/sample/channel.
