/******************************************************************************\
 * File:	dsp_loudness.xc
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for dsp_loudness.xc
 *
 * Version: 3v3rc1
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

// Gain-Control (Loudness) thread.

/******************************************************************************/
#pragma unsafe arrays
static void get_samples_from_control( // Get samples from Input channel, and process
	chanend c_inp, // audio-in channel
	int out_samps[], // Array of Output samples
	int num_iters // Number of Iterations of loudness algorithm
)
{
	int chan_cnt; // channel counter
	unsigned int samp_buf[NUM_USB_CHAN_OUT]; // Sample buffer


#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		int sample;

		samp_buf[chan_cnt] = inuint( c_inp ); // Receive sample from decouple
	} // for chan_cnt

	// timing fails if we do dsp in between receiving for both channels.
	// Instead receive into a local buffer, then do the dsp on both channels.

#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		out_samps[chan_cnt] = non_linear_gain_wrapper( samp_buf[chan_cnt] ,chan_cnt ,num_iters );

//	out_samps[chan_cnt] = samp_buf[chan_cnt]; // Dbg
	} // for chan_cnt
} // get_samples_from_control
/******************************************************************************/
void dsp_loudness( // Applies gain_control to audio stream using dsp functions
	chanend c_dsp_gain,  // Channel connects dsp and loudness threads (bi-directional)
	chanend c_gain_audio // Channel connects loudness and audio threads (bi-directional)
)
{
	int rec_samples[NUM_USB_CHAN_IN]; // array of samples to play for each input channel
	int play_samples[NUM_USB_CHAN_OUT]; // array of samples to record for each output channel

	int num_iters = 4; // Number of Iterations of loudness algorithm


	// zero sample buffers.
	safememset( (rec_samples ,unsigned char[]) ,0 ,(NUM_USB_CHAN_IN * sizeof( int )) ); 
	safememset( (play_samples ,unsigned char[]) ,0 ,(NUM_USB_CHAN_OUT * sizeof( int )) ); 

	while (1) 
	{
		inuint( c_gain_audio ); // Accept request for Amplified data from Audio thread (and discard)
	
		outuint( c_dsp_gain ,0 ); // Request Un-amplified data from DSP-control thread 

		// Test for Control-Code Signal
		if (testct( c_dsp_gain )) 
		{	// sample frequency change
			int samp_freq; // Current sample frequency


			inct( c_dsp_gain ); // Accept Control-Code Signal from DSP-control thread (and discard)
			samp_freq = inuint( c_dsp_gain ); // Accept Control-Code data (Sample Freq.)

			outct( c_gain_audio ,XS1_CT_END ); // Signal close-down Audio thread data stream
			outuint( c_gain_audio ,samp_freq ); // Send Control Data (Sample Frequency) to Audio thread

			chkct( c_gain_audio ,XS1_CT_END ); // Check next byte is XS1_CT_END signal from Audio threads (Receive and discard)
			outct( c_dsp_gain ,XS1_CT_END ); // Signal control-data-end to DSP-control thread
		} // if (testct( c_dsp_gain )) 
		else 
		{	// Normal audio loop.
			inuint( c_dsp_gain ); // Accept request for Recorded data from DSP-control thread (and discard)
			outuint( c_gain_audio ,0 ); // Request Recorded data from Audio thread
	
			give_samples_to_output( c_gain_audio ,play_samples ,NUM_USB_CHAN_OUT );

#if NUM_USB_CHAN_IN > 0
			// Process recorded samples on return channel
			get_samples_from_input( c_gain_audio ,rec_samples ,NUM_USB_CHAN_IN );
			give_samples_to_output( c_dsp_gain ,rec_samples ,NUM_USB_CHAN_IN );
#endif

			get_samples_from_control( c_dsp_gain ,play_samples ,num_iters );
		}; // else !(testct( c_dsp_gain )) 
	
	} // 	while (1) 
} // dsp_loudness
/******************************************************************************/
// dsp_loudness.xc
