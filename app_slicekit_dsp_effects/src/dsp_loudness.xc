/******************************************************************************\
 * File:	dsp_loudness.xc
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

#include "dsp_loudness.h"

// DSP-control coar.

/******************************************************************************/
void receive_loudness_config( // Receives Loudness configuration data
	streaming chanend c_dsp_gain // Channel connecting to DSP-control coar (bi-directional)
)
{
	GAIN_PARAM_S gain_param_s; // Structure to contain Loudness Configuration data


	c_dsp_gain :> gain_param_s; // Receive Loudness configuration data

	config_loudness( gain_param_s ); // Configure gain-shaping parameters
} // receive_loudness_config
/******************************************************************************/
void process_loudness_token( // Receives Control Token from channel, and acts accordingly
	streaming chanend c_dsp_gain // Channel connecting to DSP-control coar (bi-directional)
)
{
	U8_T inp_tok = sinct( c_dsp_gain ); // Receive Control Token from streaming channel


	// Determine Action required
	switch( inp_tok )
	{
		case CFG_GAIN: // Do gain-shaping configuration
			receive_loudness_config( c_dsp_gain );
		break; // case CFG_GAIN:

		default: // ERROR: Unsupported Control Token
			assert(0 == 1);
		break; // default:
	} // switch(cur_proc_state)

} // process_loudness_token
/******************************************************************************/
void process_loudness_audio( // apply loudness to audio stream
	streaming chanend c_dsp_gain, // Channel connecting to DSP-control coar (bi-directional)
	S32_T unamp_samps[],	// Unamplified audio sample buffer
	S32_T ampli_samps[] 	// Amplified audio sample buffer
)
{
	S32_T chan_cnt; // Channel counter


#pragma loop unroll
	// Loop through set of channels and process audio I/O
	for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
	{
		c_dsp_gain :> unamp_samps[chan_cnt]; // Receive audio input sample
		c_dsp_gain <: ampli_samps[chan_cnt]; // Send audio output sample
	} // for chan_cnt

	// Loop through set of channel samples and process sudio
	for(chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
	{ // Apply non-linear gain shaping (Loudness)
		ampli_samps[chan_cnt] = use_loudness( unamp_samps[chan_cnt] );
//	ampli_samps[chan_cnt] = unamp_samps[chan_cnt]; // DBG
	} // for chan_cnt

} // process_loudness_audio
/******************************************************************************/
void dsp_loudness( // Coar that applies non-linear gain control to stream of audio samples
	streaming chanend c_dsp_gain // Channel connecting to DSP-control coar (bi-directional)
)
{
	GAIN_PARAM_S def_param_s = { DEF_GAIN }; // Structure containing default gain parameters

	S32_T unamp_samps[NUM_REVERB_CHANS];	// Unamplified audio sample buffer
	S32_T ampli_samps[NUM_REVERB_CHANS];	// Amplified audio sample buffer

	S32_T chan_cnt; // Channel counter


	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
	{
		unamp_samps[chan_cnt] = 0;
		ampli_samps[chan_cnt] = 0;
	}

	config_loudness( def_param_s ); // Initial configuration with Default gain-shaping parameters

	// Loop forever
	while(1)
	{
		// Test input channel for control token
		if (stestct(c_dsp_gain))
		{ // Control Token
			process_loudness_token( c_dsp_gain );
		} // if (stestct(c_dsp_gain))
		else
		{	// Audio Data
			process_loudness_audio( c_dsp_gain ,unamp_samps ,ampli_samps );
		} // else !(stestct(c_dsp_gain))
	} // while (1)

} // dsp_loudness
/*****************************************************************************/
// dsp_loudness.xc
