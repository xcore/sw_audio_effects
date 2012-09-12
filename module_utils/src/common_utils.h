/******************************************************************************\
 * Header:  common_utils
 * File:    common_utils.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for common_utils.xc
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

#ifndef _COMMON_UTILS_H_
#define _COMMON_UTILS_H_

#include "types64bit.h"

#define SWAP_NUM (1 << 19)	// Swap effect after this many samples

#define FADE_BITS 14 // Log2[FADE_LEN] (in Samples)
#define FADE_LEN (1 << FADE_BITS) // Length of cross-fade in samples
#define HALF_FADE (FADE_LEN >> 1) // Half cross-fade length

typedef enum PROC_STATE_TAG // Different Processing States
{
  EFFECT = 0,			// DSP Effect On
  FX2DRY,					// Fade Effect to Dry
  DRY_ONLY,				// No Effect
  DRY2FX,					// Fade Dry to Effect
  NUM_PROC_STATES	// Handy Value!-)
} PROC_STATE_TYP;

typedef enum CNTRL_TOK_TAG // Control Token Codes
{
  CFG_BIQUAD = 10,	// Configure BiQuad Filter
  CFG_GAIN,			// Configure Gain-shaping
  NUM_CNTRL_TOKS	// Handy Value!-)
} CNTRL_TOK_TYP;

/*****************************************************************************/
void cross_fade_sample( // Returns Cross-faded sample
	S32_T out_samps[],	// Buffer for cross-faded Output samples
	S32_T fade_out_samps[],	// Buffer for Input samples being faded-out
	S32_T fade_in_samps[],	// Buffer for Input samples being faded-in
	S32_T num_chans,	// Number of channels
	S32_T weight	// Weighting for fade-in sample
); // Return cross-faded sample
/******************************************************************************/

#endif // _COMMON_UTILS_H_
// common_utils.h
