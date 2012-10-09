/******************************************************************************\
 * Header:  sdram_io
 * File:    sdram_io.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for sdram_io.xc
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

#ifndef _SDRAM_IO_H_
#define _SDRAM_IO_H_

#include <platform.h>
#include "app_global.h"
#include "sdram.h"
#include "sdram_memory_mapper.h"

/******************************************************************************/
void read_buffer_from_sdram( // Reads a buffer of data from SDRAM
  chanend c_sdram, // SDRAM end of channel between DSP thread and SDRAM thread (bi-directional)
	unsigned mem_adr, // Virtual Memory Address
	unsigned buf_adr, // Buffer Address
	unsigned wrd_siz // No. Of words to read
);
/******************************************************************************/
void write_buffer_to_sdram( // Writes a buffer of data to SDRAM
  chanend c_sdram, // SDRAM end of channel between DSP thread and SDRAM thread (bi-directional)
	unsigned mem_adr, // Virtual Memory Address
	unsigned buf_adr, // Buffer Address
	unsigned wrd_siz // No. Of words to write
);
/*****************************************************************************/
int sdram_io( // SDRAM I/O Thread
	chanend c_sdram // SDRAM end of channel between SDRAM and DSP threads
);
/******************************************************************************/

#endif // _SDRAM_IO_H_
/******************************************************************************/
// sdram_io.h
