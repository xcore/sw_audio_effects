/******************************************************************************\
 * Header:  dsp_template
 * File:    dsp_template.h
 *  
 * Description: Definitions, types, and prototypes for dsp_template.xc
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

#ifndef _DSP_TEMPLATE_H_
#define _DSP_TEMPLATE_H_

#include <xs1.h>
#include <print.h>
#include <assert.h>
#include "types64bit.h"
#include "app_global.h"
#include "common_utils.h"

// #include "module_dsp_effect.h" // Insert your module include file here ...
#define NUM_FX_CHANS 4  // ... and delete this line

/******************************************************************************/
void dsp_template( // Coar that applies non-linear gain control to stream of audio samples
	streaming chanend c_dsp_gain // Channel connecting to DSP-control coar (bi-directional)
);
/******************************************************************************/

#endif // _DSP_TEMPLATE_H_
/******************************************************************************/
// dsp_template.h
