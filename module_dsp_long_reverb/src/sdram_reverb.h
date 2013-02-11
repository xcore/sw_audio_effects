/******************************************************************************\
 * Header:  sdram_reverb
 * File:    sdram_reverb.h
 *  
 * Description: Definitions, types, and prototypes for sdram_reverb.xc
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

#ifndef _SDRAM_REVERB_H_
#define _SDRAM_REVERB_H_

#include <print.h>
#include "module_dsp_long_reverb_conf.h"
#include "biquad_simple.h"
#include "sdram_delay.h"
#include "non_linear_gain.h"

#ifndef NUM_REVERB_CHANS 
#error Please define NUM_REVERB_CHANS in module_dsp_long_reverb_conf.h
#endif // NUM_REVERB_CHANS

#ifndef NUM_REVERB_TAPS 
#error Please define NUM_REVERB_TAPS in module_dsp_long_reverb_conf.h
#endif // NUM_REVERB_TAPS

/**  Default room-size for reverb (in meters): 1 */
#define DEF_ROOM_SIZE 100 //

/**  Default bring-up gain for reverb */
#define DEF_REVERB_GAIN 8 //

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

/** Default Feedback level, Above 1 will cause uncontrolled feedback: 256/256 */
#define DEF_FB_LVL MIX_DIV2 // 

/**  Default CrossTalk Mix: 128/256 */
#define DEF_CROSS_MIX MIX_DIV2 //

/** Structure containing Reverb parameters*/
typedef struct MIX_PARAM_TAG // 
{
	S32_T dry_lvl; // 0:Off <--> MAX_MIX:Full
	S32_T fx_lvl; // 0:Off<--> MAX_MIX:Full
	S32_T fb_lvl; // 0:No Feedback <--> MAX_MIX:Full Feedback
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

#ifdef __XC__
// XC File

/******************************************************************************/
/** Configure reverb parameters. Note well, Must be called before use_reverb.
 * \param cur_param_s // Reference to structure containing reverb parameters
 */
void config_sdram_reverb( // Configure reverb parameters. Note well, Must be called before use_reverd
	REVERB_PARAM_S &cur_param_s // Reference to structure containing reverb parameters
);
/******************************************************************************/

/******************************************************************************/
/** Performs reverb processing.
 * \param &cntrl_gs // Reference to structure containing data to control SDRAM buffering
 * \param uneq_o_set_s // Buffer for Unequalised output samples
 * \param rev_o_set_s	// Buffer for output samples with Reverb added
 * \param out_set_s	// Buffer for final Output samples 
 * \param equal_i_set_s // Buffer containing Equalised input samples
 * \param ampli_i_set_s	// Buffer containing Amplified input samples
 */
void use_sdram_reverb( // Controls audio stream processing for reverb application using dsp functions
	CNTRL_SDRAM_S &cntrl_gs, // Reference to structure containing data to control SDRAM buffering
	CHAN_SET_S &uneq_o_set_s,	// Reference to structure containing Unequalised audio sample-set
	CHAN_SET_S &rev_o_set_s,	// Reference to structure containing output audio sample-set
	CHAN_SET_S &out_set_s,	// Reference to structure containing output audio sample-set
	CHAN_SET_S &equal_i_set_s,	// Reference to structure containing Equalised audio sample-set
	CHAN_SET_S &ampli_i_set_s	// Reference to structure containing Amplified audio sample-set
);
/******************************************************************************/

#else //if __XC__
// 'C' File

/******************************************************************************/
void config_sdram_reverb( // Configure reverb parameters. Note well, Must be called before use_reverb
	REVERB_PARAM_S * cur_param_ps // Pointer to structure containing reverb parameters
);
/******************************************************************************/

/******************************************************************************/
void use_sdram_reverb( // Controls audio stream processing for reverb application using dsp functions
	CNTRL_SDRAM_S * cntrl_ps, // Pointer to structure containing data to control SDRAM buffering
	CHAN_SET_S * uneq_o_set_ps,	// Pointer to structure containing Unequalised audio sample-set
	CHAN_SET_S * rev_o_set_ps,	// Pointer to structure containing output audio sample-set
	CHAN_SET_S * out_set_ps,	// Pointer to structure containing output audio sample-set
	CHAN_SET_S * equal_i_set_ps,	// Pointer to structure containing Equalised audio sample-set
	CHAN_SET_S * ampli_i_set_ps	// Pointer to structure containing Amplified audio sample-set
);
/******************************************************************************/

// Inverse of Speed-Of-Sound represented as (e.g. 13981/2^22)
#define INV_SOS 13981 // Inverse of Speed-Of-Sound in Fixed point format (e.g. 13981/2^SOS_BITS)
#define SOS_BITS 22 // Bit-shift used to scale speed-of-sound

// These filter taps are based on a room 619x1000. i.e. (~GoldenRatio) ...
#define TAP_BITS 8 // Bit-shift used to scale delay tap values
#define MAX_TAP (1 << TAP_BITS) // Maximum Tap value

#define WEIGHT_BITS 10 // Bit-shift used to scale delay-tap weighting values
#define MAX_WEIGHT (1 << WEIGHT_BITS) // Maximum WEIGHT value

#define REV_TAP_BITS 4 // This should be approx Log2(NUM_REVERB_TAPS)
#define ATTN_BITS (WEIGHT_BITS - REV_TAP_BITS + 3) // 9. Bit-shift used to attenuate delay-tap weighting values. Currently set to ~1/8 input volume
#define HALF_ATTN ((S64_T)1 << (ATTN_BITS - 1)) // Half attenuation value. Used for rounding

#define SCALE_FB_BITS (WEIGHT_BITS + MIX_BITS) // Total No. of feedback scaling bits
#define HALF_FB_SCALE ((S64_T)1 << (SCALE_FB_BITS - 1)) // Half feedback scaling factor. Used for rounding

#define SCALE_XMIX_BITS (ATTN_BITS + MIX_BITS) // Total No. of cross-mix scaling bits
#define HALF_XMIX_SCALE ((S64_T)1 << (SCALE_XMIX_BITS - 1)) // Half cross-mix scaling factor. Used for rounding

#define SCALE_REV_BITS (SOS_BITS + TAP_BITS) // Total No. of scaling bits
#define HALF_REV_SCALE ((S64_T)1 << (SCALE_REV_BITS - 1)) // Half scaling factor. Used for rounding

#define TAP_00 157
#define TAP_01 257 // NB Approx. MAX_TAP value
#define TAP_02 317
#define TAP_03 409 // Currently Unused
#define TAP_04 419
#define TAP_05 479
#define TAP_06 509
#define TAP_07 571
#define TAP_08 631
#define TAP_09 673
#define TAP_10 733
#define TAP_11 769
#define TAP_12 827 // Currently Unused
#define TAP_13 829
#define TAP_14 929
#define TAP_15 1021

// WARNING: If more taps are used, weights have to be recalculated. Must sum to MAX_WEIGHT
#define WGHT_00 92
#define WGHT_01 90 // NB Approx. MAX_TAP value
#define WGHT_02 84
#define WGHT_03 0		// Currently Unused
#define WGHT_04 82
#define WGHT_05 77
#define WGHT_06 78
#define WGHT_07 74
#define WGHT_08 69
#define WGHT_09 70
#define WGHT_10 65
#define WGHT_11 66
#define WGHT_12 0   // Currently Unused
#define WGHT_13 61
#define WGHT_14 59
#define WGHT_15 57

typedef struct TAP_DATA_TAG // Structure containing delay tap ratios
{
	U32_T ratios[NUM_REVERB_TAPS]; // array of delay tap ratios (NB These values are scaled by room size)
	U32_T weights[NUM_REVERB_TAPS]; // array of weights for each delay-tap (must sum to MAX_WEIGHT)
} TAP_DATA_S;

typedef struct REVERB_TAG // Structure containing all reverb data
{
	TAP_DATA_S tap_data_s; // Structure containing delay tap data
	MIX_PARAM_S * mix_lvls_ps; // Pointer to structure containing mix-levels
	S32_T init_done;	// Flag indicating Reverb is initialised
	S32_T params_set; // Flag indicating Parameters are set
	S32_T same_mix; // Inverse cross-mix value (MAX_MIX:No Reverb Crosstalk <--> 0:Swap Reverb channels)
} REVERB_S;

#endif // else __XC__

#endif // _SDRAM_REVERB_H_
/******************************************************************************/
// sdram_reverb.h
