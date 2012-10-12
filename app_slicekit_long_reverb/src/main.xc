/******************************************************************************\
 * File:	main.xc
 *  
 * Description: Top level module for Reverb application, launches all threads
 * for L2 Slice Kit Tile Board with Audio Slice 1v0 
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

/*****************************************************************************/
int main (void)
{
	streaming chan c_aud_dsp; // Channel between I/O and Processing threads
	streaming chan c_dsp_eq; // Channel between DSP-control and Equalisation threads
	streaming chan c_dsp_gain; // Channel between DSP-control and Loudness threads
  chan c_dsp_sdram; // Channel between DSP thread and SDRAM thread 



	par
	{
		on stdcore[AUDIO_IO_CORE]: audio_io( c_aud_dsp ); // Audio I/O thread

		on stdcore[DSP_CORE]: dsp_sdram_reverb( c_aud_dsp ,c_dsp_eq ,c_dsp_gain ,c_dsp_sdram ); // DSP control thread for reverb

		on stdcore[BIQUAD_CORE]: dsp_biquad( c_dsp_eq ); // BiQuad Equalisation thread

		on stdcore[GAIN_CORE]: dsp_loudness( c_dsp_gain ); // non-linear-gain (Loudness) thread

    on stdcore[MEM_CORE]: sdram_io( c_dsp_sdram ); // SDRAM thread
	}

	return 0;
} // main
/*****************************************************************************/
// main.xc
