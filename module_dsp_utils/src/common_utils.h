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

#include <math.h>
#include "types64bit.h"
#include "common_audio.h"

#define SWAP_NUM (1 << 19)	// Swap effect after this many samples

#define FADE_BITS 14 // Log2[FADE_LEN] (in Samples)
#define FADE_LEN (1 << FADE_BITS) // Length of cross-fade in samples
#define HALF_FADE (FADE_LEN >> 1) // Half cross-fade length

typedef S32_T COEF_T; // Coefficients are represented as Fixed-point values. The Mantissa & Exponent are both of type COEF_T

typedef struct FIX_POINT_TAG // Structure containing coefficients expressed as fixed point
{
	COEF_T mant; // Mantissa
	COEF_T exp; // Exponent expressed as power of 2 (Log2 of scaling factor)
} FIX_POINT_S;

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

/******************************************************************************/
S32_T increment_circular_offset( // Increment a circular buffer offset. I.e. wraps if at end of buffer
	S32_T new_off, // required new offset (without wrapping)
	S32_T buf_siz // circular buffer size
	); // Return wrapped offset
/*****************************************************************************/
void cross_fade_sample( // Returns Cross-faded sample
	S32_T out_samps[],	// Buffer for cross-faded Output samples
	S32_T fade_out_samps[],	// Buffer for Input samples being faded-out
	S32_T fade_in_samps[],	// Buffer for Input samples being faded-in
	S32_T num_chans,	// Number of channels
	S32_T weight	// Weighting for fade-in sample
); // Return cross-faded sample
/******************************************************************************/

#ifdef __XC__
// XC File

#else //if __XC__
// 'C' File

/******************************************************************************/
void scale_coef( // Scale and round floating point coeffiecient
	FIX_POINT_S * fix_coef_ps, // pointer to structure for fixed point format
	R64_T un_coef // input unscaled floating point coef.
);
/******************************************************************************/

#endif // else __XC__

#endif // _COMMON_UTILS_H_
// common_utils.h
