sw_audio_effects Change Log
===========================

1.0.3
-----

  * app_slicekit_dsp_effects demo updated to Output on both stereo pairs (Input only on channels 1/2)

  * Changes to dependencies:

    - sc_sdram_burst: 1.0.1rc1 -> 1.1.0rc0

      + Fixed init code
      + Fixed bug in refresh timing
      + Renamed regress application to testbench
      + Fixed code so that no warnings are emitted (apart from "bidirectional buffered port not supported ") when compiling with xTIMEcomposer13.
      + Documentation tidy up
      + Fixed regression test bug where code was not properly generalised
      + Added manafacture test app
      + Extended control defines to work for 4 bit ports in an arbitrary way
      + Added PINOUT_V2_IS42S16400F target
      + Added cc_tops and bottoms to all the assembly for elimination if unused
      + Added sdram_col_write() for writing to a single column quickly

    - sc_i2s: 1.4.1alpha0 -> 1.4.2rc1

      + Update dependencies.

1.0.2
-----

  * Tuning for reverb. Now uses 14 taps.
  * Added dsp_effects demo which combines various effects in one demo using the GPIO slice buttons.

1.0.1
-----

  * General code tidying and documentation udpates

1.0.0
-----

  * Initial Version
