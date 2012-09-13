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

// Structure for I2S master (sc_i2s/module_i2s_master)
on stdcore[AUDIO_IO_CORE] : struct r_i2s r_i2s = 
{
	XS1_CLKBLK_1,
	XS1_CLKBLK_2,
	PORT_MCLK_IN,
	PORT_I2S_BCLK,
	PORT_I2S_LRCLK,
#if NUM_GAIN_CHANS == 4
	{PORT_I2S_ADC0, PORT_I2S_ADC1},
	{PORT_I2S_DAC0, PORT_I2S_DAC1}
#elif NUM_GAIN_CHANS == 2
	{PORT_I2S_ADC0 },
	{PORT_I2S_DAC0 }
#else
#error Unsupported No Of Channels
#endif
};
	
on stdcore[AUDIO_IO_CORE] : out port p_gpio = PORT_GPIO;
on stdcore[AUDIO_IO_CORE] : port p_i2c = PORT_I2C;


/*****************************************************************************/
int main (void)
{
	streaming chan c_aud_dsp; // Channel between I/O and DSP Loudness thread


	par
	{
		on stdcore[AUDIO_IO_CORE]: audio_io( c_aud_dsp, r_i2s ); // Audio I/O thread

		on stdcore[0]: dsp_loudness( c_aud_dsp ); // non-linear-gain (Loudness) thread
	}

	return 0;
} // main
/*****************************************************************************/
// main.xc
