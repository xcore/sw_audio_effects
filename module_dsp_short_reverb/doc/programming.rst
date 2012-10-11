The Programming Guide
=====================

Key Files
---------
reverb.c: Contains 'C' function library
reverb.h: Header file for 'C' function library

Usage
-----
There are 2 functions in the 'C' library, designed to be called from an *.xc file.
config_reverb() Called to do initial configuration of reverb parameters (e.g. room size). 
	WARNING: This must be called before ...
use_reverb() Called to apply reverb to a a set of audio-sample stereo pairs, designed to be called from an *.xc file.

Note After initialisation, config_reverb() can be called again as required to re-configure the reverb parameters.

The default reverb parameters are set in the dsp_control() function in file dsp_control.xc.
These can be overwritten before calling config_reverb.
Currently the room_size is scaled to suit the number of channels required.
e.g def_param_s.room_size = (80 / NUM_REVERB_CHANS);

Note The maximum room-size is limited by the amount of memory used for the delay-line.

Demo Applications
=================

app_slicekit_reverb Demo
--------------------------

This application uses module_dsp_short_reverb to create a reverb-like effect on an audio-stream, 
passing through the SliceKit Core board.
The audio is connected to the board using the following Analogue ports:
Input 0/1 (Microphone mini-jack)
Output 0/1 (Headphone mini-jack)

The following parameters are configurable:
	room size (in metres)
	low-pass frequency cut-off
	sample frequency
	'bring-up' gain

	Level of Dry(Input)
	Level of Reverb Effect
	Amount of attenuation/decay of reverb tail
	Amount of Cross-talk (mixing between left/right channels)


Makefile
........
The Makefile is found in the top level directory of the application (e.g. app_slicekit_reverb)

The application is for the Slicekit Core Board so the TARGET variable needs to be set in the Makefile: e.g
TARGET = xp-skc-l2-single-xscope.xn

The number of channels supported is currently 2 or 4, this is set in the Makefile: e.g.
NUM_DELAY_CHANS = 2

Running the application with the Command Line Tools
...................................................
In the top level directory of the application (e.g. app_slicekit_reverb) type
xmake clean
xmake all

Connect the board to your PC using an XTAG-2

To start the demo type
xrun --io bin/app_slicekit_reverb.xe to start the demo

When listening to the output audio at 48kHz sample rate, you should hear the following ...
About 8 seconds of the reverb effect, followed by
About 8 seconds of the dry (un-effected) signal.

This cycle will repeat indefinitely.

Trouble-shooting
................
During initialisation, and when the system is reconfigured, 
there may be audible clicks in the audio. This is expected.

Most recorded music already has reverb added. 
So the effects of the reverb application may not always be apparent.
Best to choose some dialogue or sparse percusive sound.

app_slicekit_reverb Demo
-------------------------

See app_slicekit_reverb/doc/programming.rst
