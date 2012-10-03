/******************************************************************************\
 * File:	sdram_io.xc
 * Author: Mark Beaumont
 * Description: SDRAM I/O Thread
 * for L2 Slice Kit Core Board with Audio Slice 1v0 
 * Note: This application expects a memory Slice (1v0) to be connected to a Socket on core MEM_CORE
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

#include "sdram_io.h"

// Global data structure for sdram memory manager (sc_sdram_burst/module_sdram_memory_manager)
on stdcore[MEM_CORE] : struct sdram_ports sdram_ports = 
{ 
	PORT_SEL, 
	PORT_CAS, 
	PORT_RAS, 
	PORT_WE, 
	PORT_CLK, 
	XS1_CLKBLK_1
};

static intptr_t buf_p = 0; // Pointer to active buffer

/******************************************************************************/
void read_buffer_from_sdram( // Reads a buffer of data from SDRAM
  chanend c_sdram, // SDRAM end of channel between DSP thread and SDRAM thread (bi-directional)
	unsigned mem_adr, // Virtual Memory Address
	unsigned buf_adr, // Buffer Address
	unsigned wrd_siz // No. Of words to read
)
{
	// Check if a transfer has been done
	if (buf_p)
	{
		mm_wait_until_idle_p( c_sdram ,buf_p ); // Wait till previous transfer complete
	} // if (buf_p)

	buf_p = (intptr_t)buf_adr; // update active buffer pointer
	mm_read_words_p( c_sdram ,mem_adr ,wrd_siz ,buf_p );

} // read_buffer_from_sdram
/******************************************************************************/
void write_buffer_to_sdram( // Writes a buffer of data to SDRAM
  chanend c_sdram, // SDRAM end of channel between DSP thread and SDRAM thread (bi-directional)
	unsigned mem_adr, // Virtual Memory Address
	unsigned buf_adr, // Buffer Address
	unsigned wrd_siz // No. Of words to write
)
{
	// Check if a transfer has been done
	if (buf_p)
	{
		mm_wait_until_idle_p( c_sdram ,buf_p ); // Wait till previous transfer complete
	} // if (buf_p)

	buf_p = (intptr_t)buf_adr; // update active buffer pointer
	mm_write_words_p( c_sdram ,mem_adr ,wrd_siz ,buf_p );

} // write_buffer_to_sdram
/*****************************************************************************/
int sdram_io( // SDRAM I/O Thread
	chanend c_sdram // SDRAM end of channel between SDRAM and DSP threads
)
{
  sdram_server( c_sdram, sdram_ports ); // Call SDRAM master loop

	return 0;
} // sdram_io
/*****************************************************************************/
// sdram_io.xc
