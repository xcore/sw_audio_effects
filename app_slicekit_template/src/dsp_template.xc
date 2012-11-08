/******************************************************************************\
 * File:	dsp_template.xc
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

#include "dsp_template.h"

// DSP Effect coar.

/******************************************************************************/
void process_all_chans( // Do DSP effect processing
	S32_T out_samps[],	// Output Processed audio sample buffer
	S32_T inp_samps[],	// Input unprocessed audio sample buffer
	S32_T num_chans	// Number of input/output channels
)
{
	S32_T chan_cnt; // Channel counter

	for(chan_cnt = 0; chan_cnt < num_chans; chan_cnt++)
	{ // Apply non-linear gain shaping (Loudness)

		/* XXXXX     Enter your DSP-Effect Here    XXXX */

		/* XXXXX     and delete line below!        XXXX */
		out_samps[chan_cnt] = inp_samps[chan_cnt]; // Just copies input to output
	} // for chan_cnt

} // process_all_chans
/******************************************************************************/
void dsp_template( // Coar that applies non-linear gain control to stream of audio samples
	streaming chanend c_dsp_gain // Channel connecting to Audio_IO coar (bi-directional)
)
{
	// NB Setup correct number of channels in Makefile
	S32_T inp_samps[NUM_FX_CHANS];	// Unamplified input audio sample buffer
	S32_T amp_samps[NUM_FX_CHANS];	// Amplified audio sample buffer
	S32_T out_samps[NUM_FX_CHANS];	// Output audio sample buffer

	S32_T samp_cnt = 0;	// Sample counter
	S32_T chan_cnt; // Channel counter
	
	PROC_STATE_TYP cur_proc_state	= EFFECT; // Initialise processing state to EFFECT On.
// FX_PARAM_S def_param_s = { DEF_FX }; // Default gain parameters


	// initialise samples buffers ...

	for (chan_cnt = 0; chan_cnt < NUM_FX_CHANS; chan_cnt++)
	{
		inp_samps[chan_cnt] = 0;
		amp_samps[chan_cnt] = 0;
		out_samps[chan_cnt] = 0;
	}

//	config_template( def_param_s );	// Initial Effect configuration
	printstrln("Effect");

	// Loop forever
	while(1)
	{ 
		// Send/Receive samples over Audio coar channel
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_FX_CHANS; chan_cnt++)
		{
			c_dsp_gain :> inp_samps[chan_cnt]; 
			c_dsp_gain <: out_samps[chan_cnt]; 
		}

		samp_cnt++; // Update sample counter

		// Do DSP Processing ...
		process_all_chans( amp_samps ,inp_samps ,NUM_FX_CHANS );

		// Check current processing State
		switch(cur_proc_state)
		{
			case EFFECT: // Do DSP effect processing
				for (chan_cnt = 0; chan_cnt < NUM_FX_CHANS; chan_cnt++)
				{
					out_samps[chan_cnt] = amp_samps[chan_cnt];
				} // for chan_cnt

				if (SWAP_NUM < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = FX2DRY; // Switch to Fade-out Effect
				} // if (SWAP_NUM < samp_cnt)
			break; // case EFFECT:

			case FX2DRY: // Fade-Out Effect
				cross_fade_sample( out_samps ,amp_samps ,inp_samps ,NUM_FX_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY_ONLY; // Switch to Dry-Only Processing
					printstrln("Dry");
				} // if (SWAP_NUM < samp_cnt)
			break; // case FX2DRY:

			case DRY_ONLY: // No Effect (Dry signal only)
				for (chan_cnt = 0; chan_cnt < NUM_FX_CHANS; chan_cnt++)
				{
					out_samps[chan_cnt] = inp_samps[chan_cnt];
				} // for chan_cnt

				if (SWAP_NUM < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY2FX; // Switch to Fade-In Effect
				} // if (SWAP_NUM < samp_cnt)
			break; // case DRY_ONLY:

			case DRY2FX: // Fade-in Effect
				cross_fade_sample( out_samps ,inp_samps ,amp_samps ,NUM_FX_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = EFFECT; // Switch to Effect-Only Processing
					printstrln("Effect");
				} // if (SWAP_NUM < samp_cnt)
			break; // case DRY2FX:

			default:
				assert(0 == 1); // ERROR: Unsupported state
			break; // default:
		} // switch(cur_proc_state)

	} // while(1)

} // dsp_template
/*****************************************************************************/
// dsp_template.xc
