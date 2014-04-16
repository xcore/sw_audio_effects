sw_audio_effects Change Log
===========================

1.0.4
-----
                  syncs with the channel I/O.  * Changes to dependencies:

    - sc_i2c: 2.2.1rc0 -> 2.4.1rc0

      + module_i2c_simple header-file comments updated to correctly reflect API
      + module_i2c_simple can now be built with support to send repeated starts and retry reads and writes NACKd by slave
      + module_i2c_shared added to allow multiple logical cores to safely share a single I2C bus
      + Removed readreg() function from single_port module since it was not safe

    - sc_i2s: 1.4.2rc4 -> 1.5.0rc1

      + RESOLVED:   ADC samples over the channel interface were misaligned.  Instead of the

1.0.3
-----
                  syncs with the channel I/O.                  syncs with the channel I/O.
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
                  syncs with the channel I/O.                  syncs with the channel I/O.
  * Tuning for reverb. Now uses 14 taps.
  * Added dsp_effects demo which combines various effects in one demo using the GPIO slice buttons.

1.0.1
-----
                  syncs with the channel I/O.                  syncs with the channel I/O.
  * General code tidying and documentation udpates

1.0.0
-----
                  syncs with the channel I/O.                  syncs with the channel I/O.
  * Initial Version
