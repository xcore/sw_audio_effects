/*****************************************************************************\
 * Header:  dsp_effects
 * File:    dsp_effects.h
 *  
 * Description: Definitions, types, and prototypes for dsp_effects.xc
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
\*****************************************************************************/

#ifndef _DSP_EFFECTS_H_
#define _DSP_EFFECTS_H_

#include <xs1.h>
#include <safestring.h>
#include <print.h>

#include "types64bit.h"
#include "app_global.h"
#include "common_utils.h"
#include "gp_io.h"
#include "sdram_io.h"
#include "sdram_reverb.h"

#ifdef USE_XSCOPE
#include <xscope.h>
#endif // ifdef USE_XSCOPE

#define STR_LEN 128 // String length

/** Different Effect */
typedef enum EFFECT_TAG //
{
  NO_FX = 0,	// No Effect 
  REVERB,			// Long-Reverb
  BIQUAD,			// BiQuad Only
  NUM_EFFECTS	// Handy Value!-)
} EFFECT_ENUM;

/** Different Cross-fade States */
typedef enum FADE_STATE_ETAG //
{
  EFFECT_F = 0,			// DSP Effect On
  FX2DRY_F,					// Fade Effect to Dry
  DRY_ONLY_F,				// No Effect
  DRY2FX_F,					// Fade Dry to Effect
  START_F,					// Start-up mode
  BQ2REV_F,					// Fade BiQuad to Reverb
  REV2BQ_F,					// Fade Reverb to BiQuad 
  NUM_FADE_STATES	// Handy Value!-)
} FADE_STATE_ENUM;

// String type
typedef struct STR_TAG // 
{
	char str[STR_LEN]; // Array of string characters
} STR_S;

/** Structure containing BiQuad parameters */
typedef struct DSP_EFFECT_TAG // 
{
	REVERB_PARAM_S reverb_params;	// Reverb Configuration parameters
	BIQUAD_PARAM_S biquad_params_rvrb;	// BiQuad Configuration parameters when used in reverb
	BIQUAD_PARAM_S biquad_params_solo;	// BiQuad Configuration parameters when used in isolation
	GAIN_PARAM_S gain_params;			// Loudness Configuration parameters
	STR_S fx_names[NUM_EFFECTS]; // Array of effects names
	FADE_STATE_ENUM fade_state;	// Initialise cross-fade processing state
	S32_T fade_on; // Flag set when doing cross-fade
	EFFECT_ENUM new_effect; // New DSP effect request
	EFFECT_ENUM cur_effect; // Currently active DSP effect
	S32_T fx_len; // time spent in effect state (to ~8 secs)
	S32_T dry_len; // time spent in dry state (~8 secs)
	S32_T samp_cnt;// Sample counter
} DSP_EFFECT_S;

/*****************************************************************************/
void dsp_effects( // Controls audio stream processing for reverb application using dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O coar (bi-directional)
	streaming chanend c_dsp_eq_arr[NUM_APP_BIQUADS], // Array of channels connecting to Equalisation coars (bi-directional)
	streaming chanend c_dsp_gain, // Channel connecting to Loudness coar (bi-directional)
  chanend c_dsp_sdram, // DSP end of channel between DSP coar and SDRAM coar (bi-directional)
	chanend c_dsp_gpio // DSP end of channel between GPIO and DSP coars
);
/*****************************************************************************/

#endif // _DSP_EFFECTS_H_
/*****************************************************************************/
// dsp_effects.h
