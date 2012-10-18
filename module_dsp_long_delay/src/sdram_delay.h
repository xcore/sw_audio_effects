/******************************************************************************\
 * File:	sdram_delay.h
 *  
 * Description: Definitions, types, and prototypes for module sdram_delay
 *
 * Version: 3v3rc1
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

#ifndef _SDRAM_DELAY_H_
#define _SDRAM_DELAY_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "types64bit.h"
#include "module_dsp_long_delay_conf.h"
#include "common_audio.h"
#include "common_utils.h"

#ifndef NUM_DELAY_CHANS 
#error Please define NUM_DELAY_CHANS in module_dsp_long_delay_conf.h
#endif // NUM_DELAY_CHANS

#ifndef NUM_DELAY_TAPS 
#error Please define NUM_DELAY_TAPS in module_dsp_long_delay_conf.h
#endif // NUM_DELAY_TAPS

/** 8 MByte Memory available to configure delay buffers (in samples), adjust to suit platform: (2 << 20) */
#define MEM_SAMPS (2 << 20) //

/** Size of channel delay buffer (in samples): (MEM_SAMPS / NUM_DELAY_CHANS) */
#define DELAY_SAMPS (MEM_SAMPS / NUM_DELAY_CHANS)// 

// SDRAM parameters
#define SD_CELL_BITS 1 // number of bits used to represent cell-size
#define SD_COL_BITS 8 // number of bits used for column addressing
#define SD_ROW_BITS 12 // number of bits used for row addressing
#define SD_BANK_BITS 2 // number of bits used for bank addressing

#define SD_NUM_COLS (1 << SD_COL_BITS) // Number of columns in SDRAM
#define SD_NUM_ROWS (1 << SD_ROW_BITS) // Number of rows in SDRAM
#define SD_NUM_BANKS (1 << SD_BANK_BITS) // Number of banks in SDRAM

#define SD_COL_BYTS (1 << SD_CELL_BITS) // Number of bytes in a column (2 byte, e.g 16-bit cells)
#define SD_ROW_BYTS (SD_COL_BYTS << SD_COL_BITS) // // Number of bytes in a row (512)
#define SD_BANK_BYTS (SD_ROW_BYTS << SD_ROW_BITS) // // Number of bytes in a bank (2M)
#define SD_ROW_WRDS (SD_ROW_BYTS >> 2) // No. of Words in SDRAM row

// Buffer parameters
#define AUD_BUF_SAMPS (SD_ROW_WRDS / NUM_DELAY_CHANS)	// Match burst-length of SDRAM.

// Delay-line parameters

/** Default Sample Frequency (In Hz) 48000 */
#define DEF_SAMP_FREQ 48000 // 


/** Structure containing delay parameters */
typedef struct DELAY_PARAM_TAG // 
{
	S32_T us_delays[NUM_DELAY_TAPS]; // Set of delays in micro-seconds
	S32_T num; // No. of delay taps in use
	S32_T freq; // Sample frequency
	S32_T dbg; // Debug flag
} DELAY_PARAM_S;

typedef struct CHAN_SET_TAG // Structure for a set of audio samples (one from each channel)
{
	S32_T samps[NUM_DELAY_CHANS]; // set of audio samples (one from each channel)
} CHAN_SET_S;

typedef struct CNTRL_BUF_TAG // Structure containing data to control double-buffering
{
	U32_T buf_adr; // Address of buffer
	U32_T mem_adr; // Address of SDRAM memory
	U32_T wrd_siz; // Size of buffer in 32-bit words
	U32_T do_buf; // Flag indicating buffer ready for transfer
} CNTRL_BUF_S;

typedef struct CNTRL_SDRAM_TAG // Structure containing data to control sdram buffering
{
	CHAN_SET_S delay_sets[NUM_DELAY_TAPS]; // Array of structures containing delayed output sample-sets
	CHAN_SET_S src_set;	// Structure containing source audio sample-set (one sample for each channel)
	CNTRL_BUF_S reads[NUM_DELAY_TAPS];	// Array of control data for read buffers
	CNTRL_BUF_S write; 						// Control data for write buffer
} CNTRL_SDRAM_S;

#ifdef __XC__
// XC File
#include <print.h>

/******************************************************************************/
/** Configure sdram_delay parameters. Must be called before use_sdram_delay
 * \param cur_param_s // Reference to structure containing delay-line parameters
 */
void config_sdram_delay( // Configure sdram_delay parameters. NB Must be called before use_sdram_delay
	DELAY_PARAM_S &cur_param_s // Reference to structure containing delay-line parameters
);
/******************************************************************************/
/** Exercise sdram_delay for one input sample. Must have previously called config_sdram_delay
 * \param sdram_gs // Reference to structure containing data to control SDRAM buffering
 */
void use_sdram_delay( // exercise sdram_delay for one input sample. NB Must have previously called config_sdram_delay
	CNTRL_SDRAM_S &sdram_gs // Reference to structure containing data to control SDRAM buffering
);
/******************************************************************************/

#else //if __XC__
// 'C' File

/******************************************************************************/
void config_sdram_delay( // Configure sdram_delay parameters. NB Must be called before use_sdram_delay
	DELAY_PARAM_S * cur_param_ps // Pointer to structure containing delay-line parameters
);
/******************************************************************************/
void use_sdram_delay( // exercise sdram_delay for one input sample. NB Must have previously called config_sdram_delay
	CNTRL_SDRAM_S * sdram_ps // Pointer to structure containing data to control SDRAM buffering
);
/******************************************************************************/

#define MAX_DELAY (DELAY_SAMPS - 1) // Max index into delay buffer

#define MIN_AUDIO_FREQ 20 // Minimum Audio Frequency (In Hz)

#define BUF_MASK (AUD_BUF_SAMPS - 1)  // Mask used to extract buffer offset from delay offset

#define TWIN_SAMPS (AUD_BUF_SAMPS << 1) // Size of double-buffer

typedef struct AUD_BUF_TAG // Structure for audio buffer
{
	CHAN_SET_S sets[AUD_BUF_SAMPS]; // Array of set-of-samples
} AUD_BUF_S;

typedef struct TWIN_BUF_TAG // Structure for double-buffering of audio samples
{
	AUD_BUF_S bufs[2]; // Array of 2 audio buffers
	SAMP_CHAN_T off; // offset into Delay-line
	S32_T id;					// Id of currently active buffer
	S32_T read_active; // read-active flag (NB Unused for writes)
} TWIN_BUF_S;

typedef struct DELAY_TAG // Structure containing all delay data
{
	TWIN_BUF_S out_bufs[NUM_DELAY_TAPS]; // Array of structures containing Output Buffer data
	TWIN_BUF_S inp_bufs; // Structure containing Input Buffer data
	CNTRL_SDRAM_S * cntrl_p; // Pointer to structure containing data to control SDRAM buffering
	S32_T tap_num;		// Number of delay taps in use
	S32_T init_done;	// Flag indicating Delay is initialised
	S32_T params_set; // Flag indicating Parameters are set
	S32_T dbg; // Debug flag
} DELAY_S;

#endif // else __XC__

#endif /* _SDRAM_DELAY_H_ */

// sdram_delay.h
