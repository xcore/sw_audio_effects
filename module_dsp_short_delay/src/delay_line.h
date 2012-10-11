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
#include "module_dsp_short_delay_conf.h"
#include "common_audio.h"
#include "common_utils.h"

#ifndef NUM_DELAY_CHANS 
	#error Define. NUM_DELAY_CHANS in app_conf.h
#endif // NUM_DELAY_CHANS

/** Memory available to configure delay buffers (in samples). Adjust to suit platform: 12000 */
#define MEM_SAMPS 12000

/**  Size of channel delay buffer (in samples): (MEM_SAMPS / NUM_DELAY_CHANS) */
#define DELAY_SIZE (MEM_SAMPS / NUM_DELAY_CHANS)

/** Maximum number of different taps supported: 8 */
#define MAX_TAPS 8

/** Default number of taps: 4*/
#define DEF_TAPS 4

/** Default Sample Frequency (In Hz): 48000 */
#define DEF_SAMP_FREQ 48000

/** Structure containing Delay-line parameters */
typedef struct DELAY_PARAM_TAG // 
{
	S32_T us_delays[MAX_TAPS]; // Set of delays in micro-seconds
	S32_T num; // No. of delay taps in use
	S32_T freq; // Sample frequency
} DELAY_PARAM_S;

/******************************************************************************/
/** Exercise delay-line for one input sample. NB Must have previously called config_delay_line.
 * Samples are left-aligned signed values.
 * e.g. 24-bit audio will look like 0x12345600 (positive) or 0xFF123400 (negative)
 * \param out_samps[] // array of delayed output samples at channel precision
 * \param inp_samp // Input Sample
 * \param cur_chan // current channel
 * \return The delayed output sample
 */
void use_delay_line( // Exercise delay-line for one input sample. NB Must have previously called config_delay_line
	S32_T out_samps[], // Array of delayed output samples
	S32_T inp_samp, // Input Sample
	S32_T cur_chan // Current channel
); // Return delayed output sample
/******************************************************************************/

#ifdef __XC__
// XC File

/******************************************************************************/
/** Configure delay_line parameters. NB Must be called before use_delay_line.
 * \param cur_param_s // Reference to structure containing delay-line parameters
 */
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
