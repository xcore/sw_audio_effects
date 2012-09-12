/******************************************************************************\
 * Header:  dsp_delay
 * File:    dsp_delay.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for dsp_delay.xc
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

#ifndef _DSP_DELAY_H_
#define _DSP_DELAY_H_

#include <print.h>
#include "types64bit.h"
#include "global.h"
#include "common_utils.h"
#include "delay_line.h"

/******************************************************************************/
void dsp_delay( // Thread that applies non-linear gain control to stream of audio samples
	streaming chanend c_dsp_gain // Channel connecting to DSP-control thread (bi-directional)
);
/******************************************************************************/

#endif // _DSP_DELAY_H_
/******************************************************************************/
// dsp_delay.h
