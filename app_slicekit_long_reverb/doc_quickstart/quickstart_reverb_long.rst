.. _slicekit_long_reverb_Quickstart:

Long-Reverb SliceKit Application 
--------------------------------

This application introduces additional processing over and above the simple SDRAM based delay demo, using the BiQuad filter, long delay and Non-linear gain modules to create a proper reverberation effect.

Hardware Setup
++++++++++++++

#. Leave the setup unchanged from the Long Delay demo.

	
Import and Build the Application
++++++++++++++++++++++++++++++++

#. Locate the ``'Long-Reverb Slicekit Audio Demo'`` item in the xSOFTip pane on the bottom left of the window, 
   and drag it into the Project Explorer window in the xTIMEcomposer. 
#. Click on the app_slicekit_long_reverb item in the Explorer pane then click on the build icon (hammer) in xTIMEcomposer. 

Run the Application
+++++++++++++++++++

#. Restart the audio source.
#. Click on the ``Run`` icon (the white arrow in the green circle). After a 1 or 2 seconds the reverb effect should be heard.
   The output switches between effect and dry signals about every 8 seconds, so the listener can compare the 2 modes.
    
Look at the Code
++++++++++++++++

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

