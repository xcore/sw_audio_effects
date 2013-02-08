/******************************************************************************\
 * File:	dsp_delay.xc
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

#include "dsp_delay.h"

// DSP-control coar.

/******************************************************************************/
void process_all_chans( // Do DSP effect processing
	S32_T out_samps[],	// Output Processed audio sample buffer
	DELAY_PARAM_S &cur_param_s, // Reference to structure containing delay-line parameters
	S32_T inp_samps[],	// Input unprocessed audio sample buffer
	S32_T min_chans	// Minimum of input/output channels
)
{
	static delay_samps[NUM_DELAY_TAPS]; // Set of delayed output samples
	S32_T chan_cnt; // Channel counter
	S32_T tap_cnt; // Channel counter

	// Loop through all channels
	for(chan_cnt = 0; chan_cnt < min_chans; chan_cnt++)
	{ // Create multiple echo effect

		// Mix input sample with most delayed sample to create some feedback
		inp_samps[chan_cnt] = (inp_samps[chan_cnt] + delay_samps[cur_param_s.num - 1] + 1) >> 1;

		// Get next set of delayed output samples, and delay current input sample
		use_delay_line( delay_samps ,inp_samps[chan_cnt] ,chan_cnt );

		// Mix all delayed samples to produce one output sample ...

		out_samps[chan_cnt] = delay_samps[0]; // Initialise Output with first delayed sample

		// Loop through rest of delayed samples
		for(tap_cnt = 1; tap_cnt < cur_param_s.num; tap_cnt++)
		{
			out_samps[chan_cnt] += delay_samps[tap_cnt]; // Update output sample
		} // for tap_cnt

//	out_samps[chan_cnt] = inp_samps[chan_cnt]; // DBG
	} // for chan_cnt
	
} // process_all_chans
/******************************************************************************/
void dsp_delay( // Coar that delays a stream of audio samples
	streaming chanend c_dsp_gain // Channel connecting to Audio_IO coar (bi-directional)
)
{
	S32_T inp_samps[NUM_DELAY_CHANS];	// Unamplified input audio sample buffer
	S32_T delay_samps[NUM_DELAY_CHANS];	// Delayed audio sample buffer
	S32_T out_samps[NUM_DELAY_CHANS];	// Output audio sample buffer

	S32_T tap_cnt; // tap counter
	S32_T samp_cnt = 0;	// Sample counter
	S32_T chan_cnt; // Channel counter
	
	S32_T delay_us; // Delay in micro-secs
	PROC_STATE_ENUM cur_proc_state	= EFFECT; // Initialise processing state to EFFECT On.
	DELAY_PARAM_S delay_param_s; // Default Delay-line Configuration


	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < NUM_DELAY_CHANS; chan_cnt++)
	{
		inp_samps[chan_cnt] = 0;
		delay_samps[chan_cnt] = 0;
		out_samps[chan_cnt] = 0;
	}

	// Configure Delay-line parameters ...

	delay_param_s.num = NUM_DELAY_TAPS;
	delay_param_s.freq = DEF_SAMP_FREQ;
	delay_us = DELAY_SIZE * (1000000 / delay_param_s.freq); // Approx largest delay in micro-secs

	// Initialise Configuration parameter structure
	for (tap_cnt = (NUM_DELAY_TAPS - 1); tap_cnt>=0; tap_cnt--)
	{
		delay_param_s.us_delays[tap_cnt] = delay_us;
		delay_us >>= 1; // Double delay
	} //for tap_cnt

	config_delay_line( delay_param_s );

	// Loop forever
	while(1)
	{ 
		// Send/Receive samples over Audio coar channel
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_DELAY_CHANS; chan_cnt++)
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
				process_all_chans( out_samps ,delay_param_s ,inp_samps ,NUM_DELAY_CHANS );

				if (SWAP_NUM < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = FX2DRY; // Switch to Fade-out Effect
				} // if (SWAP_NUM < samp_cnt)
			break; // case EFFECT:

			case FX2DRY: // Fade-Out Effect
				process_all_chans( delay_samps ,delay_param_s ,inp_samps ,NUM_DELAY_CHANS );

				cross_fade_sample( out_samps ,delay_samps ,inp_samps ,NUM_DELAY_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY_ONLY; // Switch to Dry-Only Processing
				} // if (SWAP_NUM < samp_cnt)
			break; // case FX2DRY:

			case DRY_ONLY: // No Effect (Dry signal only)
				for (chan_cnt = 0; chan_cnt < NUM_DELAY_CHANS; chan_cnt++)
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
				process_all_chans( delay_samps ,delay_param_s ,inp_samps ,NUM_DELAY_CHANS );

				cross_fade_sample( out_samps ,inp_samps ,delay_samps ,NUM_DELAY_CHANS ,samp_cnt );

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

} // dsp_delay
/*****************************************************************************/
// dsp_delay.xc
