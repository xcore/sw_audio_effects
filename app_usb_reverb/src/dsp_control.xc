/******************************************************************************\
 * File:	dsp_control.xc
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for dsp_control.xc
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

#include "dsp_control.h"

// DSP-Control thread.

/******************************************************************************/
#pragma unsafe arrays
static void get_samples_from_host( // Get Input samples from Decouple thread. and process
	chanend c_inp, // audio-in channel
	int samples[], // Array of Output samples
	int samp_freq, // Current Sample Frequency
	int delay_ms // Sample delay in milli-secs
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
		samples[chan_cnt] = delay_line_wrapper( samp_buf[chan_cnt] ,chan_cnt ,samp_freq ,delay_ms );

//	samples[chan_cnt] = samp_buf[chan_cnt]; // Dbg
	} // for chan_cnt
} // get_samples_from_host
/******************************************************************************/
#pragma unsafe arrays
static void get_samples_from_eq( // Get Equalised samples from Eq thread, and process
	chanend c_inp, // audio-in channel
	int samples[] // Array of Output samples
)
{
	int chan_cnt; // channel counter
	unsigned int samp_buf[NUM_USB_CHAN_OUT]; // Sample buffer


#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		int sample;

		samp_buf[chan_cnt] = inuint( c_inp ); // Receive sample from DSP-eq
	} // for chan_cnt

	// timing fails if we do dsp in between receiving for both channels.
	// Instead receive into a local buffer, then do the dsp on both channels.

#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
//		samples[chan_cnt] = delay_line_wrapper( samp_buf[chan_cnt] ,chan_cnt ,samp_freq ,delay_ms );

	samples[chan_cnt] = samp_buf[chan_cnt]; // Dbg
	} // for chan_cnt
} // get_samples_from_eq
/******************************************************************************/
void dsp_control( // Controls audio stream processing for reverb application using dsp functions
	chanend c_decouple_dsp, // Channel connects Decoupler and DSP threads (bi-directional)
	chanend c_dsp_eq, // Channel connects DSP and Equalisation threads (bi-directional)
	chanend c_dsp_gain // Channel connects DSP and Loudness threads (bi-directional)
)
{
	int rec_samples[NUM_USB_CHAN_IN]; // array of samples to record from each device input channel
	int eq_samples[NUM_USB_CHAN_OUT]; // array of samples for each eq output channel
	int gain_samples[NUM_USB_CHAN_OUT]; // array of samples for each gain output channel

	int samp_freq = DEFAULT_FREQ; // Current sample frequency
	int delay_ms = 58; // Delay time in milli-seconds


	// zero sample buffers.
	safememset( (rec_samples ,unsigned char[]) ,0 ,(NUM_USB_CHAN_IN * sizeof( int )) ); 
	safememset( (eq_samples ,unsigned char[]) ,0 ,(NUM_USB_CHAN_OUT * sizeof( int )) ); 
	safememset( (gain_samples ,unsigned char[]) ,0 ,(NUM_USB_CHAN_OUT * sizeof( int )) ); 

	while (1) 
	{
		inuint( c_dsp_gain ); // Accept request for Reverb data from Gain thread (and discard)
		inuint( c_dsp_eq ); // Accept request for Unequalised data from Eq thread (and discard)
	
		outuint( c_decouple_dsp ,0 ); // Request Dry data from decouple thread 

		// Test for Control-Code Signal
		if (testct( c_decouple_dsp )) 
		{	// sample frequency change
			inct( c_decouple_dsp ); // Accept Control-Code Signal from Decouple thread (and discard)
			samp_freq = inuint( c_decouple_dsp ); // Accept Control-Code data

			// Signal close-down Gain & Eq thread data streams
			outct( c_dsp_gain ,XS1_CT_END );
			outct( c_dsp_eq ,XS1_CT_END );

			// Send Control Data (Sample Frequency) to Gain & Eq threads
			outuint( c_dsp_gain ,samp_freq );
			outuint( c_dsp_eq ,samp_freq );

			// Check next byte is XS1_CT_END signal from Gain & Eq threads (Receive and discard)
			chkct( c_dsp_gain ,XS1_CT_END );
			chkct( c_dsp_eq ,XS1_CT_END );

			outct( c_decouple_dsp ,XS1_CT_END ); // Signal control-data-end to Decouple thread 
		} // if (testct( c_decouple_dsp )) 
		else
		{	// Normal audio loop.
			inuint( c_decouple_dsp ); // Accept request for Dry data from Decouple thread (and discard)
	
			outuint( c_dsp_gain ,0 ); // Request recorded data from Gain thread
			outuint( c_dsp_eq ,0 ); // Request Equalised data from Eq thread
	
			give_samples_to_output( c_dsp_gain ,gain_samples ,NUM_USB_CHAN_OUT );
			get_samples_from_eq( c_dsp_eq ,gain_samples );

#if NUM_USB_CHAN_IN > 0
			// Process recorded samples on return channel
			get_samples_from_input( c_dsp_gain ,rec_samples ,NUM_USB_CHAN_IN );
			give_samples_to_output( c_decouple_dsp ,rec_samples ,NUM_USB_CHAN_IN );
#endif

			give_samples_to_output( c_dsp_eq ,eq_samples ,NUM_USB_CHAN_OUT );
			get_samples_from_host( c_decouple_dsp ,eq_samples ,samp_freq ,delay_ms );
		}; // else !(testct( c_decouple_dsp )) 
	
	} // 	while (1) 
} // dsp_control
/******************************************************************************/
// dsp_control.xc
