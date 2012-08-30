/******************************************************************************\
 * File:	dsp_equalisation.xc
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for dsp_equalisation.xc
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

#include "dsp_equalisation.h"

/******************************************************************************/
#pragma unsafe arrays
static void get_samples_from_control( // Get samples from DSP-control thread, and process
	chanend c_inp, // audio-in channel
	int samples[], // Array of Output samples
	int samp_freq // Current sample frequency
)
{
	int chan_cnt; // channel counter
	unsigned int samp_buf[NUM_USB_CHAN_OUT]; // Sample buffer


#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		int sample;

		samp_buf[chan_cnt] = inuint( c_inp ); // Receive sample from Input (DSP-control)
	} // for chan_cnt

	// timing fails if we do dsp in between receiving for both channels.
	// Instead receive into a local buffer, then do the dsp on both channels.

#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		samples[chan_cnt] = biquad_filter_wrapper( samp_buf[chan_cnt] ,chan_cnt ,samp_freq );

//	samples[chan_cnt] = samp_buf[chan_cnt]; // Dbg
	} // for chan_cnt
} // get_samples_from_control
/******************************************************************************/
void dsp_equalisation( // Applies gain_control to audio stream using dsp functions
	chanend c_dsp_eq // Channel connects DSP and Equalisation threads (bi-directional)
)
{
	int play_samples[NUM_USB_CHAN_OUT]; // array of samples to play for each output channel
	int samp_freq = DEFAULT_FREQ; // Current sample frequency


	// zero sample buffers.
	safememset( (play_samples ,unsigned char[]) ,0 ,(NUM_USB_CHAN_OUT * sizeof( int )) ); 

	while (1) 
	{
		outuint( c_dsp_eq ,0 ); // Request Unequalised data from DSP-control thread 

		// Test for Control-Code Signal
		if (testct( c_dsp_eq )) 
		{ // Sample Frequency
			inct( c_dsp_eq ); // Accept Control-Code Signal from DSP-control  thread (and discard)

			samp_freq = inuint( c_dsp_eq );  // Accept Control-Code data (Sample Frequency)

			outct( c_dsp_eq ,XS1_CT_END ); // Signal control-data-end to DSP-control thread 
		} // if (testct( c_dsp_eq )) 
		else 
		{	// Normal audio loop.
			inuint( c_dsp_eq ); // Accept request for Equalised data from DSP-control thread (and discard)

			give_samples_to_output( c_dsp_eq ,play_samples ,NUM_USB_CHAN_OUT );
			get_samples_from_control( c_dsp_eq ,play_samples ,samp_freq );
		}; // else !(testct( c_dsp_eq )) 
	
	} // 	while (1) 
} // dsp_equalisation
/******************************************************************************/
// dsp_equalisation.xc
