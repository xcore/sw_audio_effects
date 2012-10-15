Resource Requirements
=====================

Available Memory
----------------
This application stores the delayed samples in the SDRAM Memory-Slice,
it memory useage is  variable depending on the size of the maximum delay requested
(used to produce the echos), and the number of channels being processed. 
In module_dsp_short_delay/src/delay_line.h. 
A #define MEM_SAMPS is used to indicate the amount of memory available for the delay-line.
Currently this is set at 2 Mega-samples, which is the size for a 8 MByte SDRAM Memory-slice

Used memory(Bytes) is about:
   * codememory: 12K
   * datamemory: 10K

Computation
-----------
module_dsp_biquad, and module_dsp_loudness, run in their own logical cores.
Therefore, refer to the documention in those modules for computation details on those functions.

The Reverb-Control code is timing critical, 
The channel I/O had to be hand-crafted to get the application working on 4 channels.
The Memory-Slice board has to be on the same Tile that is running dsp_sdram_reverb.xc

The Reverb echos are produced using a 4 tap delay-line.
The reverb has configurable mixing parameters.
There are currently 4, each one requires a multiply. That is 4 multiples/sample/channel.
For a 48 kHz sample rate, on a 500 MHz processor, the composite Reverb_control/Delay-line requires
1250 cycles/sample per channel.
