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
#include <math.h>
#include <assert.h>
#include "types64bit.h"
#include "customdefines.h"

#ifndef NUM_USB_CHAN_OUT
#define NUM_USB_CHAN_OUT (2) // For test purposes
#endif // NUM_USB_CHAN_OUT

/******************************************************************************/
int biquad_filter_wrapper( // Call filter for one sample
	S32_T inp_samp, // Unfiltered Input Sample
	S32_T cur_chan, // current channel
	S32_T samp_freq // current sample frequency
); // Return Filtered Output Sample
/******************************************************************************/

#ifdef __XC__
// XC File

#else //if __XC__
// 'C' File

#define PI 3.14159265

// Filter parameters 
#define LPF_NOMINAL_FREQ 1000 // Nominal Frequency (In Hz)
#define LPF_QUAL_FACT 1 // Quality Factor (Nominal_F / Delta_F)

typedef R64_T REAL_T; // Real or floating-point type (NB not supported in earlier XC compilers)
typedef S32_T SAMP_CHAN_T; // Full precision sample type on channel (e.g. 32-bit)
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

#define DELAY_SIZE 3 // Size of delay-line (in samples)

typedef struct FIX_POINT_TAG // Structure containing coefficients expressed as fixed point
{
	COEF_T mant; // Mantissa
	COEF_T exp; // Exponent expressed as power of 2 (Log2 of scaling factor)
	COEF_T half; // Half of scaling factor (used in rounding)
	COEF_T err; // rounding error
} FIX_POINT_S;

typedef struct BIQUAD_COEF_TAG // Structure containing BiQuad coefficients
{
	FIX_POINT_S b[DELAY_SIZE]; // Array of weighting for input samples (FIR taps)
	FIX_POINT_S a[DELAY_SIZE]; // Array of weighting for output results (IIR taps)
} BIQUAD_COEF_S;

typedef struct BIQUAD_CHAN_TAG // Structure containing intermediate data for one channel, updated every sample
{
	FILT_T iir[DELAY_SIZE]; // Array of previous IIR filter results
	FILT_T inp[DELAY_SIZE]; // Array of previous input samples
	FILT_T filt[DELAY_SIZE]; // Array of previous filterd output samples

	FILT_T inp_err; // Rounding error due to down-sampling input
	FILT_T iir_err; // Rounding error due to down-sampling IIR output
	FILT_T filt_err; // Rounding error due to down-sampling filtered output
} BIQUAD_CHAN_S;

typedef struct BIQUAD_TAG // Structure containing all biquad data
{
	BIQUAD_CHAN_S bq_chan_s[NUM_USB_CHAN_OUT]; // Array of structure containing intermediate data for each channel, updated every sample
	BIQUAD_COEF_S common_coefs_s;  // Structure containing BiQuad coefficients
} BIQUAD_S;

#endif // else __XC__

#endif /* _BIQUAD_SIMPLE_H_ */

// biquad_simple.h
