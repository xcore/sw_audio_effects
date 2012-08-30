/******************************************************************************\
 * Header:  dsp_loudness
 * File:    dsp_loudness.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for dsp_loudness.xc
 *
 * Version: 3v3rc1
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

#ifndef _DSP_LOUDNESS_H_
#define _DSP_LOUDNESS_H_

#include <xs1.h>
#include <safestring.h>
#include <print.h>
#include "devicedefines.h"
#include "audio_sample_io.h"
#include "non_linear_gain.h"

/******************************************************************************/
void dsp_loudness( // Applies gain-control to audio stream as part of reverb application
	chanend c_gain_in, // Input audio stream (from dsp_control)
	chanend c_gain_out // Output audio stream (to Audio Driver)
);

/******************************************************************************/

#endif // _DSP_LOUDNESS_H_
/******************************************************************************/
// dsp_loudness.h
