/******************************************************************************\
 * Description: Core that applies a number of bands of vocoder processing
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

#include "dsp_channel_vocoder.h"

// channel-Vocoder core

/******************************************************************************/
void receive_channel_vocoder_config( // Receives vocoder configuration data
	streaming chanend c_dsp_vocoder, // Channel connecting to DSP-control core (bi-directional)
	S32_T slave_id // Unique Vocoder slave identifier
)
{
	VOCODER_PARAM_S vocoder_param_s; // Structure to contain vocoder Configuration data


	c_dsp_vocoder :> vocoder_param_s; // Receive vocoder configuration data

	config_vocoder( vocoder_param_s ,slave_id ); // Configure vocoder parameters
} // receive_channel_vocoder_config
/******************************************************************************/
void process_channel_vocoder_token( // Receives Control Token from channel, and acts accordingly
	streaming chanend c_dsp_vocoder, // Channel connecting to DSP-control coar (bi-directional)
	S32_T slave_id // Unique Vocoder slave identifier
)
{
	U8_T inp_tok = sinct( c_dsp_vocoder ); // Receive Control Token from streaming channel


	// Determine Action required
	switch( inp_tok )
	{
		case CFG_VOCODER: // Do vocoder configuration
			receive_channel_vocoder_config( c_dsp_vocoder ,slave_id );
		break; // case CFG_VOCODER:

		default: // ERROR: Unsupported Control Token
			assert(0 == 1);
		break; // default:
	} // switch(cur_proc_state)

} // process_channel_vocoder_token
/******************************************************************************/
void dsp_channel_vocoder( // Coar that applies Equalisation (Tone control) to stream of audio samples
	streaming chanend c_dsp_vocoder, // Channel connecting to DSP-control coar (bi-directional)
	S32_T slave_id // Unique Slave Identifier
)
{
	VOCODER_PARAM_S vocoder_param_s = { DEF_SAMP_FREQ ,DEF_QUAL_FACT }; // Default vocoder parameters

	SAMP_CHAN_T inp_samps[NUM_VOCODER_OUT_CHANS];	// Un-processed input-sample buffer
	SAMP_CHAN_T out_samps[NUM_VOCODER_OUT_CHANS];	// Processed output-sample buffer
	S32_T chan_cnt; // Channel counter


	// Initialise with default configuration. NB This should be overwritten via control over c_dsp_vocoder channel
	config_vocoder( vocoder_param_s ,slave_id );

	// Loop forever
	while(1)
	{
		// Test input channel for control token
		if (stestct(c_dsp_vocoder))
		{ // Control Token
			process_channel_vocoder_token( c_dsp_vocoder ,slave_id );
		} // if (stestct(c_dsp_vocoder))
		else
		{	// Audio Data
#pragma loop unroll
			// Loop through set of channels and process audio I/O
			for (chan_cnt = 0; chan_cnt < NUM_VOCODER_OUT_CHANS; chan_cnt++)
			{
				c_dsp_vocoder :> inp_samps[chan_cnt]; // Receive audio input sample
				c_dsp_vocoder <: out_samps[chan_cnt]; // Send audio output sample
			} // for chan_cnt

			use_vocoder( out_samps ,inp_samps ,slave_id );
		} // else !(stestct(c_dsp_vocoder))
	} // while (1)

} // dsp_channel_vocoder
/*****************************************************************************/
// dsp_channel_vocoder.xc
