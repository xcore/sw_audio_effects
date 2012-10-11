Resource Requirements
=====================

Available Memory
----------------
This application uses variable amounts of memory depending on the size of the delay-line,
and the number of channels being processed. 
A #define MEM_SAMPS is used to indicate the amount of memory available for the delay-line.
Currently this is set at 12000 samples, which uses almost all the available memory on Tile_0.

Used memory(Bytes) is about:

   * codememory:  5K
   * datamemory: 49K

Computation
-----------
The Delay-line uses NO multiples. 
So far a maximum of 4 delay taps have been used successfully, each on 4 channels.
By inspecting timings the processing time for one tap is about 80 cycles.
For a 48 kHz sample rate and 500 MHz processor, this would allow about 100 taps/sample/channel.

