/******************************************************************************\
 * File:	dsp_control.xc
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

#include "dsp_reverb.h"

// DSP-control thread.

/*****************************************************************************/
void send_biquad_config( // Send BiQuad filter configuration data
	streaming chanend c_dsp_eq, // Channel connecting to Equalisation thread (bi-directional)
	REVERB_PARAM_S &reverb_param_s // Reference to structure containing reverb parameters
)
{
	// Default BiQuad Configuration
	BIQUAD_PARAM_S biquad_param_s = { DEF_FILT_MODE ,DEF_SAMP_FREQ ,DEF_SIG_FREQ ,DEF_QUAL_FACT };


	biquad_param_s.samp_freq = reverb_param_s.samp_freq; // Assign requested sample frequency
	biquad_param_s.sig_freq = reverb_param_s.sig_freq; // Assign requested significant (lo-pass) frequency

	soutct( c_dsp_eq ,CFG_BIQUAD ); // Signal BiQuad configuration data transmission

	c_dsp_eq <: biquad_param_s; // Send BiQuad filter configuration data
} // config_build_biquad
/*****************************************************************************/
void send_loudness_config( // Send Loudness configuration data
	streaming chanend c_dsp_gain, // Channel connecting to Loudness thread (bi-directional)
	REVERB_PARAM_S &reverb_param_s // Reference to structure containing reverb parameters
)
{
	GAIN_PARAM_S gain_param_s = { DEF_GAIN }; // Default Loudness Configuration


	gain_param_s.gain = reverb_param_s.gain; // Assign requested maximum gain

	soutct( c_dsp_gain ,CFG_GAIN ); // Signal Gain configuration data transmission

	c_dsp_gain <: gain_param_s; // Send Loudness configuration data
} // send_loudness_config
/*****************************************************************************/
void sync_reverb_config( // Synchronise a reverb configuration (with other threads)
	streaming chanend c_dsp_eq, // Channel connecting to Equalisation thread (bi-directional)
	streaming chanend c_dsp_gain, // Channel connecting to Loudness thread (bi-directional)
	REVERB_PARAM_S &reverb_param_s // Structure containing Reverb configuration parameters
)
{
	// Send data to other threads ...
	send_biquad_config( c_dsp_eq ,reverb_param_s );		// Send BiQuad filter configuration data
	send_loudness_config( c_dsp_gain ,reverb_param_s );	// Send Gain-shaping configuration data

	config_reverb( reverb_param_s ); // Configure remaining reverb parameters in this thread (Delay-line)

} // sync_reverb_config
/*****************************************************************************/
void dsp_control( // Controls audio stream processing for reverb application using dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O thread (bi-directional)
	streaming chanend c_dsp_eq, // Channel connecting to Equalisation thread (bi-directional)
	streaming chanend c_dsp_gain // Channel connecting to Loudness thread (bi-directional)
)
{
	S32_T inp_samps[NUM_REVERB_CHANS];	// Input audio sample buffer
	S32_T uneq_samps[NUM_REVERB_CHANS];	// Unequalised audio sample buffer
	S32_T equal_samps[NUM_REVERB_CHANS];	// Equalised  audio sample buffer
	S32_T unamp_samps[NUM_REVERB_CHANS];	// Unamplified audio sample buffer
	S32_T ampli_samps[NUM_REVERB_CHANS];	// Amplified audio sample buffer
	S32_T out_samps[NUM_REVERB_CHANS];	// Output audio sample buffer
	S32_T fade_samps[NUM_REVERB_CHANS];	// Cross-fade audio sample buffer

	S32_T samp_cnt = 0;	// Sample counter
	S32_T chan_cnt; // Channel counter

	PROC_STATE_TYP cur_proc_state	= EFFECT; // Initialise processing state to REVERB On.
	// Default Reverb parameters
	REVERB_PARAM_S def_param_s = {{ DEF_DRY_LVL ,DEF_FX_LVL ,DEF_ATTN_MIX ,DEF_CROSS_MIX } 
																,DEF_ROOM_SIZE ,DEF_SIG_FREQ ,DEF_SAMP_FREQ ,DEF_GAIN };


	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
	{
		inp_samps[chan_cnt] = 0;
		uneq_samps[chan_cnt] = 0;
		equal_samps[chan_cnt] = 0;
		unamp_samps[chan_cnt] = 0;
		ampli_samps[chan_cnt] = 0;
		out_samps[chan_cnt] = 0;
		fade_samps[chan_cnt] = 0;
	}	// for chan_cnt

	// Synchronise a reverb configuration (with other threads) using default parameters
	def_param_s.gain = 8; // Fine-tune Gain
	def_param_s.room_size = (74 / NUM_REVERB_CHANS); // Fine-tune Room-size
	sync_reverb_config( c_dsp_eq ,c_dsp_gain ,def_param_s );

	// Loop forever
	while(1)
	{
		// Send/Receive audio samples over sample thread channels
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_aud_dsp :> inp_samps[chan_cnt]; // Receive input samples from Audio I/O thread 
			c_aud_dsp <: out_samps[chan_cnt];  // Send Output samples back to Audio I/O thread 
		} // for chan_cnt

#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_eq <: uneq_samps[chan_cnt]; // Send Unequalised samples to Eq thread  
			c_dsp_eq :> equal_samps[chan_cnt]; // Receive Equalised samples back from Eq thread  
		} // for chan_cnt

#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_gain <: unamp_samps[chan_cnt]; // Send Unamplified samples to Loudness thread   
			c_dsp_gain :> ampli_samps[chan_cnt]; // Receive Amplified samples back from Loudness thread  

		} // for chan_cnt

		samp_cnt++; // Update sample counter

		// Check current processing State
		switch(cur_proc_state)
		{
			case EFFECT: // Do Effect processing
				use_reverb( uneq_samps ,unamp_samps ,out_samps ,inp_samps ,equal_samps ,ampli_samps );

				if (SWAP_NUM < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = FX2DRY; // Switch to Fade-out Effect
				} // if (SWAP_NUM < samp_cnt)
			break; // case EFFECT:

			case FX2DRY: // Fade-Out Effect
				use_reverb( uneq_samps ,unamp_samps ,fade_samps ,inp_samps ,equal_samps ,ampli_samps );

				cross_fade_sample( out_samps ,fade_samps ,inp_samps ,NUM_REVERB_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY_ONLY; // Switch to Dry-Only Processing
				} // if (SWAP_NUM < samp_cnt)
			break; // case FX2DRY:

			case DRY_ONLY: // No Effect (Dry signal only)
				for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
				{ // NB Add a bit of filtering to prevent clicks on transitions
					out_samps[chan_cnt] = inp_samps[chan_cnt];
				} // for chan_cnt

				if (SWAP_NUM < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY2FX; // Switch to Fade-In Effect

				} // if (SWAP_NUM < samp_cnt)
			break; // case DRY_ONLY:

			case DRY2FX: // Fade-in Effect
				use_reverb( uneq_samps ,unamp_samps ,fade_samps ,inp_samps ,equal_samps ,ampli_samps );

				cross_fade_sample( out_samps ,inp_samps ,fade_samps ,NUM_REVERB_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = EFFECT; // Switch to Effect Processing
				} // if (SWAP_NUM < samp_cnt)
			break; // case DRY2FX:

			default:
				assert(0 == 1); // ERROR: Unsupported state
			break; // default:
		} // switch(cur_proc_state)

	} // while(1)

} // dsp_control
/*****************************************************************************/
// dsp_control.xc
