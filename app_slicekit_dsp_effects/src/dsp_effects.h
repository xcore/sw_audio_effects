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

/** Different Ouput Signal Types */
typedef enum OUT_SIGNAL_TAG //
{
  INPUT = 0,	// Input Signal
  BIQUAD,			// BiQuad filtered Signal
  REVERB,			// Long-Reverb Signal
  NUM_OUT_SIGNALS	// Handy Value!-)
} OUT_SIGNAL_ENUM;

/** Different Dry/Effect States */
typedef enum DRY_STATE_ETAG //
{
	FX_ON = 0, // Current Effect is Active
	DRY_ON, // Dry Signal is active
  NUM_DRY_STATES	// Handy Value!-)
} DRY_STATE_ENUM;

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
	STR_S fx_names[NUM_OUT_SIGNALS]; // Array of effects names
	DRY_STATE_ENUM dry_state;	// Dry/Effect processing state
	S32_T fade_on; // Flag set when doing cross-fade
	OUT_SIGNAL_ENUM old_sig; // Old output signal
	OUT_SIGNAL_ENUM cur_sig; // Currently Active output signal
	OUT_SIGNAL_ENUM new_effect; // Newly requested DSP effect
	S32_T pending; // Flag set when New Effect request is pending
	S32_T fx_len; // time spent in effect state (to ~8 secs)
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
