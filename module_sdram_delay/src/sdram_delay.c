/******************************************************************************\
 * File:	sdram_delay.c
 * Author: Mark Beaumont
 * Description: Functions for Delay-Line(echo)
 *
 * Version: 
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

#include "sdram_delay.h"

// Structure containing all delay data (NB Make global to avoid using malloc)
static DELAY_S delay_gs = { .init_done = 0, .params_set = 0 }; // Clear initialisation flags

/******************************************************************************/
void update_common_delays( // Update delays for each channel
	DELAY_S * delay_ps, // Pointer to structure containing all delay data
	DELAY_PARAM_S * cur_param_ps // Pointer to structure containing delay-line parameters
)
/* For a zero-latency Delay-line, if an sample-delay of X samples were required,
 * the memory offest to read the output (delayed) sample would be X taps less
 * than the memory offset required to store int input sample.
 *
 * However, due to double-buffering there is a minimum delay equivalent to reading 2 buffers.
 * Therefore, the output sample must be accessed 2 buffers early. (TWIN_SAMPS samples earlier).
 */
{
	S32_T num_taps = cur_param_ps->num; // Get Number of taps to calculate 
	S32_T tap_cnt; // delay measured in samples
	U32_T req_delay; // current requested delay measured in samples
	U32_T delay_off; // delay offet between input and output samples to achieve the requested_delay


	delay_ps->tap_num = num_taps; // Update number of delay taps in use

	// Calculate delays in samples ...

	// Loop through all delay taps.
	for (tap_cnt = 0; tap_cnt < num_taps; tap_cnt++)
	{
		// Calculate current requested delay in samples
		req_delay = ((S64_T)cur_param_ps->freq * (S64_T)cur_param_ps->us_delays[tap_cnt] + (S64_T)500000) / (S64_T)1000000;

		// Do checks
		assert(DELAY_SAMPS > req_delay); // Check buffer is large enough
		assert(TWIN_SAMPS <= req_delay); // Check delay is larger than twin-buffer (theoretical minimum delay)

		delay_off = req_delay - TWIN_SAMPS; // compute delay offset to guarantee requested delay

		// Update output offset. NB Add in extra DELAY_SAMPS to prevent negative offsets.
		delay_ps->out_bufs[tap_cnt].off = (delay_ps->inp_bufs.off + (DELAY_SAMPS - delay_off)) % DELAY_SAMPS;

// printstr("OF= "); printintln( (int)(delay_ps->out_bufs[tap_cnt].off - TWIN_SAMPS)); // MB~
	} // for tap_cnt

} // update_common_delays
/******************************************************************************/
void init_buffers( // Initialise sample buffers
	TWIN_BUF_S * twin_ps // Pointer to twin-buffer structure
)
{
	memset(&(twin_ps->bufs[0]) ,0 ,sizeof(AUD_BUF_S) ); // Clear 1st buffer of pair
	memset(&(twin_ps->bufs[1]) ,0 ,sizeof(AUD_BUF_S) ); // Clear 2nd buffer of pair

	/* In order to prevent uninitialised memory being read (and played), a read-active flag is maintained.
	 * (NB This is unused for write-buffers)
	 * The write-slot offset is initialised to the end of the memory-range,
   * so that the first write occurs at the start of the memory-range (due to wrap-around)
   * The read-slot offsets for each delay-tap are monitored.
	 * When they wrap-around to the memory-start, the read-active flag is switched on.
	 */

	twin_ps->off = MAX_DELAY; // Set write offset to largest used delay-slot. (NB Overwritten for reads in update_common_delays)
	twin_ps->read_active = 0; // Clear read-active flag. NB Un-used for writes
	twin_ps->id = 0; // Start by filling 1st buffer 
} // init_buffers
/******************************************************************************/
void init_sdram_delay( // Initialise delay data
	DELAY_S * delay_ps // Pointer to structure containing Delay data
)
{
	S32_T tap_cnt; // tap counter


	init_buffers( &(delay_ps->inp_bufs) ); // Clear input buffers

	// Loop thorugh output buffers
	for (tap_cnt=0; tap_cnt<MAX_TAPS; tap_cnt++)
	{
		init_buffers( &(delay_ps->out_bufs[tap_cnt]) ); // Clear current Output buffer
	} // for tap_cnt

} // init_sdram_delay
/******************************************************************************/
void config_sdram_delay( // Configure sdram_delay parameters. NB Must be called before use_sdram_delay
	DELAY_PARAM_S * cur_param_ps // Pointer to structure containing delay-line parameters
)
{
	// Check if Delay-Line initialised
	if (0 == delay_gs.init_done)
	{ // Initialse Delay-line
		init_sdram_delay( &(delay_gs) );

		delay_gs.init_done = 1; // Signal Delay-line initialised
	} // if (0 == delay_gs->init_done)

	delay_gs.dbg = cur_param_ps->dbg; // Get Debug status

	// Check for sensible frequency
	if (MIN_AUDIO_FREQ < cur_param_ps->freq)
	{
		// recalculate sample delay
	  update_common_delays( &(delay_gs) ,cur_param_ps );

		delay_gs.params_set = 1; // Signal Delay-line parameters configured
	} // if (MIN_AUDIO_FREQ < cur_param_ps->freq)

} // config_sdram_delay
/******************************************************************************/
void read_buffer( // Read buffer from SDRAM
	DELAY_S * delay_ps, // Pointer to structure containing all delay data
	CNTRL_BUF_S * sdram_read_p, // Pointer to structure controlling SDRAM read
	AUD_BUF_S * out_buf_p, // Pointer to output buffer
	S32_T sdram_off // delay-line offset for start of buffer
)
{
	CHAN_SET_S * dest_buf_p = &(out_buf_p->sets[0]); // Buffer destination pointer
	U32_T buf_size = sizeof(AUD_BUF_S); // Size of Buffer


	// Store buffer info. for reading from sdram
	sdram_read_p->mem_adr = (U32_T)( sdram_off * sizeof(CHAN_SET_S) ); // Store memory address
	sdram_read_p->buf_adr = (U32_T)dest_buf_p; // Store buffer address
	sdram_read_p->wrd_siz = (buf_size >> 2); // Store buffer length (in 32-bit words)
	sdram_read_p->do_buf = 1; // Signal that read-buffer is empty

} // read_buffer
/******************************************************************************/
void write_buffer( // Write buffer to SDRAM
	DELAY_S * delay_ps, // Pointer to structure containing all delay data
	CNTRL_BUF_S * sdram_write_p, // Pointer to structure controlling SDRAM write
	AUD_BUF_S * inp_buf_p, // Pointer to input buffer
	S32_T delay_off // delay-line offset for start of buffer
)
{
	CHAN_SET_S * src_buf_p = &(inp_buf_p->sets[0]); // Buffer source pointer
	U32_T buf_size = sizeof(AUD_BUF_S); // Size of Buffer


	// Store buffer info. for writing to sdram
	sdram_write_p->mem_adr = (U32_T)( delay_off * sizeof(CHAN_SET_S) ); // Store memory address
	sdram_write_p->buf_adr = (U32_T)src_buf_p; // Store buffer address
	sdram_write_p->wrd_siz = (buf_size >> 2); // Store buffer length (in 32-bit words)
	sdram_write_p->do_buf = 1; // Signal that write-buffer is full

} // write_buffer
/******************************************************************************/
void read_sample_set( // Read sample-set from buffer
	DELAY_S * delay_ps, // Pointer to structure containing all delay data
	CNTRL_BUF_S * sdram_read_p, // Pointer to structure controlling SDRAM read
	TWIN_BUF_S * out_buf_ps, // Pointer to structure containing output buffer data
	CHAN_SET_S * out_set_p // Pointer to delayed output sample-set
)
{
	AUD_BUF_S * buf_p = &(out_buf_ps->bufs[out_buf_ps->id]); // Pointer to active buffer
	S32_T buf_off = (out_buf_ps->off & BUF_MASK); // Mask out buffer offset

 
	// Read delayed output sample-set from buffer 
	*out_set_p = buf_p->sets[buf_off]; // Read delayed output sample-set from buffer 
	out_buf_ps->off++; // Increment Delay-line offset

	// Check for end-of-buffer
	if (0 == (out_buf_ps->off & BUF_MASK))
	{
		// Check for end-of-delay-line
		if (out_buf_ps->off >= DELAY_SAMPS)
		{
			out_buf_ps->off = 0;
			out_buf_ps->read_active = 1; // Start of delay-line, so switch on reads
		} // if (out_buf_ps->off >= DELAY_SAMPS)

if (delay_ps->dbg) printf("RB%1d: OF=%5d \n" ,out_buf_ps->id ,out_buf_ps->off ); //MB~

		// Check if safe to read data
		if (out_buf_ps->read_active)
		{
			read_buffer( delay_ps ,sdram_read_p ,buf_p ,out_buf_ps->off ); // Read new buffer from memory

			out_buf_ps->id = (1 - out_buf_ps->id); // Toggle twin-buffer id	
		} // if (out_buf_ps->read_active)

	} // if (out_buf_ps->off & BUF_MASK)

} // read_sample_set
/******************************************************************************/
void write_sample_set( // Write sample-set to buffer
	DELAY_S * delay_ps, // Pointer to structure containing all delay data
	CNTRL_BUF_S * sdram_write_p, // Pointer to structure controlling SDRAM write
	TWIN_BUF_S * inp_buf_ps, // Pointer to structure containing input buffer data
	CHAN_SET_S * inp_set_p // Pointer to input sample-set
)
{
	AUD_BUF_S * buf_p = &(inp_buf_ps->bufs[inp_buf_ps->id]); // Pointer to active buffer
	S32_T buf_off = (inp_buf_ps->off & BUF_MASK); // Mask out buffer offset

 
	// Store input sample-set in buffer
	buf_p->sets[buf_off] = *inp_set_p; // Store input sample-set in buffer 
	inp_buf_ps->off++; // Increment Delay-line offset

	// Check for end-of-buffer
	if (0 == (inp_buf_ps->off & BUF_MASK))
	{
		// Check for end-of-delay-line
		if (inp_buf_ps->off >= DELAY_SAMPS)
		{
			inp_buf_ps->off = 0;
		} // if (inp_buf_ps->off >= DELAY_SAMPS)

// if (delay_ps->dbg) printf("WB%1d: OF=%5d \n" ,inp_buf_ps->id ,inp_buf_ps->off ); //MB~
		// Write buffer to memory. NB Memory offset is 1 buffer later then Write Offset
		write_buffer( delay_ps ,sdram_write_p ,buf_p ,inp_buf_ps->off );

		inp_buf_ps->id = (1 - inp_buf_ps->id); // Toggle twin-buffer id	

	} // if (inp_buf_ps->off & BUF_MASK)
} // write_sample_set
/******************************************************************************/
void do_sdram_delay( // Do buffer I/O for input and output sample-sets
	DELAY_S * delay_ps // Pointer to structure containing all delay data
)
{
	S32_T tap_cnt; // tap counter


	write_sample_set( delay_ps ,&(delay_ps->cntrl_p->write) ,&(delay_ps->inp_bufs) ,&(delay_ps->cntrl_p->inp_set) );

	// Loop through all delay taps.
	for (tap_cnt = 0; tap_cnt < delay_ps->tap_num; tap_cnt++)
	{
		read_sample_set( delay_ps ,&(delay_ps->cntrl_p->reads[tap_cnt]) ,&(delay_ps->out_bufs[tap_cnt]) ,&(delay_ps->cntrl_p->delay_sets[tap_cnt]) );
	} // for tap_cnt

} // do_sdram_delay
/******************************************************************************/
void use_sdram_delay( // exercise sdram_delay for one input sample. NB Must have previously called config_sdram_delay
	CNTRL_SDRAM_S * cntrl_dsp_ps // Pointer to structure containing data to control SDRAM buffering
)
{
	// Check if delay-line parameters have been initialised
	if (0 == delay_gs.params_set)
	{
		assert(0 == 1); // Please call config_sdram_delay() function before use_sdram_delay() 
	} // if (0 == init_flag)
	else
	{
		delay_gs.cntrl_p = cntrl_dsp_ps; // Store location of DSP control structure

		// Insert new sample-set into delay-line, and retrieve an array of delayed sample-sets
		do_sdram_delay( &(delay_gs) );
	} // else !(0 == init_flag)

} // use_sdram_delay
/******************************************************************************/
// sdram_delay.c
