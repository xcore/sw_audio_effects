Resource Requirements
=====================

Available Memory
----------------
After loading application remaining memory(Bytes) is about:
Core0: 0xa900 
Core1: 0xd700

Computation
-----------
The BiQuad algorithm uses 6 multiples/sample on each iteration.
It is estimated that 24 multiples are possible at 48kHz sample rate.
This would allow 4 channels of audio to be processed simultaneously.

