/******************************************************************************\
 * Header:  dsp_biquad
 * File:    dsp_biquad.h
 *  
 * Description: Definitions, types, and prototypes for dsp_biquad.xc
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

#ifndef _DSP_BIQUAD_H_
#define _DSP_BIQUAD_H_

#include <xs1.h>
#include <print.h>
#include "types64bit.h"
#include "app_global.h"
#include "common_utils.h"
#include "reverb.h"
#include "biquad_simple.h"

/******************************************************************************/
void dsp_biquad( // Coar that applies Equalisation (Tone control) to stream of audio samples
	streaming chanend c_dsp_eq // Channel connecting to DSP-control coar (bi-directional)
);
/******************************************************************************/

#endif // _DSP_BIQUAD_H_
/******************************************************************************/
// dsp_biquad.h
