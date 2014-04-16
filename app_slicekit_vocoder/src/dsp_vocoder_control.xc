/******************************************************************************\
 * Description: Control for Vocoder
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

#include "dsp_vocoder_control.h"

// DSP-control core.

/*****************************************************************************/
void send_vocoder_config( // Send Vocoder configuration data
	streaming chanend c_dsp_vocoder, // Channel connecting to channel-vocoder core (bi-directional)
	VOCODER_PARAM_S &vocoder_param_s // Reference to structure containing vocoder parameters
)
{
	soutct( c_dsp_vocoder ,CFG_VOCODER ); // Signal Vocoder configuration data transmission

	c_dsp_vocoder <: vocoder_param_s; // Send Vocoder configuration data
} // send_vocoder_config
/******************************************************************************/
void mixdown_all_chans( // Mixdown all vocoder output channels to stereo pair
	SAMP_CHAN_T out_samps[],	// Output Processed audio sample buffer
	SAMP_CHAN_T inp_samps[NUM_VOCODER_SLAVES][NUM_VOCODER_OUT_CHANS],	// Array of Vocoder channel outputs
	S32_T num_chans	// Number of input/output channels
)
{
	S32_T slave_cnt; // Slave counter
	S32_T chan_cnt; // Channel counter


	// Initialise stereo output with 1st vocoder channel
	for(chan_cnt = 0; chan_cnt < num_chans; chan_cnt++)
	{
		out_samps[chan_cnt] = (inp_samps[0][chan_cnt] >> 2); //MB~
	} // for chan_cnt

	// Add-in rest of vocoder channels
	for (slave_cnt = 1; slave_cnt < NUM_VOCODER_SLAVES; slave_cnt++)
	{
		for(chan_cnt = 0; chan_cnt < num_chans; chan_cnt++)
		{
			out_samps[chan_cnt] += (inp_samps[slave_cnt][chan_cnt] >> 2); //MB~
		} // for chan_cnt
	} // for slave_cnt

} // mixdown_all_chans
/*****************************************************************************/
void dsp_vocoder_control( // Controls audio stream processing for reverb application using dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O core (bi-directional)
	streaming chanend c_dsp_slaves[] // Array of channels between DSP master and vocoder slaves
)
{
	VOCODER_PARAM_S vocoder_param_s = { SAMP_FREQ }; // Structure containing vocoder parameters
	SAMP_CHAN_T inp_samps[NUM_VOCODER_INP_CHANS];	// Unprocessed input audio sample buffer
	SAMP_CHAN_T out_samps[NUM_VOCODER_INP_CHANS]; // Output audio sample buffer
	SAMP_CHAN_T vocoder_out_samps[NUM_VOCODER_SLAVES][NUM_VOCODER_OUT_CHANS];	// Array of Vocoder channel outputs
	SAMP_CHAN_T vocoder_cm_samps[NUM_VOCODER_OUT_CHANS];	// Vocoder inputs (Carrier & Modulator)
	SAMP_CHAN_T vocoder_mix_samps[NUM_VOCODER_OUT_CHANS];	// Stereo-mix of Vocoder processed audio
	SAMP_CHAN_T zero_samps[NUM_VOCODER_OUT_CHANS];	// zero-valued audio sample buffer
	S32_T samp_cnt = 0;	// Sample counter
	S32_T chan_cnt; // Channel counter
	S32_T slave_cnt; // Vocoder slave counter
	S32_T pair_cnt; // Channel pair counter
	PROC_STATE_ENUM cur_proc_state	= DRY2FX; // Initialise processing state to Fade-in EFFECT.


	acquire_lock();
	printstrln("Vocoder Starts...");
	release_lock();

	// initialise samples buffers ...

	for (chan_cnt = 0; chan_cnt < NUM_VOCODER_INP_CHANS; chan_cnt++)
	{
		inp_samps[chan_cnt] = 0;
		out_samps[chan_cnt] = 0;
	} // for chan_cnt

	for (chan_cnt = 0; chan_cnt < NUM_VOCODER_OUT_CHANS; chan_cnt++)
	{
		for (slave_cnt = 0; slave_cnt < NUM_VOCODER_SLAVES; slave_cnt++)
		{
			vocoder_out_samps[slave_cnt][chan_cnt] = 0;
		} // for slave_cnt

		zero_samps[chan_cnt] = 0;
		vocoder_cm_samps[chan_cnt] = 0;
		vocoder_mix_samps[chan_cnt] = 0;
	} // for chan_cnt

	// Loop through all vocoder slaves
	for (slave_cnt = 0; slave_cnt < NUM_VOCODER_SLAVES; slave_cnt++)
	{
		send_vocoder_config( c_dsp_slaves[slave_cnt] ,vocoder_param_s );
	} // for slave_cnt

	// Fade-in input to prevent clicks
	while(samp_cnt < FADE_LEN)
	{
		// Send/Receive samples over Audio core channel
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_VOCODER_INP_CHANS; chan_cnt++)
		{
			c_aud_dsp :> inp_samps[chan_cnt];
			c_aud_dsp <: out_samps[chan_cnt]; // NB channels 2 & 3 are empty
		}

		// Create mono signals
		for (pair_cnt = 0; pair_cnt < NUM_VOCODER_OUT_CHANS; pair_cnt++)
		{
			chan_cnt = 2 * pair_cnt;
			vocoder_cm_samps[pair_cnt] = (inp_samps[chan_cnt] + inp_samps[chan_cnt+1]) >> 1;
		} // for pair_cnt

		// Create output sample-set by cross-fading from zero-valued sample-set to input sample-set
		cross_fade_sample( out_samps ,zero_samps ,vocoder_cm_samps ,NUM_VOCODER_OUT_CHANS ,samp_cnt );

		//NB Out-samples now contains faded input-samples

		// Send/Receive samples over dsp channel
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_VOCODER_OUT_CHANS; chan_cnt++)
		{
#pragma loop unroll
			for (slave_cnt = 0; slave_cnt < NUM_VOCODER_SLAVES; slave_cnt++)
			{
				c_dsp_slaves[slave_cnt] <: out_samps[chan_cnt]; // Send faded input-samples
				c_dsp_slaves[slave_cnt] :> vocoder_out_samps[slave_cnt][chan_cnt]; // Get vocoder channel output
			} // for slave_cnt
		} // for chan_cnt

		samp_cnt++; // Update sample counter
	} // while(1)

	samp_cnt = 0; // Reset sample counter

	// Loop forever
	while(1)
	{
		// Send/Receive samples over Audio core channel
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_VOCODER_INP_CHANS; chan_cnt++)
		{
			c_aud_dsp :> inp_samps[chan_cnt];
			c_aud_dsp <: out_samps[chan_cnt];
		}

		// Create mono signals
		for (pair_cnt = 0; pair_cnt < NUM_VOCODER_OUT_CHANS; pair_cnt++)
		{
			chan_cnt = 2 * pair_cnt;
			vocoder_cm_samps[pair_cnt] = (inp_samps[chan_cnt] + inp_samps[chan_cnt+1]) >> 1;
		} // for pair_cnt

		// Overwrite right-channel Carrier with left-channel Modulator

		// Send/Receive samples over dsp channel
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_VOCODER_OUT_CHANS; chan_cnt++)
		{
#pragma loop unroll
			for (slave_cnt = 0; slave_cnt < NUM_VOCODER_SLAVES; slave_cnt++)
			{
				c_dsp_slaves[slave_cnt] <: vocoder_cm_samps[chan_cnt]; // Send input channel
				c_dsp_slaves[slave_cnt] :> vocoder_out_samps[slave_cnt][chan_cnt]; // Get vocoder channel output
			} // for slave_cnt
		} // for chan_cnt

		samp_cnt++; // Update sample counter

		// Do DSP Processing ...
		mixdown_all_chans( vocoder_mix_samps ,vocoder_out_samps ,NUM_VOCODER_OUT_CHANS );

		// Check current processing State
		switch(cur_proc_state)
		{
			case EFFECT: // Full Effect: Copy processed samples to output
				for (chan_cnt = 0; chan_cnt < NUM_VOCODER_OUT_CHANS; chan_cnt++)
				{
					out_samps[chan_cnt] = vocoder_mix_samps[chan_cnt];
				} // for chan_cnt

				if (SWAP_NUM < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = FX2DRY; // Switch to Fade-out Effect
				} // if (SWAP_NUM < samp_cnt)
			break; // case EFFECT:

			case FX2DRY: // Fade-Out Effect
				cross_fade_sample( out_samps ,vocoder_mix_samps ,inp_samps ,NUM_VOCODER_OUT_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY_ONLY; // Switch to Dry-Only Processing
acquire_lock(); printcharln('D');	release_lock(); //MB~
				} // if (SWAP_NUM < samp_cnt)
			break; // case FX2DRY:

			case DRY_ONLY: // No Effect: Copy un-processed input samples to output
				for (chan_cnt = 0; chan_cnt < NUM_VOCODER_OUT_CHANS; chan_cnt++)
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
				cross_fade_sample( out_samps ,inp_samps ,vocoder_mix_samps ,NUM_VOCODER_OUT_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = EFFECT; // Switch to Effect-Only Processing
acquire_lock(); printcharln('E');	release_lock(); //MB~
				} // if (SWAP_NUM < samp_cnt)
			break; // case DRY2FX:

			default:
				assert(0 == 1); // ERROR: Unsupported state
			break; // default:
		} // switch(cur_proc_state)
	} // while(1)

} // dsp_vocoder_control
/*****************************************************************************/
// dsp_vocoder_control.xc
