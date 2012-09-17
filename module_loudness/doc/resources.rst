Resource Requirements
=====================

Available Memory
----------------
After loading application remaining memory(Bytes) is about:
Core0: 0xd700 
Core1: 0xd800

Computation
-----------
The gain-shaping algorithm uses 2 multiples/sample on each iteration.
It is estimated that 24 multiples are possible at 48kHz sample rate.
On a stereo channel, this would allow 6 iterations, or a maximum Gain of 2^6 =64.
NB It is anticipated to upgrade this algorithm to use 2 multiplies/sample whatever the gain requested.

