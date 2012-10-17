
Evaluation Platforms
====================

.. _sec_hardware_platforms:

Recommended Hardware
--------------------

Slicekit
++++++++

This module may be evaluated using the Slicekit Modular Development Platform, available from digikey. 
Minimum Required board SKUs are:

   * XP-SKC-L2 (Slicekit L2 Core Board) plus XA-SK-AUDIO plus XA-SK-XTAG2 (Slicekit XTAG adaptor)
   * XA-SK-SDRAM (Memory-Slice Board) plugged into same Tile as running ``main.xc``

Demonstration Applications
--------------------------

Stand Alone Demo
++++++++++++++++

Example stand alone usage of this module can be found within the XSoftIP suite as follows:

   * Package: sw_audio_effects
   * Application: app_slicekit_long_delay

Reverb Demo
+++++++++++

This module is used in the app_slicekit_long_reverb example application which shows how the module is deployed 
within the context of a more complex audio processing application. 
Note that in addition to the recommended hardware above, the XA-SK-SDRAM Slice Card will also be required, 
since this application buffers more audio samples than can be accomodated in the free internal SRAM of the xCore Tile.

   * Package: sw_audio_effects
   * Application: app_slicekit_long_reverb
