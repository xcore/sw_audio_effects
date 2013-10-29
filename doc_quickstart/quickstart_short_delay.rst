.. _slicekit_short_delay_Quickstart:

This document covers the setup and execution of the Short-Delay application, for real time audio processing. It requires the XA-SK-AUDIO sliceCARD.

Short-Delay sliceKIT Application 
--------------------------------

This is a demonstration of a simple audio application that uses a small number of software modules and slice cards, to produce a short audio delay effect.

Hardware Setup
++++++++++++++

The following hardware components are required:

* XP-SKC-L2 (sliceKIT L2 Core Board)
* XA-SK-AUDIO (Audio board)
* XA-SK-XTAG2 (sliceKIT xTAG adaptor)
* xTAG-2 (xTAG Connector board)

XP-SKC-L2 sliceKIT Core board has four slots with edge connectors: ``SQUARE``, ``CIRCLE``, ``TRIANGLE`` and ``STAR``, 
and one chain connector marked with a ``CROSS``.

To setup up the system:

#. Connect the XA-SK-AUDIO Audio board to sliceKIT Core board using the connector marked with the ``CIRCLE``.
#. Connect the xTAG Adapter board to sliceKIT Core board, using the chain connector marked with a ``CROSS``.
#. Connect xTAG-2 board to the xTAG adapter.
#. Set the ``xCONNECT`` to ``OFF`` on the xTAG Adapter.
#. Connect the xTAG-2 to host PC with a USB cable (not provided with the sliceKIT starter kit).
#. Connect analogue audio source (e.g. microphone) to minijack In_1/2
#. Connect analogue audio monitor (e.g. headphones) to minijack Out_1/2
#. Connect D/C barrel jack of XMOS power supply to sliceKIT Core board.
#. Switch on the power supply to the sliceKIT Core board.

.. figure:: images/hardware_setup_no_sdram.jpg
   :width: 500px
   :align: center

   Hardware Setup for Short-Delay sliceKIT Application
   
	
Import and Build the Application
++++++++++++++++++++++++++++++++

1. Open xTIMEcomposer and check that it is operating in online mode. 
   Open the edit perspective (Window->Open Perspective->XMOS Edit).
#. Locate the ``'Short-Delay sliceKIT Audio Demo'`` item in the xSOFTip pane on the bottom left of the window, 
   and drag it into the Project Explorer window in the xTIMEcomposer. 
   This will also cause the modules on which this application depends to be imported as well. 
   These modules are: module_dsp_short_delay, module_dsp_utils, module_i2s_master and module_i2c_single_port.
#. Click on the app_slicekit_short_delay item in the Explorer pane then click on the build icon (hammer) in xTIMEcomposer. 
   Check the console window to verify that the application has built successfully. 

For help in using xTIMEcomposer, try the xTIMEcomposer tutorial, that can be found by selecting Help->Tutorials from the xTIMEcomposer menu.

Note that the Developer Column in the xTIMEcomposer on the right hand side of your screen provides information on the xSOFTip components you are using. 
Select the module_dsp_short_delay component in the Project Explorer, and you will see its description together with API documentation. 
Having done this, click the `back` icon until you return to this quickstart guide within the Developer Column.

Run the Application
+++++++++++++++++++

Now that the application has been compiled, the next step is to run it on the sliceKIT Core Board using the tools to load the application over JTAG (via the xTAG2 and xTAG Adaptor card) into the xCORE multicore microcontroller.

#. Supply some analogue audio to the audio board (via mini jack In_1/2) by connecting a PC or phone audio output. Now try playing the XMOS introductory video found `here <http://www.xmos.com>`_. Alternatively connect and speak into a microphone if you have one.
#. Monitor the analogue audio from the audio board (via mini jack Out_1/2) by connecting some headphones, or sending the audio to an active speaker.
#. Click on the ``Run`` icon (the white arrow in the green circle). After 1 or 2 seconds fast echoes should be heard.
   The output switches between effect and dry signals about every 8 seconds, so the listener can compare the 2 modes.
    
Look at the Code
++++++++++++++++

Now that the application has been run with the default settings, you could try and adjust the delay configuration parameters.
Such as the number of delay taps.

#. Examine the application code. In xTIMEcomposer, navigate to the ``src`` directory under app_slicekit_short_delay 
   and double click on the ``main.xc`` file within it. The file will open in the central editor window.
#. Find the ``main.xc`` file and note that main() runs 2 cores (processes) in parallel connected by one channel.
   The processes are distributed over the tiles available on the sliceKIT Core board.

   * AUDIO_IO_TILE handles the Analogue-to-Digital and Digital-to-Analogue conversion.
   * c_aud_dsp is a bi-directional channel transferring 32-bit audio samples.
   * DSP_TILE handles the Digital Signal Processing required to filter the audio samples.
   
#. Find the app_global.h header. At the top are the tile definitions.
   Note that on the sliceKIT Core Board there are only 2 physical tiles 0 and 1.
   All audio DSP processes use cores on the same tile (1).
#. Find the dsp_delay.xc file. The function ``dsp_delay()`` handles the DSP processing for the delay line.
   It communicates with the other parallel core via channel c_aud_dsp.
   Data from these channels is buffered, and the buffers are passed to the ``use_delay_line()`` function for processing.
   ``use_delay_line()`` and ``config_delay_line()`` can be found in directory ``module_dsp_short_delay\src``. 
   Finally, there is a finite-state-machine which switches the output between the dry and effect signals.
#. The Delay-line uses no multiplies.  This application has been successfully tested with up to 4 delay taps, each on 4 channels.
   By inspecting timings, the processing time for one tap is about 80 cycles.
   For a 48 kHz sample rate and 500 MHz processor, this would allow about 100 taps/sample/channel.
