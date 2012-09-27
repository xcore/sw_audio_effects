/******************************************************************************\
 * File:	common_utils.c
 * Author: Mark Beaumont
 * Description: Contains common routines used by DSP audio effects
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

#include "common_utils.h"

/******************************************************************************/
S32_T increment_circular_offset( // Increment a circular buffer offset. I.e. wraps if at end of buffer
	S32_T new_off, // required new offset (without wrapping)
	S32_T buf_siz // circular buffer size
) // Return wrapped offset
{
	// Check if at and of buffer
	if (buf_siz <= new_off)
	{
		return 0; // Move to start of buffer
	}

	return new_off; // Normal Increment
} // increment_circular_offset
/*****************************************************************************/
void cross_fade_sample( // Returns Cross-faded sample
	S32_T out_samps[],	// Buffer for cross-faded Output samples
	S32_T fade_out_samps[],	// Buffer for Input samples being faded-out
	S32_T fade_in_samps[],	// Buffer for Input samples being faded-in
	S32_T num_chans,	// Number of channels
	S32_T weight	// Weighting for fade-in sample
) // Return cross-faded sample
{
	S32_T chan_cnt; // Channel counter
	S64_T tmp_samp; // Intermediate sample value


	for(chan_cnt = 0; chan_cnt < num_chans; chan_cnt++)
	{
		tmp_samp = (S64_T)fade_in_samps[chan_cnt] - (S64_T)fade_out_samps[chan_cnt];
		tmp_samp = ((tmp_samp * weight) + (S64_T)HALF_FADE) >> FADE_BITS;
		out_samps[chan_cnt] = (S32_T)(tmp_samp + fade_out_samps[chan_cnt]);
	} // for chan_cnt
} // cross_fade_sample
/******************************************************************************/
// common_utils.c
