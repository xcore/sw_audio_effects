Overview
========

This application boosts the perceived Loudness of an audio stream, by using a non-linear gain-shaping curve.
This ensures maximum allowed signal level is NOT exceeded, and the low-level noise is not increased.
The non-linear curve is implemented as a piece-wise parabolic curve.
Currently 5 pieces are used.

DESCRIPTION
-----------
This algorithm uses a piece-wise parabolic curve, to limit the number of multiplies required per sample.
Currently 5 parabola's are used, as follows:-

   * Unity-gain section (Straight line) for low-level signals (e.g. noise)
   * Transition to maximum gradient (user-configurable). 
   * Transition from max. gradient to 'max-gain' section.
   * 'max-gain' section, (Straight line) with minimum gradient (reciprocal of max-gradient)
   * Transition from min. gradient back to 'unity-gain' to finish.
