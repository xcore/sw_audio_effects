/*****************************************************************************\
 * Header:  dsp_sdram_reverb
 * File:    dsp_sdram_reverb.h
 *  
 * Description: Definitions, types, and prototypes for dsp_sdram_reverb.xc
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
\*****************************************************************************/

#ifndef _DSP_SDRAM_CONTROL_H_
#define _DSP_SDRAM_CONTROL_H_

#include <xs1.h>
#include <print.h>

#include "types64bit.h"
#include "app_global.h"
#include "common_utils.h"
#include "sdram_io.h"
#include "sdram_reverb.h"

#ifdef USE_XSCOPE
#include <xscope.h>
#endif // ifdef USE_XSCOPE

/*****************************************************************************/
void dsp_sdram_reverb( // Controls audio stream processing for reverb application using dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O coar (bi-directional)
	streaming chanend c_dsp_eq, // Channel connecting to Equalisation coar (bi-directional)
	streaming chanend c_dsp_gain, // Channel connecting to Loudness coar (bi-directional)
  chanend c_dsp_sdram // DSP end of channel between DSP coar and SDRAM coar (bi-directional)
);
/*****************************************************************************/

#endif // _DSP_SDRAM_CONTROL_H_
/*****************************************************************************/
// dsp_sdram_reverb.h
