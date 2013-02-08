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
#include <print.h>
#include "types64bit.h"
#include "app_global.h"
#include "common_utils.h"
#include "gp_io.h"
#include "sdram_io.h"
#include "sdram_reverb.h"

/** Different Processing States */
typedef enum EFFECT_TAG //
{
  REVERB = 0,	// Long-Reverb
  BIQUAD,			// BiQuad Only
  NUM_EFFECTS	// Handy Value!-)
} EFFECT_ENUM;

/** Structure containing BiQuad parameters */
typedef struct DSP_EFFECT_TAG // 
{
	PROC_STATE_ENUM fade_state;	// Initialise cross-fade processing state
	EFFECT_ENUM cur_effect; // Active DSP effect
	S32_T fx_len; // time spent in effect state (to ~8 secs)
	S32_T dry_len; // time spent in dry state (~8 secs)
	S32_T samp_cnt;// Sample counter
} DSP_EFFECT_S;

/*****************************************************************************/
void dsp_effects( // Controls audio stream processing for reverb application using dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O coar (bi-directional)
	streaming chanend c_dsp_eq, // Channel connecting to Equalisation coar (bi-directional)
	streaming chanend c_dsp_gain, // Channel connecting to Loudness coar (bi-directional)
  chanend c_dsp_sdram, // DSP end of channel between DSP coar and SDRAM coar (bi-directional)
	chanend c_dsp_gpio // DSP end of channel between GPIO and DSP coars
);
/*****************************************************************************/

#endif // _DSP_EFFECTS_H_
/*****************************************************************************/
// dsp_effects.h
