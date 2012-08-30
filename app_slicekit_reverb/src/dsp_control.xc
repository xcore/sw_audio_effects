/******************************************************************************\
 * File:	dsp_control.xc
 * Author: Mark Beaumont
 * Description: Control thread for Reverb, also handles delay functionality, 
 *	calls Loudness and Equalisation threads
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

#include "dsp_control.h"

// DSP-control thread.

/*****************************************************************************/
void dsp_control( // Controls audio stream processing for reverb application using dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O thread (bi-directional)
	streaming chanend c_dsp_eq, // Channel connecting to Equalisation thread (bi-directional)
	streaming chanend c_dsp_gain // Channel connecting to Loudness thread (bi-directional)
)
#define SWAP_NUM (1 << 19)	// Swap effect after this many samples
{
	S32_T inp_samps[I2S_CHANS_ADC];	// Input audio sample buffer
	S32_T uneq_samps[I2S_CHANS_ADC];	// Unequalised audio sample buffer
	S32_T equal_samps[I2S_CHANS_DAC];	// Equalised  audio sample buffer
	S32_T unamp_samps[I2S_CHANS_DAC];	// Unamplified audio sample buffer
	S32_T out_samps[I2S_CHANS_DAC];	// Output audio sample buffer

	S32_T samp_cnt = 0;	// Sample counter
	S32_T reverb_on = 1;	// Reverb status flag
	S32_T min_chans = I2S_CHANS_DAC;	// Preset minimum of input/output channels to No. of Output channels
	S32_T chan_cnt; // Channel counter
	S32_T samp_freq = SAMP_FREQ; // Current sample frequency
	int delay_ms = 150; // Delay time in milli-seconds
	

	// if necessary, update minimum number of channels
	if (min_chans > I2S_CHANS_ADC)
	{
		min_chans = I2S_CHANS_ADC;
	} // if (min_chans > I2S_CHANS_ADC)

	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < min_chans; chan_cnt++)
	{
		inp_samps[chan_cnt] = 0;
		uneq_samps[chan_cnt] = 0;
		equal_samps[chan_cnt] = 0;
		unamp_samps[chan_cnt] = 0;
		out_samps[chan_cnt] = 0;
	}	// for chan_cnt

	// Loop forever
	while(1)
	{
		// Send/Receive audio samples over sample thread channels
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < min_chans; chan_cnt++)
		{

			// Service channels in chronological order
			c_aud_dsp :> inp_samps[chan_cnt]; // Receive input samples from Audio I/O thread 

			c_dsp_eq <: uneq_samps[chan_cnt]; // Send Unequalised samples to Eq thread  
			c_dsp_eq :> equal_samps[chan_cnt]; // Receive Equalised samples back from Eq thread  

			c_dsp_gain <: unamp_samps[chan_cnt]; // Send Unamplified samples to Loudness thread   
			c_dsp_gain :> out_samps[chan_cnt]; // Receive Amplified samples back from Loudness thread  

			c_aud_dsp <: out_samps[chan_cnt];  // Send Output samples back to Audio I/O thread 
		} // for chan_cnt

		// Check if its time to toggle reverb effect
		if (SWAP_NUM > samp_cnt)
		{
			samp_cnt++;
		} // if (SWAP_NUM > samp_cnt)
		else
		{
			samp_cnt = 0; // Reset sample counter
			reverb_on = 1 - reverb_on; // Toggle reverb effect
		} // if (SWAP_NUM > samp_cnt)

		// Check if effect on or off
		if (1 == reverb_on)
		{	// Do reverb processing
			reverb( uneq_samps ,unamp_samps ,inp_samps ,equal_samps ,min_chans ,samp_freq ,delay_ms );
		} // if (1 == reverb_on)
		else
		{ // Copy inputs to outputs
			for (chan_cnt = 0; chan_cnt < min_chans; chan_cnt++)
			{ // NB Add a bit of filtering to prevent clicks on transitions
				uneq_samps[chan_cnt] = (uneq_samps[chan_cnt] + inp_samps[chan_cnt]) >> 1;
				unamp_samps[chan_cnt] = (unamp_samps[chan_cnt] + equal_samps[chan_cnt]) >> 1;
			} // for chan_cnt
		} // else !(1 == reverb_on)
	} // while(1)

} // dsp_control
/*****************************************************************************/
// dsp_control.xc
