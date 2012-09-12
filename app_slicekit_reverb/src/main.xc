/******************************************************************************\
 * File:	main.xc
 * Author: Mark Beaumont
 * Description: Top level module for Reverb application, launches all threads
 * for L2 Slice Kit Core Board with Audio Slice 1v0 
 * Note: This application expects a Audio Slice (1v0) to be connected to a Type 1 Socket on core AUDIO_IO_CORE
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

#include "main.h"

// Global variables

on stdcore[AUDIO_IO_CORE] : buffered out port:32 p_lrclk	= PORT_I2S_LRCLK;
on stdcore[AUDIO_IO_CORE] : buffered out port:32 p_bclk		= PORT_I2S_BCLK;
on stdcore[AUDIO_IO_CORE] : port p_mclk										= PORT_MCLK_IN;

on stdcore[AUDIO_IO_CORE] : out port p_gpio	= PORT_GPIO;
on stdcore[AUDIO_IO_CORE] : port p_i2c			= PORT_I2C;

on stdcore[AUDIO_IO_CORE] : clock	clk_audio_mclk	= XS1_CLKBLK_2;	 /* Master clock */
on stdcore[AUDIO_IO_CORE] : clock	clk_audio_bclk	= XS1_CLKBLK_3;	 /* Bit clock */

/*****************************************************************************/
int main (void)
{
	streaming chan c_aud_dsp; // Channel between I/O and Processing threads
	streaming chan c_dsp_eq; // Channel between DSP-control and Equalisation threads
	streaming chan c_dsp_gain; // Channel between DSP-control and Loudness threads


	par
	{
		on stdcore[AUDIO_IO_CORE]: audio_io( c_aud_dsp ); // Audio I/O thread

		on stdcore[0]: dsp_control( c_aud_dsp ,c_dsp_eq ,c_dsp_gain ); // DSP-control thread

		on stdcore[AUDIO_IO_CORE]: dsp_biquad( c_dsp_eq ); // BiQuad Equalisation thread

		on stdcore[AUDIO_IO_CORE]: dsp_loudness( c_dsp_gain ); // non-linear-gain (Loudness) thread
	}

	return 0;
} // main
/*****************************************************************************/
// main.xc
