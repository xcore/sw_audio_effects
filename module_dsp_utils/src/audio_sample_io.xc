/******************************************************************************\
 * File:	audio_sample_io.xc
 *
 * Description: Contains routines for buffering samples on audio channels
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

#include "audio_sample_io.h"

/******************************************************************************/
#pragma unsafe arrays
void get_samples_from_input( // Get samples from Input channel
	chanend c_inp, // audio-in channel
	int samples[], // Array or Output samples
	int num_chans // Number of channels
)
{
	int chan_cnt; // channel counter

	for (chan_cnt=0; chan_cnt<num_chans; chan_cnt++)
	{
		int sample;

		sample = inuint( c_inp ); // Receive sample from Input channel
		samples[chan_cnt] = sample;
	} // chan_cnt
} // get_samples_from_device
/******************************************************************************/
#pragma unsafe arrays
void give_samples_to_output( // Give samples to Output channel
	chanend c_out, // audio-out channel
	const int samples[],  // Array or Input samples
	int num_chans // Number of channels
)
{
	int chan_cnt; // channel counter

	for (chan_cnt=0; chan_cnt<num_chans; chan_cnt++)
	{
		int sample;

		sample = samples[chan_cnt];
		outuint( c_out ,sample ); // Transmit sample to Output channel
	} // for chan_cnt
} // give_samples_to_output
/******************************************************************************/
// audio_sample_io.xc
