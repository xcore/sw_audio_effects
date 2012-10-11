
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

Programming Guide
=================

Key Files
---------

   * ``delay_line.c``: Contains 'C' function library
   * ``delay_line.h``: Header file for 'C' function library

Usage
-----

There are 2 functions in the 'C' library, designed to be called from an *.xc file.
   * ``config_delay()`` Called to do initial configuration of delay parameters (e.g. delay offsets). WARNING: This must be called before ...
   * ``use_delay()`` Called to apply set of delays to one audio-sample, designed to be called from an *.xc file.

Note After initialisation, config_delay() can be called again as required to re-configure the delay offsets.

Delays are set in microseconds in the us_delays field of the DELAY_PARAM_S structure.

Demo Applications
=================

Delay-line Application For SliceKit Board (``app_slicekit_delay``)
-----------------------------------------

This application uses module_dsp_short_delay to create a set of delays (or echos) for an audio-stream, 
passing through the SliceKit Core board.
The audio is connected to the board using the following Analogue ports:
   * Input 0/1 (Microphone mini-jack)
   * Output 0/1 (Headphone mini-jack)

An itermediate signal is created mixing the input stream with the output of the delay-line,
which is set-up to produce 4 different echos. 
This intermedate signal is used for the output, 
but also fed back into the delay-line, to create a potentially infinite set of echos.
In fact the mixing is arranged so that the echos decay exponentially in amplitude.

Makefile
........

The Makefile is found in the top level directory of the application (e.g. ``app_slicekit_delay``)

The application is for the Slicekit Core Board so the TARGET variable needs to be set in the Makefile: e.g

TARGET = xp-skc-l2-single-xscope.xn

The number of channels supported in currently 2 or 4, this is set in auto_conf.h: e.g.

NUM_DELAY_CHANS = 2

Running the application with the Command Line Tools
...................................................

In the top level directory of the application (e.g. ``app_slicekit_delay``) type
   * xmake clean
   * xmake all

Connect the board to your PC using an XTAG-2

To start the demo type
   * xrun --io bin/app_slicekit_delay.xe to start the demo

When listening to the output audio at 48kHz sample rate, you should hear the following ...
   * About 8 seconds of the effect (i.e. multiple echos decaying in amplitude), followed by
   * About 8 seconds of the dry (un-effected) signal.

This cycle will repeat indefinitely.

Trouble-shooting
................

During initialisation, and when the system is reconfigured, 
there may be audible clicks in the audio. This is expected.

