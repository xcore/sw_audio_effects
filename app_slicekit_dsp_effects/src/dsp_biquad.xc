/******************************************************************************\
 * File:	dsp_biquad.xc
 *  
 * Description: Coar that applies non-linear gain to stream of audio samples
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

#include "dsp_biquad.h"

// DSP-control coar.

/******************************************************************************/
void receive_biquad_config( // Receives BiQuad filter configuration data
	S32_T biquad_id, // Identifies which BiQuad to use
	streaming chanend c_dsp_eq // Channel connecting to DSP-control coar (bi-directional)
)
{
	BIQUAD_PARAM_S biquad_param_s; // Structure to contain BiQuad filter Configuration data


	c_dsp_eq :> biquad_param_s; // Receive BiQuad filter configuration data
	
	config_biquad_filter( biquad_id ,biquad_param_s ); // Configure BiQuad filter parameters
} // receive_biquad_config
/******************************************************************************/
void process_biquad_token( // Receives Control Token from channel, and acts accordingly
	S32_T biquad_id, // Identifies which BiQuad to use
	streaming chanend c_dsp_eq // Channel connecting to DSP-control coar (bi-directional)
)
{
	U8_T inp_tok = sinct( c_dsp_eq ); // Receive Control Token from streaming channel


	// Determine Action required
	switch( inp_tok )
	{
		case CFG_BIQUAD: // Do BiQuad filter configuration
			receive_biquad_config( biquad_id ,c_dsp_eq );
		break; // case CFG_BIQUAD:

		default: // ERROR: Unsupported Control Token
			assert(0 == 1);
		break; // default:
	} // switch(cur_proc_state)

} // process_biquad_token
/******************************************************************************/
void process_biquad_audio( // apply biquad to audio stream
	streaming chanend c_dsp_eq, // Channel connecting to DSP-control coar (bi-directional)
	S32_T uneq_samps[],	// UnEqualised audio sample buffer
	S32_T equal_samps[],	// Equalised audio sample buffer
	S32_T biquad_id // Identifies which BiQuad to use
)
{
	S32_T chan_cnt; // Channel counter


#pragma loop unroll
	// Loop through set of channels and process audio I/O
	for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
	{
		c_dsp_eq :> uneq_samps[chan_cnt]; // Receive audio input sample
		c_dsp_eq <: equal_samps[chan_cnt]; // Send audio output sample
	} // for chan_cnt

	// Loop through set of channel samples and process audio
	for(chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
	{ // Apply BiQuad filter
		equal_samps[chan_cnt] = use_biquad_filter( biquad_id ,uneq_samps[chan_cnt] ,chan_cnt );
//	equal_samps[chan_cnt] = uneq_samps[chan_cnt]; // DBG
	} // for chan_cnt
} // process_biquad_audio
/******************************************************************************/
void dsp_biquad( // Coar that applies Equalisation (Tone control) to stream of audio samples
	streaming chanend c_dsp_eq, // Channel connecting to DSP-control coar (bi-directional)
	S32_T biquad_id // Identifies which BiQuad to use
)
{
 // Default biquad filter parameters
	BIQUAD_PARAM_S biquad_param_s = { DEF_FILT_MODE ,DEF_SAMP_FREQ ,DEF_SIG_FREQ ,DEF_QUAL_FACT };

	S32_T uneq_samps[NUM_REVERB_CHANS];	// UnEqualised audio sample buffer
	S32_T equal_samps[NUM_REVERB_CHANS];	// Equalised audio sample buffer
	S32_T chan_cnt; // Channel counter
	

	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
	{
		uneq_samps[chan_cnt] = 0;
		equal_samps[chan_cnt] = 0;
	}

	// Initialise with default configuration. NB This should be overwritten via control over c_dsp_eq channel
	config_biquad_filter( biquad_id ,biquad_param_s );

	// Loop forever
	while(1)
	{ 
		// Test input channel for control token
		if (stestct(c_dsp_eq))
		{ // Control Token
			process_biquad_token( biquad_id ,c_dsp_eq );
		} // if (stestct(c_dsp_eq))
		else
		{	// Audio Data
			process_biquad_audio( c_dsp_eq ,uneq_samps ,equal_samps ,biquad_id );
		} // else !(stestct(c_dsp_eq))
	} // while (1)

} // dsp_biquad
/*****************************************************************************/
// dsp_biquad.xc
