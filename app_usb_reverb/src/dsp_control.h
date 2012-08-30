/******************************************************************************\
 * Header:  dsp_control
 * File:    dsp_control.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for dsp_control.xc
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

#ifndef _DSP_CONTROL_H_
#define _DSP_CONTROL_H_

#include <xs1.h>
#include <safestring.h>
#include <print.h>
#include "devicedefines.h"
#include "audio_sample_io.h"
#include "delay_line.h"

/******************************************************************************/
void dsp_control( // Controls audio stream processing for reverb application using dsp functions
	chanend c_control_in, // Input audio stream (from Decoupler)
	chanend c_dsp_eq, // Channel connects DSP and Equalisation threads (bi-directional)
	chanend c_control_out // Output audio stream (to Audio Driver)
);

/******************************************************************************/

#endif // _DSP_CONTROL_H_
/******************************************************************************/
// dsp_control.h
