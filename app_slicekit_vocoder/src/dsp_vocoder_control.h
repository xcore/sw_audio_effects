/*****************************************************************************\
 * Description: Definitions, types, and prototypes for dsp_control.xc
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

#ifndef _DSP_VOCODER_CONTROL_H_
#define _DSP_VOCODER_CONTROL_H_

#include <xs1.h>
#include <print.h>
#include <assert.h>

#include "common_audio.h"
#include "app_global.h"
#include "common_utils.h"
#include "vocoder.h"

#define NUM_SLAVE_OUTPUT_CHANS 2 // NB Assume Stereo output per slave

/*****************************************************************************/
void dsp_vocoder_control( // Controls audio stream processing for vocoder application using dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O core (bi-directional)
	streaming chanend c_dsp_slaves[] // Array of channels between DSP master and vocoder slaves
);
/*****************************************************************************/

#endif // _DSP_VOCODER_CONTROL_H_
/*****************************************************************************/
// dsp_vocoder_control.h
