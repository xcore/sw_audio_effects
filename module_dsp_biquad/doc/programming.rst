Programming Guide
=================

Key Files
---------

   * ``biquad_simple.c``: Contains 'C' function library
   * ``biquad_simple.h``: Header file for 'C' function library

Usage
-----

There are 2 functions in the 'C' library, designed to be called from an xC file.

   * ``config_biquad_filter()`` Called to do initial configuration of biquad parameters (e.g. Low-pass). WARNING: This must be called before ...
   * ``use_biquad_filter()``, which is called to apply the filter to one audio-sample, designed to be called from an xC file.

Note well: After initialization, ``config_biquad_filter()`` can be called again as required to re-configure the filter.

To set the filter parameters, edit the following defines in the include file module_dsp_biquad/src/biquad_simple.h

   * DEF_FILT_MODE // Default Filter Mode
   * DEF_SAMP_FREQ // Default Sample Frequency
   * DEF_SIG_FREQ // Default Significant Filter Frequency (E.g. Low-pass cut-off)
   * DEF_QUAL_FACT // Default Quality-factor

Demo Applications
=================

BiQuad Application For sliceKIT Board (app_slicekit_biquad)
-----------------------------------------------------------

To get started with this application, run through the instructions in the slicekit_biquad_filter_Quickstart_ quickstart guide.

This application uses module_dsp_biquad to apply a filter to an audio-stream passing through the sliceKIT Core board.
The audio is connected to the Audio Slice board using the following Analogue ports:

   * Input 0/1 (Microphone mini-jack)
   * Output 0/1 (Headphone mini-jack)

Makefile
........

The Makefile is found in the top-level directory of the application (e.g. app_slicekit_biquad)

The application is for the sliceKIT Core Board so the TARGET variable needs to be set in the Makefile: e.g
TARGET = SLICEKIT-L2.xn

The number of channels supported in currently 2 or 4, this is set in auto_conf.h: e.g.
NUM_BIQUAD_CHANS = 2

Running the application with the Command Line Tools
...................................................

In the top-level directory of the application (e.g. app_slicekit_biquad) type

   * xmake clean
   * xmake all

Connect the board to your PC using an xTAG-2

To start the demo type

   * xrun --io bin/app_slicekit_biquad.xe

When listening to the output audio at 48kHz sample rate, you should hear the following ...

   * About 8 seconds of the effect (i.e. low-pass audio), followed by
   * About 8 seconds of the dry (un-effected) signal (i.e. quieter audio)

Each time the effect is chosen, the filter switches to a different mode.
Currently the following 6 modes are supported:

   * LO_PASS,			// Low Pass Filter
   * HI_PASS,			// High Pass Filter
   * BAND_PASS,		// Band Pass Filter
   * BAND_STOP,		// Band Stop (Notch) Filter
   * ALL_PASS,			// Phase-Shift Filter
   * CUSTOM,				// Custom Filter

This cycle will repeat indefinitely.

Trouble-shooting
................

During initialization, and when the system is reconfigured, 
there may be audible clicks in the audio. This is expected.

The filter may produce output audio that is louder than the input,
especially when high value for Quality_Factor are selected.
This in turn may produce audible distortion in the output audio.
If this occurs, try one of the following:

   * Turning down the volume of the audio source (i.e. the volume level input to the application), 
   * Reduce the value of DEF_QUAL_FACT

Try Other Audio Applications
----------------------------

Some of the other audio applications are ``app_slicekit_loudness``, ``app_slicekit_short_delay``, ``app_slicekit_short_reverb``.

* The loudness application increases the perceived loudness, without increasing the maximum volume.
	See doc_quickstart/quickstart_loudness.rst
* The short delay application creates a multiple echo effect, by storing audio in local memory.
	See doc_quickstart/quickstart_short_delay.rst
* The short reverb application is an example of a more complex application running 4 cores, 
  and combining the biquad, loudness and short-delay modules to create room-sizes up to about 36 meters (for a stereo channel at 48 kHz).
	See doc_quickstart/quickstart_reverb_short.rst
