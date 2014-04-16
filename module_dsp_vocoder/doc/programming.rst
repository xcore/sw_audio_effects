Programming Guide TODO
=================

Key Files
---------

   * ``vocoder.c``: Contains 'C' function library
   * ``vocoder.h``: Header file for 'C' function library

Usage
-----

There are 2 functions in the 'C' library, designed to be called from an xC file.

   * ``config_vocoder()`` Called to do initial configuration of vocoder parameters (e.g. No. of bands). WARNING: This must be called before ...
   * ``use_vocoder()`` Called to apply vocoder processing, designed to be called from an xC file.

Note, after initialization, ``config_vocoder()`` can be called again as required to re-configure the vocoder parameters.

The default vocoder parameters are set in the dsp_vocoder_control() function in file dsp_vocoder_control.xc.
These can be overwritten before calling config_vocoder.

Demo Applications
=================

Vocoder Application For sliceKIT Board
-------------------------------------------

To get started with this application, run through the instructions in the BiQuad Filter quickstart guide.

This application uses ``module_dsp_vocoder`` to create a vocoder-like effect on an audio-stream, 
passing through the sliceKIT Core board.
The audio is connected to the Audio Slice board using the following Analogue ports:

   * Input 0/1 (Microphone mini-jack)
   * Output 0/1 (Headphone mini-jack)

The following parameters are configurable:

   * sample frequency
   * Level of Dry (Input)
   * Level of Effect (Output)
   * No. Of Vocoder bands

Makefile
........

The Makefile is found in the top-level directory of the application (e.g. ``app_slicekit_vocoder``)

The application is for the sliceKIT Core Board so the TARGET variable needs to be set in the Makefile: e.g.

TARGET = SLICEKIT-L16.xn

The number of vocoder-bands-per-slave is currently set to 2 in auto_conf.h  e.g.
NUM_APP_SLAVE_BANDS = 2

Running the application with the Command Line Tools
...................................................

In the top-level directory of the application (e.g. ``app_slicekit_vocoder``) type

   * xmake clean
   * xmake all

Connect the board to your PC using an xTAG-2

To start the demo type

   * xrun --io bin/app_slicekit_vocoder.xe

When listening to the output audio at 48kHz sample rate, you should hear the following ...

   * About 8 seconds of the vocoder effect, followed by
   * About 8 seconds of the dry (un-effected) signal.

This cycle will repeat indefinitely.

Trouble-shooting
................

During initialization, and when the system is reconfigured, 
there may be audible clicks in the audio. This is expected.

Most recorded music already has vocoder added. 
So the effects of the vocoder application may not always be apparent.
Best to choose some dialogue or sparse percussive sound.

Try Other Audio Applications
----------------------------

