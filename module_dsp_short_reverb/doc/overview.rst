Overview
========

This application creates a short 'reverb-like' effect.
The input audio stream is fed into a complex DSP function that uses multiple delays, equalisation, gain-shaping and feedback.
The dsp_function returns an output stream that is a mix of the input and effect signal.
The following characteristics of the reverb are currently configurable:

   * Room-size (in metres)
   * Frequency absorption (Low-pass filter cut-off frequency)
   * Volume absorption ('bring-up' gain)

The following signal-mix levels are currently configurable:

   * Dry (input) signal
   * Reverb (pure effect) signal
   * Reverb attenuation (controls time of reverb decay)
   * Cross-channel mix (Controls movement between  Mono-reverb <--> Stereo-reverb <--> Opposite-reverb )
