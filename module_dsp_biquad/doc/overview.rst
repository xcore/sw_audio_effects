Overview
========

This application filters an audio stream using a ``Digital BiQuad Filter`` as described `here <http://en.wikipedia.org/wiki/Digital_biquad_filter>`_.

It is implemented in Direct Form 2, which requires only two delay registers.

The BiQuad filter is flexible and can be configured in many different ways. For example, Low-Pass, Hi-Pass and so on.
