/******************************************************************************\
 * File:	vocoder.c
 *
 * Description: Performs processing for a number of vocoder frequency bands
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

#include "vocoder.h"

// Structure containing all vocoder data (NB Make global to avoid using malloc)
static VOCODER_S vocoder_gs = { .init_done = 0, .params_set = 0 };

/*****************************************************************************/
void init_vocoder( // Initialise vocoder parameters
	VOCODER_S * vocoder_ps // Pointer to structure containing vocoder data
)
{
} // init_vocoder
/*****************************************************************************/
void config_vocoder( // Configure vocoder parameters
	VOCODER_PARAM_S * vocoder_param_ps, // Pointer to structure containing vocoder parameters
	S32_T slave_id // Unique slave identifier
)
{
	// Check if Delay-Line initialised
	if (0 == vocoder_gs.init_done)
	{ // Initialse Reverb
		init_vocoder( &(vocoder_gs) );

		vocoder_gs.init_done = 1; // Signal Reverb initialised
	} // if (0 == vocoder_gs->init_done)

	vocoder_gs.params_set = 1; // Signal Reverb parameters configured

} // config_vocoder
/*****************************************************************************/
void use_vocoder( // Controls audio stream processing for vocoder application using dsp functions
	SAMP_CHAN_T out_samps[], 	// Buffer for Processed Output samples
	SAMP_CHAN_T inp_samps[],	// Buffer containing Unprocessed input samples
	S32_T slave_id // Unique slave Identifier
)
{
	S32_T chan_cnt; // Channel counter
	S32_T other_chan; // Other channel of stereo pair


	// Check if vocoder parameters have been initialised
	if (0 == vocoder_gs.params_set)
	{
		assert(0 == 1); // Please call config_vocoder() function before use_vocoder()
	} // if (0 == vocoder_gs.params_set)
	else
	{
		// Loop through set of channel samples and process delay taps
		for(chan_cnt = 0; chan_cnt < NUM_VOCODER_OUT_CHANS; chan_cnt++)
		{
			out_samps[chan_cnt] = inp_samps[0]; // Copy Mono signal to both stereo channels
		} // for chan_cnt
	} // else !(0 == vocoder_gs.params_set)
} // use_vocoder
/*****************************************************************************/
// vocoder.c
