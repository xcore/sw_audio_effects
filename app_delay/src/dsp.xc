/******************************************************************************\
 * File:	dsp.xc
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for dsp.xc
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

#include "dsp.h"

// DSP thread.

#include <xs1.h>
#include <safestring.h>
#include "devicedefines.h"
#include <print.h>
#include "delay_line.h"

/******************************************************************************/
//give/get functions based on mixer.xc then simplified.

#pragma unsafe arrays
void give_samples_to_host(
	chanend c_out, // audio-out channel
	const int samples[] // samples
)
{
	int chan_cnt; // channel counter


#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_IN; chan_cnt++)
	{
		int sample;

		sample = samples[chan_cnt];

		outuint( c_out ,sample ); // Transmit sample to De-coupler
	} // for chan_cnt
} // give_samples_to_host
/******************************************************************************/

#pragma unsafe arrays
static void get_samples_from_host(
	chanend c_inp, // audio-in channel
	int samples[],
	int samp_freq, // Current Sample Frequency
	int delay_ms // Sample delay in milli-secs
)
{
	int chan_cnt; // channel counter
	unsigned int l_samp[NUM_USB_CHAN_OUT];


#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		int sample;

		l_samp[chan_cnt] = inuint( c_inp ); // Receive sample from decouple
	} // for chan_cnt

	// timing fails if we do dsp in between receiving for both channels.
	// Instead receive into a local buffer, then do the dsp on both channels.

#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		samples[chan_cnt] = delay_line_wrapper( l_samp[chan_cnt] ,chan_cnt ,samp_freq ,delay_ms );

//	samples[chan_cnt] = l_samp[chan_cnt]; // Dbg
	} // for chan_cnt
} // get_samples_from_host
/******************************************************************************/
#pragma unsafe arrays
void give_samples_to_device(
	chanend c_out, // audio-out channel 
	const int samples[] 
)
{
	int chan_cnt; // channel counter

#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		int sample;

		sample = samples[chan_cnt];
		outuint( c_out ,sample ); // Transmit sample to Audio Driver
	} // for chan_cnt
} // give_samples_to_device
/******************************************************************************/
#pragma unsafe arrays
void get_samples_from_device(
	chanend c_inp, // audio-in channel 
	int samples[]
)
{
	int chan_cnt; // channel counter

#pragma loop unroll
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_IN; chan_cnt++)
	{
		int sample;

		sample = inuint( c_inp );
		samples[chan_cnt] = sample; // Receive sample from Audio-Driver
	} // chan_cnt
} // get_samples_from_device
/******************************************************************************/
void dsp( // processes audio stream using dsp functions
	chanend c_aud_in, // Input audio stream
	chanend c_aud_out, // Output audio stream
	in port p_button_a, // 1st Input button for selecting DSP functions
	in port p_button_b // 2nd Input button for selecting DSP functions
)
{
	int inp_samples[NUM_USB_CHAN_IN]; // array of samples for each input channel
	int out_samples[NUM_USB_CHAN_OUT]; // array of samples for each output channel

	int sampFreq; // Current sample frequency
	int delay_ms = 58; // Delay time in milli-seconds


	set_port_inv (p_button_a);
	set_port_inv (p_button_b);

	// zero sample buffers.
	safememset( (inp_samples ,unsigned char[]) ,0 ,(NUM_USB_CHAN_IN * sizeof( int )) ); 
	safememset( (out_samples ,unsigned char[]) ,0 ,(NUM_USB_CHAN_OUT * sizeof( int )) ); 

	while (1) 
	{
		inuint( c_aud_out );
	
		// Request data from decouple thread 
		outuint( c_aud_in ,0 );
	
		if (testct( c_aud_in )) 
		{
			// sample frequency change.
			inct( c_aud_in );
			sampFreq = inuint( c_aud_in );

			// Send SF change to audio thread.
			outct( c_aud_out ,XS1_CT_END );
			outuint( c_aud_out ,sampFreq );

			// wait for handshake and send back
			chkct( c_aud_out ,XS1_CT_END );
			outct( c_aud_in ,XS1_CT_END );
		} // if (testct( c_aud_in )) 
		else 
		{
			// Normal audio loop.
			// Get info from host indicating we're ready and tell the audio
			// thread data is on its way.
			inuint( c_aud_in );
	
			outuint( c_aud_out ,0 );
	
			give_samples_to_device( c_aud_out ,out_samples );
			get_samples_from_device( c_aud_out ,inp_samples );
			give_samples_to_host( c_aud_in ,inp_samples );
			get_samples_from_host( c_aud_in ,out_samples ,sampFreq ,delay_ms );
		}; // else !(testct( c_aud_in )) 
	
	} // 	while (1) 
} // dsp
/******************************************************************************/
// dsp.xc
