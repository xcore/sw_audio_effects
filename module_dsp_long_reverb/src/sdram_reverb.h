/******************************************************************************\
 * Header:  sdram_reverb
 * File:    sdram_reverb.h
 * Author: Mark Beaumont
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
#include "app_conf.h"
#include "biquad_simple.h"
#include "sdram_delay.h"
#include "non_linear_gain.h"

#ifndef NUM_REVERB_CHANS 
	#error Define. NUM_REVERB_CHANS in app_conf.h
#endif // NUM_REVERB_CHANS

#define NUM_REV_TAPS 4

#define DEF_ROOM_SIZE 1 // Default room-size for reverb

#define MIX_BITS 8 // Bit-Shift used in Mixing
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

#define DEF_DRY_LVL MIX_3DIV4 // Default Volume Level Of Dry Signal
#define DEF_FX_LVL MIX_3DIV4 // Default Volume Level Of Effect Signal
#define DEF_ATTN_MIX MIX_DIV8 // Default Attenuation. NB Below 1/8 can cause uncontrolled feedback
#define DEF_CROSS_MIX MIX_DIV4 // Default CrossTalk Mix

typedef struct MIX_PARAM_TAG // Structure containing BiQuad parameters
{
	S32_T dry_lvl; // 0:Off <--> MAX_MIX:Full
	S32_T fx_lvl; // 0:Off<--> MAX_MIX:Full
	S32_T attn_mix; // 0:No Attenuation <--> MAX_MIX:No Feedback
	S32_T cross_mix; // 0:No Reverb Crosstalk <--> MAX_MIX:Swap Reverb channels
} MIX_PARAM_S;

typedef struct REVERB_PARAM_TAG // Structure containing BiQuad parameters
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
void config_sdram_reverb( // Configure reverb parameters. NB Must be called before use_reverd
	REVERB_PARAM_S &cur_param_s // Reference to structure containing reverb parameters
);
/******************************************************************************/

/******************************************************************************/
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
void config_sdram_reverb( // Configure reverb parameters. NB Must be called before use_reverb
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
#define TAP_BITS 10 // Bit-shift used to scale delay tap values
#define MAX_TAP (1 << TAP_BITS) // Maximum Tap value

#define SCALE_REV_BITS (SOS_BITS + TAP_BITS) // Total No. of scaling bits
#define HALF_REV_SCALE ((S64_T)1 << (SCALE_REV_BITS - 1)) // Half scaling factor. Used for rounding

#define TAP_0 263
#define TAP_1 431
#define TAP_2 631
#define TAP_3 MAX_TAP // NB Maximum tap value must be 'Power of 2' (see above)

typedef struct REVERB_TAG // Structure containing all reverb data
{
	U32_T tap_ratios[NUM_REV_TAPS]; // delay tap ratios (NB These values are scaled by room size)
	MIX_PARAM_S * mix_lvls_ps; // Pointer to structure containing mix-levels
	S32_T init_done;	// Flag indicating Reverb is initialised
	S32_T params_set; // Flag indicating Parameters are set
} REVERB_S;

#endif // else __XC__

#endif // _SDRAM_REVERB_H_
/******************************************************************************/
// sdram_reverb.h
