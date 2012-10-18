/******************************************************************************\
 * Header:  reverb
 * File:    reverb.h
 *  
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

#include <print.h>
#include "module_dsp_short_reverb_conf.h"
#include "biquad_simple.h"
#include "delay_line.h"
#include "non_linear_gain.h"

#ifndef NUM_REVERB_CHANS 
#error Please define NUM_REVERB_CHANS in module_dsp_short_reverb_conf.h
#endif // NUM_REVERB_CHANS

#ifndef NUM_REVERB_TAPS 
#error Please define NUM_REVERB_TAPS in module_dsp_short_reverb_conf.h
#endif // NUM_REVERB_TAPS

/**  Default room-size for reverb (in meters): 1 */
#define DEF_ROOM_SIZE 1 //

/** Bit-Shift used in Mixing: 8 */
#define MIX_BITS 8 //

#define MAX_MIX (1 << MIX_BITS) // Maximum Mix Value

#define MIX_DIV2 (MAX_MIX >> 1) // 1/2 Maximum Mix Value
#define MIX_DIV4 (MIX_DIV2 >> 1) // 1/4 Maximum Mix Value
#define MIX_DIV8 (MIX_DIV4 >> 1) // 1/8 Maximum Mix Value
#define MIX_DIV16 (MIX_DIV8 >> 1) // 1/16 Maximum Mix Value
#define MIX_DIV32 (MIX_DIV16 >> 1) // 1/32 Maximum Mix Value
#define MIX_DIV64 (MIX_DIV32 >> 1) // 1/64 Maximum Mix Value
#define MIX_DIV128 (MIX_DIV64 >> 1) // 1/128 Maximum Mix Value
#define MIX_DIV256 (MIX_DIV128 >> 1) // 1/256 Maximum Mix Value

#define MIX_3DIV4 (MIX_DIV2 + MIX_DIV4) // 3/4 Maximum Mix Value

/** Default Volume Level Of Dry Signal: 192/256 */
#define DEF_DRY_LVL MIX_3DIV4 // 

/**  Default Volume Level Of Effect Signal: 192/256 */
#define DEF_FX_LVL MIX_3DIV4 //

/** Default Attenuation. NB Below 1/8 can cause uncontrolled feedback: 32/256 */
#define DEF_ATTN_MIX MIX_DIV8 // 

/**  Default CrossTalk Mix: 64/256 */
#define DEF_CROSS_MIX MIX_DIV4 //

/** Structure containing Reverb parameters*/
typedef struct MIX_PARAM_TAG // 
{
	S32_T dry_lvl; // 0:Off <--> MAX_MIX:Full
	S32_T fx_lvl; // 0:Off<--> MAX_MIX:Full
	S32_T attn_mix; // 0:No Attenuation <--> MAX_MIX:No Feedback
	S32_T cross_mix; // 0:No Reverb Crosstalk <--> MAX_MIX:Swap Reverb channels
} MIX_PARAM_S;

/** Structure containing BiQuad parameters */
typedef struct REVERB_PARAM_TAG // 
{
	MIX_PARAM_S mix_lvls; // Structure containing mix-levels
	S32_T room_size; // Room-size (in metres)
	S32_T sig_freq; // Significant filter frequency (e.g. low-pass cut-off )
	S32_T samp_freq; // Current sample frequency
	S32_T gain; // Reverb 'bring-up' gain
} REVERB_PARAM_S;

/******************************************************************************/
/** Performs reverb processing.
 * \param uneq_o_samps[] // Buffer for Unequalised output samples
 * \param rev_o_samps[]	// Buffer for output samples with Reverb added
 * \param out_samps[]	// Buffer for final Output samples 
 * \param inp_samps[]	// Buffer containing Dry input samples
 * \param equal_i_samps[] // Buffer containing Equalised input samples
 * \param amp_i_samps[]	// Buffer containing Amplified input samples
 */
void use_reverb( // Performs reverb processing
	S32_T uneq_o_samps[],	// Buffer for Unequalised output samples
	S32_T rev_o_samps[],	// Buffer for output samples with Reverb added
	S32_T out_samps[],	// Buffer for final Output samples 
	S32_T inp_samps[],	// Buffer containing Dry input samples
	S32_T equal_i_samps[],	// Buffer containing Equalised input samples
	S32_T amp_i_samps[]	// Buffer containing Amplified input samples
);
/******************************************************************************/

#ifdef __XC__
// XC File

/******************************************************************************/
/** Configure reverb parameters. NB Must be called before use_reverb.
 * \param cur_param_s // Reference to structure containing reverb parameters
 */
void config_reverb( // Configure reverb parameters. NB Must be called before use_reverd
	REVERB_PARAM_S &cur_param_s // Reference to structure containing reverb parameters
);
/******************************************************************************/

#else //if __XC__
// 'C' File

/******************************************************************************/
void config_reverb( // Configure reverb parameters. NB Must be called before use_reverb
	REVERB_PARAM_S * cur_param_ps // Pointer to structure containing reverb parameters
);
/******************************************************************************/

// Inverse of Speed-Of-Sound represented as (e.g. 13981/2^22)
#define INV_SOS 13981 // Inverse of Speed-Of-Sound in Fixed point format (e.g. 13981/2^SOS_BITS)
#define SOS_BITS 22 // Bit-shift used to scale speed-of-sound

// These filter taps are based on a room 619x1000. i.e. (~GoldenRatio) ...
#define TAP_BITS 10 // Bit-shift used to scale delay tap values
#define MAX_TAP (1 << TAP_BITS) // Maximum Tap value

#define SCALE_REV_BITS (SOS_BITS + TAP_BITS) // Total No. of scaling bits
#define HALF_REV_SCALE ((S64_T)1 << (SCALE_REV_BITS - 1)) // Half scaling factor. Used for rounding

#define TAP_0 263
#define TAP_1 431
#define TAP_2 631
#define TAP_3 MAX_TAP // NB Maximum tap value must be 'Power of 2' (see above)

typedef struct TAP_RATIOS_TAG // Structure containing delay tap ratios
{
	U32_T taps[NUM_REVERB_TAPS]; // array of delay tap ratios (NB These values are scaled by room size)
} TAP_RATIOS_S;

typedef struct REVERB_TAG // Structure containing all reverb data
{
	TAP_RATIOS_S ratios; // Structure containing delay tap ratios (NB These values are scaled by room size)
	MIX_PARAM_S * mix_lvls_ps; // Pointer to structure containing mix-levels
	S32_T init_done;	// Flag indicating Reverb is initialised
	S32_T params_set; // Flag indicating Parameters are set
} REVERB_S;

#endif // else __XC__

#endif // _REVERB_H_
/******************************************************************************/
// reverb.h
