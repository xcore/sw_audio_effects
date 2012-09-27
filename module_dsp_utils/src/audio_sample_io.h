/******************************************************************************\
 * Header:  audio_sample_io
 * File:    audio_sample_io.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for audio_sample_io.xc
 *
 * Version: 0v1
 * Build:
 *
 * The copyrights, all other intellectual and industrial
 * property rights are retained by XMOS and/or its licensors.
 * Terms and conditions covering the use of this code can
 * be found in the Xmos End User License Agreement.
 *
 * Copyright XMOS Ltd 2012
 *
 * In the case where this code is a modification of existing code
 * under a separate license, the separate license terms are shown
 * below. The modifications to the code are still covered by the
 * copyright notice above.
 *
\******************************************************************************/

#ifndef _AUDIO_SAMPLE_IO_H_
#define _AUDIO_SAMPLE_IO_H_

#include <xs1.h>

/******************************************************************************/
void get_samples_from_input( // Get samples from Input channel
	chanend c_inp, // audio-in channel 
	int samples[], // Array or Output samples
	int num_chans // Number of channels
);
/******************************************************************************/
void give_samples_to_output( // Give samples to Output channel
	chanend c_out, // audio-out channel 
	const int samples[],  // Array or Input samples
	int num_chans // Number of channels
);
/******************************************************************************/

#endif // _AUDIO_SAMPLE_IO_H_
// audio_sample_io.h
