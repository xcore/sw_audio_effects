Resource Requirements
=====================

Available Memory
----------------
This application uses variable amounts of memory depending on the size of the delay-line,
(used to produce the echos) and the number of channels being processed. 
In module_delay/src/delay_line.h, a #define MEM_SAMPS is used
to indicate the amount of memory available for the delay-line.
Currently this is set at 13000 samples, which uses almost all the available memory on Core_0
After loading application remaining memory(Bytes) is about:
Core0: 0x0750 
Core1: 0x8e80

Computation
-----------
module_biquad, and module_loudness, run in their own threads.
Therefore, refer to the documention in those modules for computation details on those functions.

The Reverb-Control thread is timing critical, 
The channel I/O had to be hand-crafted to get the application working on 4 channels.

The Reverb echos are produced using a 4 tap delay-line.
The reverb has configurable mixing parametere. 
There are currently 4, each one requires a multiply. That is 4 multiples/sample/channel.
For a 48 kHz sample rate, on a 500 MHz processor, the composite Reverb_control/Delay-line requires
1250 cycles per channel.
