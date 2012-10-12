/******************************************************************************\
 * File:	reverb.c
 *  
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

// Structure containing all reverb data (NB Make global to avoid using malloc)
static REVERB_S reverb_gs = { .init_done = 0, .params_set = 0 }; // Clear initialisation flags

/*****************************************************************************/
void config_build_delay( // Calculate delay parameters and call delay configuration
	REVERB_S * reverb_ps, // Pointer to structure containing reverb data
	REVERB_PARAM_S * reverb_param_ps // Pointer to structure containing reverb parameters
)
{
	DELAY_PARAM_S delay_param_s =  { .num = NUM_REV_TAPS }; // Default Delay-line Configuration
	U64_T factor; // scaling factor for delay taps
	S32_T tap_cnt; // delay measured in samples


	delay_param_s.freq = reverb_param_ps->samp_freq; // Assign requested sample frequency
	
	// Calculate Delay-line taps (in milli-seconds) for requested room-size ...

	factor = (S64_T)1000000 * (S64_T)INV_SOS * (S64_T)reverb_param_ps->room_size; // common factor to convert delay to milli-seconds

	// Loop through all delay taps.
	for (tap_cnt = 0; tap_cnt < NUM_REV_TAPS; tap_cnt++)
	{
		// Calculate delay taps in samples.
		delay_param_s.us_delays[tap_cnt] 
			= (S32_T)((factor * (S64_T)reverb_ps->tap_ratios[tap_cnt] + HALF_REV_SCALE ) >> SCALE_REV_BITS);
	} // for tap_cnt

	config_delay_line( &(delay_param_s) );

} // config_build_delay
/*****************************************************************************/
void init_reverb( // Initialise reverb parameters
	REVERB_S * reverb_ps // Pointer to structure containing reverb data
)
{
	assert(4 == NUM_REV_TAPS); // ERROR: Only NUM_REV_TAPS=4 supported

	reverb_ps->tap_ratios[0] = TAP_0;
	reverb_ps->tap_ratios[1] = TAP_1;
	reverb_ps->tap_ratios[2] = TAP_2;
	reverb_ps->tap_ratios[3] = TAP_3;

	assert(MAX_TAP == reverb_ps->tap_ratios[NUM_REV_TAPS - 1]); // Calculations depend on Max tap value of MAX_TAP

} // init_reverb
/*****************************************************************************/
void config_reverb( // Configure reverb parameters
	REVERB_PARAM_S * reverb_param_ps // Pointer to structure containing reverb parameters
)
{
	// Check if Delay-Line initialised
	if (0 == reverb_gs.init_done)
	{ // Initialse Reverb
		init_reverb( &(reverb_gs) );

		reverb_gs.init_done = 1; // Signal Reverb initialised
	} // if (0 == reverb_gs->init_done)

	assert(MIN_AUDIO_FREQ <= reverb_param_ps->samp_freq); // Check for sensible frequency

	/* Configure Delay-line which is in this thread. 
		NB BiQuad and Loudness are in different threads and therefore configuration is synchronised via dsp_control.xc */

	config_build_delay( &(reverb_gs) ,reverb_param_ps );  

	// Assign requested mix setting ...

	reverb_gs.mix_lvls_ps = &(reverb_param_ps->mix_lvls);

	reverb_gs.params_set = 1; // Signal Reverb parameters configured

} // config_reverb
/*****************************************************************************/
void use_reverb( // Controls audio stream processing for reverb application using dsp functions
	SAMP_CHAN_T uneq_o_samps[],	// Buffer for Unequalised output samples
	SAMP_CHAN_T rev_o_samps[],	// Buffer for output samples with Reverb added
	SAMP_CHAN_T out_samps[],	// Buffer for final Output samples 
	SAMP_CHAN_T inp_samps[],	// Buffer containing Dry input samples
	SAMP_CHAN_T equal_i_samps[],	// Buffer containing Equalised input samples
	SAMP_CHAN_T amp_i_samps[]	// Buffer containing Amplified input samples
)
{
	static SAMP_CHAN_T delay_samps[NUM_REV_TAPS]= {0,0,0,0};	// Set of delayed samples for one channel
	MIX_PARAM_S * mix_lvls_ps = reverb_gs.mix_lvls_ps; // Local Pointer to structure containing mix-levels
	S64_T same_samps[NUM_REVERB_CHANS];	// Same-channel buffer for Channel-Mix (E.g. left->left)
	S64_T swap_samps[NUM_REVERB_CHANS];	// Swap-channel buffer for Channel-Mix (E.g. left->right)
	S64_T samp_sum;	// Intermediate sample sum
	S64_T samp_diff;	// Intermediate sample difference
	S32_T chan_cnt; // Channel counter
	S32_T other_chan; // Other channel of stereo pair
	

	// Check if reverb parameters have been initialised
	if (0 == reverb_gs.params_set)
	{
		assert(0 == 1); // Please call config_reverb() function before use_reverb() 
	} // if (0 == reverb_gs.params_set)
	else
	{
		// Loop through set of channel samples and process delay taps
		for(chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// NB Equalisation converts uneq_o_samps to equal_i_samps
// equal_i_samps[chan_cnt] = uneq_o_samps[chan_cnt]; //MB~ Dbg

			// Get Delayed Samples
			use_delay_line( delay_samps ,equal_i_samps[chan_cnt] ,chan_cnt );

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
			samp_diff = (S64_T)mix_lvls_ps->attn_mix * ((S64_T)inp_samps[chan_cnt] - samp_sum);
			uneq_o_samps[chan_cnt] = (S32_T)(samp_sum + ((samp_diff + (S64_T)MIX_DIV2) >> MIX_BITS));
// uneq_o_samps[chan_cnt] = inp_samps[chan_cnt]; //MB~ Dbg
		} // for chan_cnt

		// NB Requires two loops, as left and right channel need to have been updated before mixing can be done
		// Loop through set of channel samples and produce Left/Right output mixes
		for(chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			other_chan = chan_cnt ^ 1; // Get Id for other channel (Assumes 0 <--> 1, 2 <--> 3, etc)

			// Mix Left and Right channels 
			samp_diff = (S64_T)mix_lvls_ps->cross_mix * (swap_samps[other_chan] - same_samps[chan_cnt]);
			rev_o_samps[chan_cnt] = (S32_T)(same_samps[chan_cnt] + ((samp_diff + (S64_T)MIX_DIV2) >> MIX_BITS));
// rev_o_samps[chan_cnt] = equal_i_samps[other_chan]; //MB~ Dbg

			// NB Non-linear-gain thread converts rev_o_samps to amp_i_samps
// amp_i_samps[chan_cnt] = rev_o_samps[chan_cnt]; // MB~ Dbg

			// Mix Dry and Reverb Signals (User Controlable)
			samp_sum = (S64_T)mix_lvls_ps->dry_lvl * (S64_T)inp_samps[chan_cnt]; // Initialise with Dry signal level
			samp_sum += (S64_T)mix_lvls_ps->fx_lvl * (S64_T)amp_i_samps[chan_cnt]; // Add-in Effect signal level
			out_samps[chan_cnt] = (S32_T)((samp_sum + (S64_T)MIX_DIV2) >> MIX_BITS);
//			out_samps[chan_cnt] = amp_i_samps[chan_cnt]; // MB~ Dbg
		} // for chan_cnt
	} // else !(0 == reverb_gs.params_set)

} // use_reverb
/*****************************************************************************/
// reverb.c
