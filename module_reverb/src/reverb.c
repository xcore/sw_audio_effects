/******************************************************************************\
 * File:	reverb.c
 * Author: Mark Beaumont
 * Description: Control thread for Reverb, also handles delay functionality, 
 *	calls Loudness and Equalisation threads
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

#include "reverb.h"

/*****************************************************************************/
void reverb( // Controls audio stream processing for reverb application using dsp functions
	SAMP_CHAN_T uneq_o_samps[],	// Output Unequalised audio sample buffer
	SAMP_CHAN_T rev_o_samps[],	// Buffer for output samples with Reverb added
	SAMP_CHAN_T inp_samps[],	// Input sample buffer
	SAMP_CHAN_T equal_i_samps[],	// Buffer for input equalised samples
	S32_T inp_chans,	// Number of input channels (NB Only Stereo supported)
	S32_T samp_freq, // Current sample frequency
	S32_T delay_ms // Delay time in milli-seconds
)
{
	static SAMP_CHAN_T delay_samps[NUM_TAPS]= {0,0,0,0};	// Set of delayed samples for one channel
	S64_T chan_mix;	// Left+Right Channel-Mix
	S64_T same_samps[NUM_CHANS];	// Same-channel buffer for Channel-Mix (E.g. left->left)
	S64_T swap_samps[NUM_CHANS];	// Swap-channel buffer for Channel-Mix (E.g. left->right)
	S64_T samp_sum;	// Intermediate sample sum
	S64_T samp_diff;	// Intermediate sample difference
	S32_T chan_cnt; // Channel counter
	

	assert( NUM_CHANS == inp_chans ); // ERROR: Only Stereo supported

	assert( 4 == NUM_TAPS); // ERROR: Only NUM_TAPS=4 supported

	// Loop through set of channel samples and process delay taps
	for(chan_cnt = 0; chan_cnt < NUM_CHANS; chan_cnt++)
	{
		// Get Delayed Samples
		delay_line_wrapper( delay_samps ,equal_i_samps[chan_cnt] ,chan_cnt ,samp_freq ,delay_ms );

		// Sum 'Early Reflections' for left and right channels
		same_samps[chan_cnt] = (S64_T)delay_samps[0] + (S64_T)delay_samps[1];		// A + B 
		swap_samps[chan_cnt] = (S64_T)delay_samps[2] + (S64_T)delay_samps[3];		// C + D

		// Sum 4 weighted delay taps as follows: (19A + 17B + 15C + 13D)/64
		samp_sum = (same_samps[chan_cnt] + swap_samps[chan_cnt]) << 4;						// 16(A + B + C + D)
		samp_diff = (S64_T)delay_samps[0] - (S64_T)delay_samps[3];								// (A - D)
		samp_diff += (samp_diff << 1);																						// 3(A - D)
		samp_sum += (samp_diff + (S64_T)delay_samps[1] - (S64_T)delay_samps[2]);	// 19A + 17B + 15C + 13D
		samp_sum = (S32_T)((samp_sum + 32) >> 6); // Normalised value

		// Control Reverb Feedback/Attenuation (User Controlable)
		uneq_o_samps[chan_cnt] = (S32_T)(( (S64_T)inp_samps[chan_cnt] + (63 * samp_sum) + 32) >> 6);
	} // for chan_cnt

	// NB Requires two loops, as left and right channel need to have been updated before mixing can be done
	// Loop through set of channel samples and produce Left/Right output mixes
	for(chan_cnt = 0; chan_cnt < NUM_CHANS; chan_cnt++)
	{
		// Mix Left and Right channels 
		chan_mix = ((same_samps[chan_cnt] << 1) + same_samps[chan_cnt] + swap_samps[1 - chan_cnt] + 1) >> 1; // E.g. (3L + R)/4

		// Mix Dry and Reverb Signals (User Controlable)
		rev_o_samps[chan_cnt] = (S32_T)(( (S64_T)inp_samps[chan_cnt] + (63 * chan_mix) + 2) >> 2);
	} // for chan_cnt

} // reverb
/*****************************************************************************/
// reverb.c
