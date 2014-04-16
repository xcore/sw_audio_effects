/******************************************************************************\
 * File:	main.xc
 *
 * Description: Top level module for Reverb application, launches all coars
 * for L2 Slice Kit Tile Board with Audio Slice 1v0
 * Note: This application expects a Audio Slice (1v0) to be connected to a Type 1 Socket on core AUDIO_IO_TILE
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

#ifdef USE_XSCOPE
/*****************************************************************************/
void xscope_user_init( void ) // 'C' constructor function (NB called before main)
{
	xscope_register( 1
		,XSCOPE_CONTINUOUS ,"Dummy" ,XSCOPE_INT ,"n"
	); // xscope_register

	xscope_config_io( XSCOPE_IO_BASIC ); // Enable XScope printing
} // xscope_user_init
#endif // ifdef USE_XSCOPE

/*****************************************************************************/
int main (void)
{
	streaming chan c_aud_dsp; // Channel between I/O and Processing coars
	streaming chan c_dsp_slaves[NUM_VOCODER_SLAVES]; // Array of channels between DSP master and vocoder slaves


	par
	{
		on tile[TILE_ID] :
		{
		  init_locks(); // Initialise Mutex for display

			par {
				audio_io( c_aud_dsp ); // Audio I/O core

				dsp_vocoder_control( c_aud_dsp ,c_dsp_slaves ); // DSP-control core

				// Loop through all motors
				par (int slave_cnt=0; slave_cnt<NUM_VOCODER_SLAVES; slave_cnt++)
				{
					dsp_channel_vocoder( c_dsp_slaves[slave_cnt] ,slave_cnt ); // DSP-control core
				} // par slave_cnt
	
			} // par

		  free_locks(); // Free Mutex for display
		} // on tile[TILE_ID]
	} // par

	return 0;
} // main
/*****************************************************************************/
// main.xc
