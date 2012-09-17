/******************************************************************************\
 * File:	biquad_simple.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for module biquad_simple
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

#ifndef _BIQUAD_SIMPLE_H_
#define _BIQUAD_SIMPLE_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <print.h>
#include "types64bit.h"
#include "common_audio.h"

#ifndef NUM_BIQUAD_CHANS
#error Please define NUM_BIQUAD_CHANS in Makefile
#endif // NUM_BIQUAD_CHANS

typedef enum FILT_MODE_TAG // Different Processing States
{
  LO_PASS = 0,	// Low Pass Filter
  HI_PASS,			// High Pass Filter
  BAND_PASS,		// Band Pass Filter
  BAND_STOP,		// Band Stop (Notch) Filter
  ALL_PASS,			// Phase-Shift Filter
  CUSTOM,				// Custom Filter
  NUM_FILT_MODES	// Handy Value!-)
} FILT_MODE_TYP;

// Filter parameters 
#define DEF_FILT_MODE LO_PASS // Default Filter Mode
#define DEF_SAMP_FREQ 48000 // Default Sample Frequency (In Hz)
#define DEF_SIG_FREQ 1000 // Default Significant Filter Frequency (In Hz)

#define QUAL_BITS 8 // No. Of bits used to scale Quality-factor
#define DEF_QUAL_FACT (1 << QUAL_BITS) // Default Quality-factor of 1, scaled by QUAL_BITS

typedef struct BIQUAD_PARAM_TAG // Structure containing BiQuad parameters
{
	FILT_MODE_TYP filt_mode; // BiQuad filter type (e.g. Low-pass)
	S32_T samp_freq; // Sample frequency
	S32_T sig_freq; // Significant filter frequency (e.g. centre-freq of band-pass)
	S32_T qual; // Quality-factor
} BIQUAD_PARAM_S;

/******************************************************************************/
S32_T use_biquad_filter( // Use BiQuad filter on one sample from one channel
	S32_T inp_samp, // Unfiltered input sample from channel
	S32_T cur_chan // current channel
); // Return filtered Output Sample
/******************************************************************************/

#ifdef __XC__
// XC File

/******************************************************************************/
void config_biquad_filter( // Configure BiQuad filter 
	BIQUAD_PARAM_S &cur_param_ps // Reference to structure containing current biquad filter parameters
);

/******************************************************************************/
#else //if __XC__
// 'C' File

/******************************************************************************/
void config_biquad_filter( // Configure BiQuad filter 
	BIQUAD_PARAM_S * cur_param_ps // Pointer to structure containing current biquad filter parameters
);
/******************************************************************************/

#define PI 3.14159265

// Filter parameters 
#define MIN_SAMP_FREQ 20 // Minimum Sample Frequency (In Hz)

typedef R64_T REAL_T; // Real or floating-point type (NB not supported in earlier XC compilers)
typedef S32_T COEF_T; // Coefficients are represented as Fixed-point values. The Mantissa & Exponent are both of type COEF_T
typedef S64_T FILT_T; // type used for internal filter calculations

/* Scaling Data
 * BiQuad requires 5 MAC's, Therefore 3 bits of headroom required for accumulator
 * On Input, samples are stored in the MS-bits of a 32-bit integer.
 * The input samples are down-scaled to the LS-Bits of the integer, before any calculation.
 * This provides 8 bits of headroom
 */
#define SAMP_BITS ((sizeof(SAMP_CHAN_T) << 3) - 1) // Bits used to represent magnitude of input/output sample (i.e without sign bit)
#define COEF_BITS ((sizeof(COEF_T) << 3) - 1) // Bits used to represent magnitude of full precision sample (i.e without sign bit)
#define HEAD_BITS 8 // Headroom Bits

#define HEAD_FACT (1 << HEAD_BITS) // coefficient up-scaling factor
#define HALF_HEAD (HEAD_FACT >> 1) // half up-scaling factor

#define NUM_FILT_TAPS 3 // Number of filter taps (for FIR or IIR, I.E. FIR+IIR = 2 * NUM_FILT_TAPS)

typedef struct FIX_POINT_TAG // Structure containing coefficients expressed as fixed point
{
	COEF_T mant; // Mantissa
	COEF_T exp; // Exponent expressed as power of 2 (Log2 of scaling factor)
	COEF_T half; // Half of scaling factor (used in rounding)
	COEF_T err; // rounding error
} FIX_POINT_S;

typedef struct BIQUAD_COEF_TAG // Structure containing BiQuad coefficients
{
	FIX_POINT_S b[NUM_FILT_TAPS]; // Array of weighting for input samples (FIR taps)
	FIX_POINT_S a[NUM_FILT_TAPS]; // Array of weighting for output results (IIR taps)
} BIQUAD_COEF_S;

typedef struct BIQUAD_CHAN_TAG // Structure containing intermediate data for one channel, updated every sample
{
	FILT_T iir[NUM_FILT_TAPS]; // Array of previous IIR filter results
	FILT_T inp[NUM_FILT_TAPS]; // Array of previous input samples
	FILT_T filt[NUM_FILT_TAPS]; // Array of previous filterd output samples

	FILT_T inp_err; // Rounding error due to down-sampling input
	FILT_T iir_err; // Rounding error due to down-sampling IIR output
	FILT_T filt_err; // Rounding error due to down-sampling filtered output
} BIQUAD_CHAN_S;

typedef struct BIQUAD_TAG // Structure containing all biquad data
{
	BIQUAD_CHAN_S bq_chan_s[NUM_BIQUAD_CHANS]; // Array of structure containing intermediate data for each channel, updated every sample
	BIQUAD_COEF_S common_coefs_s;  // Structure containing BiQuad coefficients
	S32_T init_done;	// Flag indicating gain-shaping is initialised
	S32_T params_set; // Flag indicating Parameters are set
} BIQUAD_S;

#endif // else __XC__

#endif /* _BIQUAD_SIMPLE_H_ */

// biquad_simple.h
