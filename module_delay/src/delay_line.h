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
#include <print.h>
#include "types64bit.h"
#include "common_audio.h"

#ifndef NUM_DELAY_CHANS 
#error Please define NUM_DELAY_CHANS in Makefile
#endif // NUM_DELAY_CHANS

#define MEM_SAMPS 13000 // Memory available to configure delay buffers (in samples). Adjust to suit platform
#define DELAY_SIZE (MEM_SAMPS / NUM_DELAY_CHANS)// Size of channel delay buffer (in samples).

#define MAX_TAPS 8 // Max. No. of different taps supported
#define DEF_TAPS 4 // Default number of taps
#define DEF_SAMP_FREQ 48000 // Default Sample Frequency (In Hz)

typedef struct DELAY_PARAM_TAG // Structure containing BiQuad parameters
{
	S32_T us_delays[MAX_TAPS]; // Set of delays in micro-seconds
	S32_T num; // No. of delay taps in use
	S32_T freq; // Sample frequency
} DELAY_PARAM_S;

/******************************************************************************/
void use_delay_line( // Exercise delay-line for one input sample. NB Must have previously called config_delay_line
	S32_T out_samps[], // array of delayed output samples at channel precision
	S32_T inp_samp, // Input Sample
	S32_T cur_chan // current channel
); // Return delayed output sample
/******************************************************************************/

#ifdef __XC__
// XC File

/******************************************************************************/
void config_delay_line( // Configure delay_line parameters. NB Must be called before use_delay_line
	DELAY_PARAM_S &cur_param_s // Reference to structure containing delay-line parameters
);
/******************************************************************************/

#else //if __XC__
// 'C' File

/******************************************************************************/
void config_delay_line( // Configure delay_line parameters. NB Must be called before use_delay_line
	DELAY_PARAM_S * cur_param_ps // Pointer to structure containing delay-line parameters
);
/******************************************************************************/

#define MAX_DELAY (DELAY_SIZE - 1) // Max index into delay buffer

#define MIN_AUDIO_FREQ 20 // Minimum Audio Frequency (In Hz)

typedef struct DELAY_CHAN_TAG // Structure containing delayed data for one channel, updated every sample
{
	SAMP_CHAN_T buf[DELAY_SIZE]; // Delay-line Buffer
	SAMP_CHAN_T outs[MAX_TAPS]; // Buffer to hold set of delayed output samples
	U32_T delays[MAX_TAPS]; // offset into Delay-line for delayed output samples
	SAMP_CHAN_T inp; // offset into Delay-line for input sample
	S32_T tap_num;		// Number of delay taps in use on this channel
} DELAY_CHAN_S;

typedef struct DELAY_TAG // Structure containing all delay data
{
	DELAY_CHAN_S chan_s[NUM_DELAY_CHANS]; // Array of structure containing data for each channel
	S32_T init_done;	// Flag indicating Delay is initialised
	S32_T params_set; // Flag indicating Parameters are set
} DELAY_S;

#endif // else __XC__

#endif /* _DELAY_LINE_H_ */

// delay_line.h
