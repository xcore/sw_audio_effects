/******************************************************************************\
 * Description: Definitions, types, and prototypes for dsp_channel_vocoder.xc
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

#ifndef _DSP_CHANNEL_VOCODER_H_
#define _DSP_CHANNEL_VOCODER_H_

#include <xs1.h>
#include <print.h>
#include <assert.h>

#include "common_audio.h"
#include "app_global.h"
#include "common_utils.h"
#include "vocoder.h"

/******************************************************************************/
void dsp_channel_vocoder( // Applies a number of bands of vocoder processing to input stream
	streaming chanend c_dsp_vocoder, // Channel connecting to DSP-control core (bi-directional)
	S32_T slave_id // Unique Vocoder slave identifier
);
/******************************************************************************/

#endif // _DSP_CHANNEL_VOCODER_H_
/******************************************************************************/
// dsp_channel_vocoder.h
