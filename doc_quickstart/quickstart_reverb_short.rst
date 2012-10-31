.. _slicekit_short_reverb_Quickstart:

Short Reverb SliceKit Application
---------------------------------

This is a demonstration of a complex audio application that uses a number of software modules to produce a short 'reverb-like' effect on an audio stream.

Hardware Setup
++++++++++++++

Leave the setup is unchanged from the Biquad application demo.
	
Import and Build the Application
++++++++++++++++++++++++++++++++

#. Locate the ``'Short-Reverb Slicekit Audio Demo'`` item in the xSOFTip pane on the bottom left of the window, 
   and drag it into the Project Explorer window in the xTIMEcomposer. 
   This will also cause the modules on which this application depends to be imported as well. 
#. Click on the app_slicekit_short_reverb item in the Explorer pane then click on the build icon (hammer) in xTIMEcomposer to build it as before. 


Run the Application
+++++++++++++++++++

#. If the previous application is still running, click on the red square button in the debug console to stop it.
#. Leaving the hardware setup unchanged Click on the ``Run`` icon and ensure the XMOS video is playing. After a 1 or 2 seconds the reverb effect should be heard. The output switches between effect and dry signals about every 8 seconds, so the listener can compare the 2 modes. Since this application only uses internal SRAM for audio buiffering, the reverb effect is of limited length. 
    
Look at the Code
++++++++++++++++


#. Find the app_global.h header. At the top are the tile definitions.
   Note that on the SliceKit Core Board there are only 2 physical tiles 0 and 1.
   To maximise the amount of memory available for the process (core) controlling the delay (`dsp_control()``),
   it is given it's own tile. All other cores are placed on the other tile.
#. Find the dsp_reverb.xc file. The function ``dsp_control()`` handles the DSP processing for the reverb.
   It communicates with the other 3 parallel cores, via 3 channels: c_aud_dsp, c_dsp_eq, and c_dsp_gain.
   Data from these channels is buffered, and the buffers are passed to the ``use_reverb()`` function for processing.
   ``use_reverb()`` and ``config_reverb()`` can be found in directory ``module_dsp_short_reverb\src``. 
   Finally, there is a finite-state-machine which switches the output between the dry and effect signals.
