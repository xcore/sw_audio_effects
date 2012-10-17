.. _slicekit_long_reverb_Quickstart:

Long-Reverb SliceKit Application : Quick Start Guide
----------------------------------------------------

This is a demonstration of a complex audio application that uses the SDRAM Memory slice, a number of software modules, 
and additional slices, to produce a long 'reverb-like' effect on an audio stream.

Hardware Setup
++++++++++++++

The following hardware components are required:

* XP-SKC-L2 (Slicekit L2 Core Board)
* XA-SK-AUDIO (Audio board)
* XA-SK-SDRAM (Memory-Slice Board). This will need to be plugged into the same xCore Tile as the Audio card. 
* XA-SK-XTAG2 (Slicekit XTAG adaptor)
* XTAG-2 (XTAG Connector board)

XP-SKC-L2 Slicekit Core board has four slots with edge conectors: ``SQUARE``, ``CIRCLE``, ``TRIANGLE`` and ``STAR``, 
and one chain connector marked with a ``CROSS``.

To setup up the system:

#. Connect XA-SK-SDRAM Memory-Slice to the XP-SKC-L2 Slicekit Core board using the connector marked with the ``SQUARE``.
#. Connect the XA-SK-AUDIO Audio board to Slicekit Core board using the connector marked with the ``CIRCLE``.
#. Connect the XTAG Adapter board to Slicekit Core board, using the chain connector marked with a ``CROSS``.
#. Connect XTAG-2 board to the XTAG adapter.
#. Set the ``XMOS LINK`` to ``OFF`` on the XTAG Adapter.
#. Connect the XTAG-2 to host PC with a USB cable (not provided with the Slicekit starter kit).
#. Connect analogue audio source (e.g. microphone) to minijack In_1/2
#. Connect analogue audio monitor (e.g. headphones) to minijack Out_1/2
#. Connect D/C barrel jack of XMOS power supply to Slicekit Core board.
#. Switch on the power supply to the Slicekit Core board.

.. figure:: images/hardware_setup.jpg
   :width: 500px
   :align: center

   Fig_1: Hardware Setup for Long-Reverb SliceKit Application
   
	
Import and Build the Application
++++++++++++++++++++++++++++++++

1. Open xTIMEcomposer and check that it is operating in online mode. 
   Open the edit perspective (Window->Open Perspective->XMOS Edit).
#. Locate the ``'Long-Reverb SliceKit Application'`` item in the xSOFTip pane on the bottom left of the window, 
   and drag it into the Project Explorer window in the xTIMEcomposer. 
   This will also cause the modules on which this application depends to be imported as well. 
   These modules are: module_dsp_long_reverb, module_dsp_long_delay, module_dsp_loudness, module_dsp_biquad, 
   module_dsp_utils, module_sdram_memory_mapper, module_sdram, module_i2s_master and module_i2c_single_port.
#. Click on the app_slicekit_long_reverb item in the Explorer pane then click on the build icon (hammer) in xTIMEcomposer. 
   Check the console window to verify that the application has built successfully. 
   There will be quite a number of warnings that ``bidirectional buffered port not supported in hardware``. 
   These can be safely ignored for this component.

For help in using xTIMEcomposer, try the xTIMEcomposer tutorial, that can be found by selecting Help->Tutorials from the xTIMEcomposer menu.

Note that the Developer Column in the xTIMEcomposer on the right hand side of your screen 
provides information on the xSOFTip components you are using. 
Select the module_dsp_long_reverb component in the Project Explorer, and you will see its description together with API documentation. 
Having done this, click the `back` icon until you return to this quickstart guide within the Developer Column.

Run the Application
+++++++++++++++++++

Now that the application has been compiled, the next step is to run it on the Slicekit Core Board using the tools 
to load the application over JTAG (via the XTAG2 and Xtag Adaptor card) into the xCORE multicore microcontroller.

#. Supply some analogue audio to the audio board (via mini jack In_1/2). 
   E.g. by connecting the output of an MP3 player, or speaking into a microphone.
#. Monitor the analogue audio from the audio board (via mini jack Out_1/2). 
   E.g. by connecting some headphones, or sending the audio to an active speaker.
#. Click on the ``Run`` icon (the white arrow in the green circle). After a 1 or 2 seconds the reverb effect should be heard.
   The output switches between effect and dry signals about every 8 seconds, so the listener can compare the 2 modes.
    
Next Steps
++++++++++

Now that the application has been run with the default settings, you could try and adjust the reverb configuration parameters. 
Such as room-size and frequency aborption. Note well, some combinations may produce uncontrollable feedback.

Look at the Code
................

#. Examine the application code. In xTIMEcomposer, navigate to the ``src`` directory under app_slicekit_long_reverb 
   and double click on the ``main.xc`` file within it. The file will open in the central editor window.
#. Find the ``main.xc`` file and note that main() runs 5 cores (processes) in parallel. 
   These are distributed over the tiles available on the SliceKit Core board.
#. Find the app_global.h header. At the top are the tile definitions.
   Note that on the SliceKit Core Board there are only 2 physical tiles 0 and 1.
   Long-reverb is a time critical application. It is important that the SDRAM (Memory-slice) 
   is connected to the same tile as the one running the delay functions (dsp_sdram_reverb).
#. Find the dsp_sdram_reverb.xc file. The function ``dsp_sdram_reverb()`` controls the DSP processing for the reverb.
   It communicates with the other 4 parallel cores, via 4 channels: c_aud_dsp, c_dsp_eq, c_dsp_gain, and c_dsp_sdram.
   Data from these channels is buffered, and the buffers are passed to the ``use_sdram_reverb()`` function for processing.
   ``use_sdram_reverb()`` and ``config_sdram_reverb()`` can be found in directory ``module_dsp_long_reverb\src``. 
   Finally, there is a finite-state-machine which switches the output between the dry and effect signals.

Try Other Audio Applications which use the delay effect
.......................................................

Other significant applications are ``app_slicekit_short_reverb`` and ``app_slicekit_long_delay``. 

* The short reverb application does NOT use the XA-SK-SDRAM (Memory-Slice board).
  Instead it uses only local memory, this limits the room-size that can be achieved to below 36 metres.
* The long delay application combines the XA-SK-SDRAM (Memory-Slice board) with the XA-SK-AUDIO Audio board,
  to create delays of up to 20 seconds (for a stereo channel at 48 kHz).
