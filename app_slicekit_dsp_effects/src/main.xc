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
	streaming chan c_dsp_eq[NUM_BIQUADS]; // Array of channel between DSP-control and Equalisation coars
	streaming chan c_dsp_gain; // Channel between DSP-control and Loudness coars
  chan c_dsp_sdram; // Channel between DSP coar and SDRAM coar 
  chan c_dsp_gpio; // Channel between DSP coar and GPIO coar


	par
	{
		on tile[AUDIO_IO_TILE]: audio_io( c_aud_dsp ); // Audio I/O coar

		on tile[DSP_TILE]: dsp_effects( c_aud_dsp ,c_dsp_eq ,c_dsp_gain ,c_dsp_sdram ,c_dsp_gpio ); // Control coar for DSP Effects

		on tile[GAIN_TILE]: dsp_loudness( c_dsp_gain ); // non-linear-gain (Loudness) coar

    on tile[MEM_TILE]: sdram_io( c_dsp_sdram ); // SDRAM coar

    on tile[GPIO_TILE]: gp_io( c_dsp_gpio ); // GPIO coar

		on tile[BIQUAD0_TILE]: dsp_biquad( c_dsp_eq[0] ); // BiQuad Equalisation coar
		on tile[BIQUAD1_TILE]: dsp_biquad( c_dsp_eq[1] ); // BiQuad Equalisation coar
#ifdef MB
		par (int biquad_cnt=0; biquad_cnt<NUM_BIQUADS; biquad_cnt++)
		{
			on tile[BIQUAD_TILE]: dsp_biquad( c_dsp_eq[biquad_cnt] ); // BiQuad Equalisation coar
		} // par biquad_cnt
#endif //MB~
	} // par

	return 0;
} // main
/*****************************************************************************/
// main.xc
