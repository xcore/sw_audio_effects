
# This variable should contain a space separated list of all
# the directories containing buildable applications (usually
# prefixed with the app_ prefix)
#
# If the variable is set to "all" then all directories that start with app_
# are built.
BUILD_SUBDIRS = app_slicekit_biquad app_slicekit_delay app_slicekit_loudness app_slicekit_reverb 

XMOS_MAKE_PATH ?= ..
include $(XMOS_MAKE_PATH)/xcommon/module_xcommon/build/Makefile.toplevel

# This variable should contain a space separated list of all
# the directories containing buildable applications (usually
# prefixed with the app_ prefix)
#
# If the variable is set to "all" then all directories that start with app_
# are built.
BUILD_SUBDIRS = app_slicekit_biquad app_slicekit_delay app_slicekit_loudness app_slicekit_reverb 

XMOS_MAKE_PATH ?= ..
include $(XMOS_MAKE_PATH)/xcommon/module_xcommon/build/Makefile.toplevel
The Programming Guide
=====================

Key Files
---------
sdram_reverb.c: Contains 'C' function library
sdram_reverb.h: Header file for 'C' function library

Usage
-----
There are 2 functions in the 'C' library, designed to be called from an *.xc file.
config_sdram_reverb() Called to do initial configuration of reverb parameters (e.g. room size). 
	WARNING: This must be called before ...
use_sdram_reverb() Called to apply reverb to a a set of audio-sample stereo pairs, designed to be called from an *.xc file.

Note After initialisation, config_sdram_reverb() can be called again as required to re-configure the reverb parameters.

The default reverb parameters are set in the dsp_sdram_reverb() function in file dsp_sdram_reverb.xc.
These can be overwritten before calling config_sdram_reverb.
Currently the room_size is set to 200 metres (Cathedral)
e.g def_param_s.room_size = 200;

Demo Applications
=================

app_slicekit_sdram_reverb Demo
------------------------------

This application uses module_dsp_long_reverb to create a long reverb-like effect on an audio-stream, 
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
The Makefile is found in the top level directory of the application (e.g. app_slicekit_sdram_reverb)

The application is for the Slicekit Core Board so the TARGET variable needs to be set in the Makefile: e.g
TARGET = xp-skc-l2-single-xscope.xn (or SLICEKIT-L2.xn)

The number of channels supported in currently 2 or 4, this is set in app_conf.h e.g.
#define NUM_APP_CHANS 4

Running the application with the Command Line Tools
...................................................
In the top level directory of the application (e.g. app_slicekit_sdram_reverb) type
xmake clean
xmake all

Connect the board to your PC using an XTAG-2

To start the demo type
xrun --io bin/app_slicekit_sdram_reverb.xe to start the demo

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

app_slicekit_sdram_reverb Demo
-------------------------

See app_slicekit_sdram_reverb/doc/programming.rst
