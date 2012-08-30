/******************************************************************************\
 * Header:  reverb
 * File:    reverb.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for reverb.xc
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

#ifndef _REVERB_H_
#define _REVERB_H_

#include "types64bit.h"
#include "delay_line.h"

#define NUM_CHANS 2 // Number of channels (NB Only Stereo supported)

/******************************************************************************/
void reverb( // Performs reverb processing
	S32_T equal_samps[],	// Buffer for output equalised samples
	S32_T rev_samps[],	// Buffer for output samples with Reverb added
	S32_T inp_samps[],	// Input sample buffer
	S32_T uneq_samps[],	// Unequalised audio sample buffer
	S32_T num_chans,	// Number of channels (NB Only Stereo supported)
	S32_T samp_freq, // Current sample frequency
	S32_T delay_ms // Delay time in milli-seconds
);
/******************************************************************************/

#ifdef __XC__
// XC File

#else //if __XC__
// 'C' File

#define SAMP_BITS ((sizeof(SAMP_CHAN_T) << 3) - 1) // Bits used to hold magnitude of channel sample (e.g. 31)
#define MAX_SAMP (((S64_T)1 << SAMP_BITS) - 1) // Maximum sample value

#endif // else __XC__

#endif // _REVERB_H_
/******************************************************************************/
// reverb.h
