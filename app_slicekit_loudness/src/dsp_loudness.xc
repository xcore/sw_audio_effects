/******************************************************************************\
 * File:	dsp_loudness.xc
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

#include "dsp_loudness.h"

// DSP-control thread.

/******************************************************************************/
void process_all_chans( // Do DSP effect processing
	S32_T out_samps[I2S_CHANS_DAC],	// Output Processed audio sample buffer
	S32_T inp_samps[I2S_CHANS_ADC],	// Input unprocessed audio sample buffer
	S32_T min_chans	// Minimum of input/output channels
)
{
	S32_T chan_cnt; // Channel counter
	

	for(chan_cnt = 0; chan_cnt < min_chans; chan_cnt++)
	{ // Apply non-linear gain shaping (Loudness)
		out_samps[chan_cnt] = use_loudness( inp_samps[chan_cnt] ,chan_cnt );
//			out_samps[chan_cnt] = inp_samps[chan_cnt]; // DBG
	} // for chan_cnt

} // process_all_chans
/******************************************************************************/
void dsp_loudness( // Thread that applies non-linear gain control to stream of audio samples
	streaming chanend c_dsp_gain // Channel connecting to Audio_IO thread (bi-directional)
)
{
	S32_T inp_samps[I2S_CHANS_ADC];	// Unamplified input audio sample buffer
	S32_T amp_samps[I2S_CHANS_DAC];	// Amplified audio sample buffer
	S32_T out_samps[I2S_CHANS_DAC];	// Output audio sample buffer

	S32_T samp_cnt = 0;	// Sample counter
	S32_T min_chans = I2S_CHANS_ADC;	// Preset minimum of input/output channels to No. of Input channels
	S32_T chan_cnt; // Channel counter
	
	PROC_STATE_TYP cur_proc_state	= EFFECT; // Initialise processing state to EFFECT On.
	GAIN_PARAM_S def_param_s = { DEF_GAIN }; // Default gain parameters


	// if necessary, update minimum number of channels
	if (min_chans > I2S_CHANS_DAC)
	{
		min_chans = I2S_CHANS_DAC;
	} // if (min_chans > I2S_CHANS_DAC)

	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < min_chans; chan_cnt++)
	{
		inp_samps[chan_cnt] = 0;
		amp_samps[chan_cnt] = 0;
		out_samps[chan_cnt] = 0;
	}

	config_loudness( def_param_s );	// Initial Gain-shaping configuration

	// Loop forever
	while(1)
	{ 
		// Send/Receive samples over Audio thread channel
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < min_chans; chan_cnt++)
		{
			c_dsp_gain :> inp_samps[chan_cnt]; 
			c_dsp_gain <: out_samps[chan_cnt]; 
		}

		samp_cnt++; // Update sample counter

		// Do DSP Processing ...

		// Check current processing State
		switch(cur_proc_state)
		{
			case EFFECT: // Do DSP effect processing
				process_all_chans( out_samps ,inp_samps ,min_chans );

				if (SWAP_NUM < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = FX2DRY; // Switch to Fade-out Effect
				} // if (SWAP_NUM < samp_cnt)
			break; // case EFFECT:

			case FX2DRY: // Fade-Out Effect
				process_all_chans( amp_samps ,inp_samps ,min_chans );

				cross_fade_sample( out_samps ,amp_samps ,inp_samps ,min_chans ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY_ONLY; // Switch to Dry-Only Processing
				} // if (SWAP_NUM < samp_cnt)
			break; // case FX2DRY:

			case DRY_ONLY: // No Effect (Dry signal only)
				for (chan_cnt = 0; chan_cnt < min_chans; chan_cnt++)
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
				process_all_chans( amp_samps ,inp_samps ,min_chans );

				cross_fade_sample( out_samps ,inp_samps ,amp_samps ,min_chans ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = EFFECT; // Switch to Effect-Only Processing
				} // if (SWAP_NUM < samp_cnt)
			break; // case DRY2FX:

			default:
				assert(0 == 1); // ERROR: Unsupported state
			break; // default:
		} // switch(cur_proc_state)
	} // while(1)

} // dsp_loudness
/*****************************************************************************/
// dsp_loudness.xc
