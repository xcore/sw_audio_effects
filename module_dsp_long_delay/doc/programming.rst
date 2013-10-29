Programming Guide
=================

Key Files
---------

   * ``sdram_delay.c``: Contains 'C' function library
   * ``sdram_delay.h``: Header file for 'C' function library

Usage
-----

There are 2 functions in the 'C' library, designed to be called from an xC file.
   * ``config_sdram_delay()`` Called to do initial configuration of delay parameters (e.g. delay offsets). WARNING: This must be called before ...
   * ``use_sdram_delay()`` Called to apply set of delays to one audio-sample, designed to be called from an xC file.

Note, after initialization, ``config_sdram_delay()`` can be called again as required to re-configure the delay offsets.

Delays are set in microseconds in the us_delays field of the DELAY_PARAM_S structure.

Demo Applications
=================

To get started with this application, run through the instructions in the slicekit_biquad_filter_Quickstart_ quickstart guide.

Long-Delay Application For sliceKIT Board (``app_slicekit_long_delay``)
-----------------------------------------

This application uses module_dsp_long_delay to create a set of delays (or echoes) for an audio-stream, 
passing through the sliceKIT Core board.
The audio is connected to the Audio Slice board using the following Analogue ports:
   * Input  0/1 (Microphone mini-jack)
   * Output 0/1 (Headphone mini-jack)

An intermediate signal is created mixing the input stream with the output of the delay-line,
which is set-up to produce 4 different echoes. 
This intermedate signal is used for the output, 
but also fed back into the delay-line, to create a potentially infinite set of echoes.
In fact the mixing is arranged so that the echoes decay exponentially in amplitude.

Makefile
........

The Makefile is found in the top-level directory of the application (e.g. ``app_slicekit_long_delay``)

The application is for the sliceKIT Core Board so the TARGET variable needs to be set in the Makefile: e.g

TARGET = SLICEKIT-L2.xn

The number of channels supported in currently 2 or 4, this is set in auto_conf.h: e.g.

NUM_DELAY_CHANS = 2

Running the application with the Command Line Tools
...................................................

In the top-level directory of the application (e.g. ``app_slicekit_long_delay``) type
   * xmake clean
   * xmake all

Connect the board to your PC using an xTAG-2

To start the demo type
   * xrun --io bin/app_slicekit_long_delay.xe

When listening to the output audio at 48kHz sample rate, you should hear the following ...
   * About 16 seconds of the effect (i.e. multiple echoes decaying in amplitude), followed by
   * About 8 seconds of the dry (un-effected) signal.

This cycle will repeat indefinitely.

Trouble-shooting
................

During initialization, and when the system is reconfigured, 
there may be audible clicks in the audio. This is expected.

Try Other Audio Applications
----------------------------

Another audio application is ``app_slicekit_long_reverb``.

* The long reverb application is an example of a more complex application running 4 cores, 
  and combining the biquad, loudness and long-delay modules to create large room-sizes, e.g. 200 meters (for a stereo channel at 48 kHz).
	See doc_quickstart/quickstart_reverb_long.rst
