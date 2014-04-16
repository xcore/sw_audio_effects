/******************************************************************************\
 * Description: Definitions, types, and prototypes for vocoder.xc
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

#ifndef _VOCODER_H_
#define _VOCODER_H_

#include <print.h>
#include <assert.h>

#include "common_audio.h"
#include "use_locks.h"
#include "module_dsp_vocoder_conf.h"

#ifndef NUM_VOCODER_OUT_CHANS
#error Please define NUM_VOCODER_OUT_CHANS in module_dsp_vocoder_conf.h
#endif // NUM_VOCODER_OUT_CHANS

#ifndef NUM_VOCODER_SLAVE_BANDS
#error Please define NUM_VOCODER_SLAVE_BANDS in module_dsp_vocoder_conf.h
#endif // NUM_VOCODER_SLAVE_BANDS

#define DEF_SAMP_FREQ 48000 // Default Sample Frequency (In Hz)
#define DEF_QUAL_FACT 0.5 // Default Quality Factor (flat filter resonance)

/** Structure containing Vocoder parameters */
typedef struct VOCODER_PARAM_TAG //
{
	S32_T samp_freq; // Current sample frequency
	R32_T qual_fact; // Current quality factor (filter resonance)
} VOCODER_PARAM_S;

/******************************************************************************/
/** Performs vocoder processing.
 */
void use_vocoder( // Performs vocoder processing
	SAMP_CHAN_T inp_samps[],	// Buffer for input samples
	SAMP_CHAN_T out_samps[],	// Buffer containing output samples
	S32_T slave_id // Unique slave Identifier
);
/******************************************************************************/

#ifdef __XC__
// XC File

/******************************************************************************/
/** Configure vocoder parameters. NB Must be called before use_vocoder.
 * \param cur_param_s // Reference to structure containing vocoder parameters
 * \param slave_id // Unique slave identifier
 */
void config_vocoder( // Configure vocoder parameters. NB Must be called before use_reverd
	VOCODER_PARAM_S &cur_param_s, // Reference to structure containing vocoder parameters
	S32_T slave_id // Unique slave identifier
);
/******************************************************************************/

#else //if __XC__
// 'C' File

/******************************************************************************/
void config_vocoder( // Configure vocoder parameters. NB Must be called before use_vocoder
	VOCODER_PARAM_S * vocoder_param_ps, // Pointer to structure containing vocoder parameters
	S32_T slave_id // Unique slave identifier
);
/******************************************************************************/

typedef struct VOCODER_TAG // Structure containing all vocoder data
{
	S32_T init_done;	// Flag indicating Vocoder is initialised
	S32_T params_set; // Flag indicating Parameters are set
} VOCODER_S;

#endif // else __XC__

#endif // _VOCODER_H_
/******************************************************************************/
// vocoder.h
