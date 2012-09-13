/******************************************************************************\
 * File:	dsp_biquad.xc
 * Author: Mark Beaumont
 * Description: Thread that applies non-linear gain to stream of audio samples
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

// DSP-control thread.

/******************************************************************************/
void receive_biquad_config( // Receives BiQuad filter configuration data
	streaming chanend c_dsp_eq // Channel connecting to DSP-control thread (bi-directional)
)
{
	BIQUAD_PARAM_S biquad_param_s; // Structure to contain BiQuad filter Configuration data


	c_dsp_eq :> biquad_param_s; // Receive BiQuad filter configuration data
	
	config_biquad_filter( biquad_param_s ); // Configure BiQuad filter parameters
} // receive_biquad_config
/******************************************************************************/
void process_biquad_token( // Receives Control Token from channel, and acts accordingly
	streaming chanend c_dsp_eq // Channel connecting to DSP-control thread (bi-directional)
)
{
	U8_T inp_tok = sinct( c_dsp_eq ); // Receive Control Token from streaming channel


	// Determine Action required
	switch( inp_tok )
	{
		case CFG_BIQUAD: // Do BiQuad filter configuration
			receive_biquad_config( c_dsp_eq );
		break; // case CFG_BIQUAD:

		default: // ERROR: Unsupported Control Token
			assert(0 == 1);
		break; // default:
	} // switch(cur_proc_state)

} // process_biquad_token
/******************************************************************************/
void process_biquad_audio( // apply biquad to audio stream
	streaming chanend c_dsp_eq, // Channel connecting to DSP-control thread (bi-directional)
	S32_T uneq_samps[],	// UnEqualised audio sample buffer
	S32_T equal_samps[],	// Equalised audio sample buffer
	S32_T num_chans				// Number of audio channels
)
{
	S32_T chan_cnt; // Channel counter


#pragma loop unroll
	// Loop through set of channels and process audio I/O
	for (chan_cnt = 0; chan_cnt < num_chans; chan_cnt++)
	{
		c_dsp_eq :> uneq_samps[chan_cnt]; // Receive audio input sample
		c_dsp_eq <: equal_samps[chan_cnt]; // Send audio output sample
	} // for chan_cnt

	// Loop through set of channel samples and process audio
	for(chan_cnt = 0; chan_cnt < num_chans; chan_cnt++)
	{ // Apply BiQuad filter
		equal_samps[chan_cnt] = use_biquad_filter( uneq_samps[chan_cnt] ,chan_cnt );
//			ampli_samps[chan_cnt] = unamp_samps[chan_cnt]; // DBG
	} // for chan_cnt
} // process_biquad_audio
/******************************************************************************/
void dsp_biquad( // Thread that applies Equalisation (Tone control) to stream of audio samples
	streaming chanend c_dsp_eq // Channel connecting to DSP-control thread (bi-directional)
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

	biquad_param_s.sig_freq = 4000; // Fine-tune Filter cut-off
	biquad_param_s.qual = 1; // Fine-tune Quality factor
	config_biquad_filter( biquad_param_s ); // Initial configuration with Default BiQuad filter parameters ...

	// Loop forever
	while(1)
	{ 
		// Test input channel for control token
		if (stestct(c_dsp_eq))
		{ // Control Token
			process_biquad_token( c_dsp_eq );
		} // if (stestct(c_dsp_eq))
		else
		{	// Audio Data
			process_biquad_audio( c_dsp_eq ,uneq_samps ,equal_samps ,NUM_REVERB_CHANS );
		} // else !(stestct(c_dsp_eq))
	} // while (1)

} // dsp_biquad
/*****************************************************************************/
// dsp_biquad.xc
