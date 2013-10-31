.. _slicekit_long_delay_Quickstart:

Long-Delay sliceKIT Application 
--------------------------------

This is a demonstration of a simple audio application that adds the XA-SK-SDRAM sliceCARD to the existing setup using the XA-SK-AUDIO sliceCARD, enabling larger amounts of audio buffering and hence more long lived and easily audible audio effects.

Hardware Setup
++++++++++++++

To setup up the system:

#. Turn off the power to the sliceKIT Core Board.
#. Connect XA-SK-SDRAM Memory-Slice to the XP-SKC-L16 sliceKIT Core board using the connector marked with the ``SQUARE``.
#. Leave the rest of the setup unchanged from before.

.. figure:: images/hardware_setup.jpg
   :width: 500px
   :align: center

   Hardware Setup for Long-Delay sliceKIT Application
   
	
Import and Build the Application
++++++++++++++++++++++++++++++++

#. Locate the ``'Long-Delay sliceKIT Audio Demo'`` item in the xSOFTip pane on the bottom left of the window, 
   and drag it into the Project Explorer window in the xTIMEcomposer. 
#. Click on the app_slicekit_long_delay item in the Explorer pane then click on the build icon (hammer) in xTIMEcomposer. 
   Check the console window to verify that the application has built successfully. There will be quite a number of warnings that ``bidirectional buffered port not supported in hardware``. These can be safely ignored for this component.

Run the Application
+++++++++++++++++++

#. Restart the audio source.
#. Click on the ``Run`` icon (the white arrow in the green circle). After 1 or 2 seconds the delay effect should be heard.
   The output switches between effect and dry signals, so the listener can compare the 2 modes: 
   16 seconds of the effect, followed by 8 seconds of the dry signal.
    
Look at the Code
++++++++++++++++

#. Examine the application code. In xTIMEcomposer, navigate to the ``src`` directory under app_slicekit_long_delay 
   and double click on the ``main.xc`` file within it. The file will open in the central editor window.
#. Find the ``main.xc`` file and note that main() runs 3 cores (processes) in parallel. 
   These are distributed over the tiles available on the sliceKIT Core board.
#. Find the app_global.h header. At the top are the tile definitions.
   Note that on the sliceKIT Core Board there are only 2 physical tiles 0 and 1.
   Long-Delay is a time critical application. It is important that the SDRAM (Memory-slice) 
   is connected to the same tile as the one running the delay functions (dsp_sdram_delay).
#. Find the dsp_sdram_delay.xc file. The function ``dsp_sdram_delay()`` controls the DSP processing for the delay.
   It communicates with the other 2 parallel cores, via 2 channels: c_aud_dsp and c_dsp_sdram.
   Data from these channels is buffered, and the buffers are passed to the ``use_sdram_delay()`` function for processing.
   ``use_sdram_delay()`` and ``config_sdram_delay()`` can be found in directory ``module_dsp_long_delay\src``. 
   Finally, there is a finite-state-machine which switches the output between the dry and effect signals.

