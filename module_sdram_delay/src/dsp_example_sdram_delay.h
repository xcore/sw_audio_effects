/******************************************************************************\
 * Header:  dsp_sdram_delay
 * File:    dsp_sdram_delay.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for dsp_sdram_delay.xc
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

#ifndef _DSP_SDRAM_DELAY_H_
#define _DSP_SDRAM_DELAY_H_

#include "types64bit.h"
#include "common_utils.h"
#include "delay_line.h"

/******************************************************************************/
void dsp_sdram_delay( // Thread that delays a stream of audio samples
	streaming chanend c_aud_dsp, // Channel between Audio_IO thread and DSP thread (bi-directional)
  chanend c_dsp_sdram // Channel between DSP thread and SDRAM thread (bi-directional)
);
/******************************************************************************/

#endif // _DSP_SDRAM_DELAY_H_
/******************************************************************************/
// dsp_sdram_delay.h
