/******************************************************************************\
 * File:	delay_line.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for module delay_line
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

#ifndef _DELAY_LINE_H_
#define _DELAY_LINE_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types64bit.h"
#include "customdefines.h"

#ifndef NUM_USB_CHAN_OUT
#define NUM_USB_CHAN_OUT (2) // For test purposes
#endif // NUM_USB_CHAN_OUT

#define DEFAULT_DELAY 0 // Default delay (in milli-seconds)

/******************************************************************************/
int delay_line_wrapper( // Delay one input sample, return one (different) delayed output sample
	S32_T inp_samp, // Input Sample
	S32_T cur_chan, // current channel
	S32_T samp_freq, // current sample frequency
	S32_T delay_ms // sample delay in milli-secs
); // Return delayed output sample
/******************************************************************************/

#ifdef __XC__
// XC File

#else //if __XC__
// 'C' File

#define DELAY_SIZE 2700 // Size of delay buffer in samples. Adjust to suit available memory
#define MAX_DELAY (DELAY_SIZE - 1) // Max index into delay buffer

typedef S32_T SAMP_CHAN_T; // Full precision sample type on channel (e.g. 32-bit)

typedef struct DELAY_CHAN_TAG // Structure containing delayed data for one channel, updated every sample
{
	SAMP_CHAN_T buf[DELAY_SIZE]; // Buffer to hold delayed samples
	U32_T delay; // delay for this channel (in samples)
	U32_T inp; // offset into buffer for input sample
	U32_T out; // offset into buffer for output sample
} DELAY_CHAN_S;

typedef struct DELAY_TAG // Structure containing all delay data
{
	DELAY_CHAN_S chan_s[NUM_USB_CHAN_OUT]; // Array of structure containing data for each channel
} DELAY_S;

#endif // else __XC__

#endif /* _DELAY_LINE_H_ */

// delay_line.h
