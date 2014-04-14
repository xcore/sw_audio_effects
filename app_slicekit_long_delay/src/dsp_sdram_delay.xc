/******************************************************************************\
 * File:	dsp_sdram_delay.xc
 *
 * Description: Coar that applies non-linear gain to stream of audio samples
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

#include "dsp_sdram_delay.h"

// DSP-control coar.

/******************************************************************************/
void initialise_sdram( // Initialise SDRAM Memory-Slice. Used for debug
  chanend c_dsp_sdram // DSP end of channel between DSP coar and SDRAM coar (bi-directional)
)
#define MAX_WORD 0x7fffffff // Maximum Word value
#define DECREMENT (MAX_WORD / AUD_BUF_SAMPS) // Ramp decrement per sample (NB finish on extreme -ve value)
{
	S32_T wrd_buf[SD_ROW_WRDS]; // Word buffer
	U32_T buf_adr = get_word_address( wrd_buf ); // Get address of buffer
	U32_T mem_adr = 0; // Get address of buffer
	S32_T wrd_cnt; // word counter
	S32_T row_cnt; // SDRAM row counter


	wrd_buf[0] = MAX_WORD; // Initialise with peak value

	// Fill buffer with audible samples (Ramp)
	for (wrd_cnt=1; wrd_cnt<SD_ROW_WRDS; wrd_cnt++)
	{
		wrd_buf[wrd_cnt] = wrd_buf[wrd_cnt - 1] - DECREMENT;
	} //for wrd_cnt

	// Fill Memory by copying buffer ('Saw-tooth' wave)
	for (row_cnt=0; row_cnt<SD_NUM_ROWS; row_cnt++)
	{
		write_buffer_to_sdram( c_dsp_sdram ,mem_adr ,buf_adr ,SD_ROW_WRDS );

		mem_adr += SD_ROW_BYTS; // Increment memory address
	} //for row_cnt
} // initialise_sdram
/******************************************************************************/
void zero_sdram( // Initialise SDRAM Memory-Slice to all zeros.
  chanend c_dsp_sdram // DSP end of channel between DSP coar and SDRAM coar (bi-directional)
)
#define DECREMENT (MAX_WORD / AUD_BUF_SAMPS) // Ramp decrement per sample (NB finish on extreme -ve value)
{
	S32_T wrd_buf[SD_ROW_WRDS]; // Word buffer
	U32_T buf_adr = get_word_address( wrd_buf ); // Get address of buffer
	U32_T mem_adr = 0; // Get address of buffer
	S32_T wrd_cnt; // word counter
	S32_T row_cnt; // SDRAM row counter


	// Fill buffer with audible samples (Ramp)
	for (wrd_cnt=0; wrd_cnt<SD_ROW_WRDS; wrd_cnt++)
	{
		wrd_buf[wrd_cnt] = 0; // Set to zero
	} //for wrd_cnt

	// Fill Memory by copying buffer ('Saw-tooth' wave)
	for (row_cnt=0; row_cnt<SD_NUM_ROWS; row_cnt++)
	{
		write_buffer_to_sdram( c_dsp_sdram ,mem_adr ,buf_adr ,SD_ROW_WRDS );

		mem_adr += SD_ROW_BYTS; // Increment memory address
	} //for row_cnt
} // zero_sdram
/******************************************************************************/
void init_sdram_buffers( // Initialisation buffers for SDRAM access
	CNTRL_SDRAM_S &cntrl_gs // Reference to structure containing data to control SDRAM buffering
)
{
	S32_T tap_cnt; // tap counter
	S32_T chan_cnt; // Channel counter


	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < NUM_DELAY_CHANS; chan_cnt++)
	{
		cntrl_gs.src_set.samps[chan_cnt] = 0;

		for (tap_cnt=0; tap_cnt<NUM_DELAY_TAPS; tap_cnt++)
		{
			cntrl_gs.delay_sets[tap_cnt].samps[chan_cnt] = 0;
		} //for tap_cnt
	}

	// Initialise SDRAM control structure ...

	cntrl_gs.write.do_buf = 0;
	for (tap_cnt=0; tap_cnt<NUM_DELAY_TAPS; tap_cnt++)
	{
		cntrl_gs.reads[tap_cnt].do_buf = 0;
	} //for tap_cnt
} // init_sdram_buffers
/******************************************************************************/
void init_parameters( // Initialise delay parameters
	DELAY_PARAM_S &delay_param_s // Reference to structure containing SDRAM-Delay configuration data
)
{
	S32_T delay_max_us; // Maximum delay in micro-secs
	S32_T delay_us; // Delay in micro-secs
	S32_T tap_cnt; // tap counter


	// Initialsie Delay-line configuration parameters ...
	delay_param_s.num = NUM_DELAY_TAPS;
	delay_param_s.freq = DEF_SAMP_FREQ;
	delay_max_us = DELAY_SAMPS * (1000000 / delay_param_s.freq); // Approx smallest delay in micro-secs
	delay_us = (DELAY_SAMPS / ((1 << NUM_DELAY_TAPS) - 1)) * (1000000 / delay_param_s.freq); // approx. smallest tap delay in micro-secs
	delay_param_s.dbg = 0;

	// Initialise Configuration parameter structure
	delay_param_s.us_delays[0] = delay_us;

	for (tap_cnt = 1; tap_cnt<NUM_DELAY_TAPS; tap_cnt++)
	{
		delay_param_s.us_delays[tap_cnt] = (delay_param_s.us_delays[tap_cnt-1] << 1) + delay_us;
	} //for tap_cnt

	assert(delay_max_us > delay_param_s.us_delays[NUM_DELAY_TAPS-1]); // ERROR: delay too large
} // init_parameters
/******************************************************************************/
void buffer_check( // Check if any buffer I/O required
	CNTRL_SDRAM_S &cntrl_gs, // Reference to structure containing data to control SDRAM buffering
  chanend c_dsp_sdram // DSP end of channel between DSP coar and SDRAM coar (bi-directional)
)
{
	S32_T tap_cnt; // tap counter


	// Check if write-buffer is full
	if (cntrl_gs.write.do_buf)
	{
		write_buffer_to_sdram( c_dsp_sdram ,cntrl_gs.write.mem_adr ,cntrl_gs.write.buf_adr ,cntrl_gs.write.wrd_siz );

		cntrl_gs.write.do_buf = 0; // Clear write flag
	} // if (cntrl_gs.do_write)

	// Check if any read-buffers are empty
	for (tap_cnt=0; tap_cnt<NUM_DELAY_TAPS; tap_cnt++)
	{
		if (cntrl_gs.reads[tap_cnt].do_buf)
		{
			read_buffer_from_sdram( c_dsp_sdram ,cntrl_gs.reads[tap_cnt].mem_adr ,cntrl_gs.reads[tap_cnt].buf_adr ,cntrl_gs.reads[tap_cnt].wrd_siz );

			cntrl_gs.reads[tap_cnt].do_buf = 0; // Clear read flag
		} // if (cntrl_gs.do_read)
	} //for tap_cnt

} // buffer_check
/******************************************************************************/
void process_all_chans( // Do DSP effect processing
	CNTRL_SDRAM_S &cntrl_gs, // Reference to structure containing data to control SDRAM buffering
	DELAY_PARAM_S &cur_param_s, // Reference to structure containing delay-line parameters
	CHAN_SET_S &mix_set_s,	// Structure containing mixed output sample-set
	S32_T num_chans	// Number of channels in set
)
{
	S32_T mux; // multiply for mix
	S32_T shift; // bit-shift for mix
	S32_T chan_cnt; // Channel counter
	S32_T tap_cnt; // Channel counter

	// Get next array of delayed output sets, and delay current input set
	use_sdram_delay( cntrl_gs );

	// Mix all delayed samples to produce one output sample ...

	mix_set_s = cntrl_gs.src_set; // Initialse output sample-set with input

	// Loop through all output channels
	for(chan_cnt = 0; chan_cnt < num_chans; chan_cnt++)
	{
		mux = 1;
		shift = 0;
		// Loop through rest of delayed samples
		for(tap_cnt = 0; tap_cnt < cur_param_s.num; tap_cnt++)
		{
			// Mix in next delayed sample. Each one 3/4 volume of previous
			mix_set_s.samps[chan_cnt] += (mux * (cntrl_gs.delay_sets[tap_cnt].samps[chan_cnt] >> shift));
			mux *= 3;
			shift += 2;
		} // for tap_cnt
	} // for chan_cnt

} // process_all_chans
/******************************************************************************/
void dsp_sdram_delay( // Coar that delays a stream of audio samples
	streaming chanend c_dsp_aud, // DSP end of channel between Audio_IO coar and DSP coar (bi-directional)
  chanend c_dsp_sdram // DSP end of channel between DSP coar and SDRAM coar (bi-directional)
)
{
	CNTRL_SDRAM_S cntrl_gs; // Structure containing data to control SDRAM buffering
	DELAY_PARAM_S delay_param_s; // Structure containing SDRAM-Delay configuration data
	CHAN_SET_S zero_set_s;	// Structure containing zero-valued sample-set
	CHAN_SET_S mix_set_s;	// Structure containing mixed output sample-set
	CHAN_SET_S out_set_s;	// Structure containing intermediate audio sample-set, used for cross-fade

	S32_T dry_len = SWAP_NUM;	// time spent in dry state (~8 secs)
	S32_T fx_len = (SWAP_NUM << 1);	// time spent in effect state (to ~16 secs)
	S32_T samp_cnt = 0; // Sample counter
	S32_T chan_cnt; // Channel counter

	PROC_STATE_ENUM cur_proc_state	= DRY2FX; // Initialise processing state to Fade-in EFFECT.


	init_sdram_buffers( cntrl_gs ); // Initialise buffers for SDRAM access

	init_parameters( delay_param_s ); // Initialise delay parameters

	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < NUM_DELAY_CHANS; chan_cnt++)
	{
		out_set_s.samps[chan_cnt] = 0;
		zero_set_s.samps[chan_cnt] = 0;
	}

	// Configure Delay-line parameters ...

	config_sdram_delay( delay_param_s );

	zero_sdram( c_dsp_sdram ); // Clear Memory-Slice.

	// Fade-in input to prevent clicks
	while(samp_cnt < FADE_LEN)
	{
		// Send/Receive samples over Audio coar channel
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_DELAY_CHANS; chan_cnt++)
		{
			c_dsp_aud :> cntrl_gs.src_set.samps[chan_cnt];
			c_dsp_aud <: out_set_s.samps[chan_cnt];
		}

		samp_cnt++; // Update sample counter

		// Create output sample-set by cross-fading from zero-valued sample-set to input sample-set
		cross_fade_sample( out_set_s.samps ,zero_set_s.samps ,cntrl_gs.src_set.samps ,NUM_DELAY_CHANS ,samp_cnt );

		cntrl_gs.src_set = out_set_s; // Copy output back to input

		// Do DSP Processing ...
		process_all_chans( cntrl_gs ,delay_param_s ,mix_set_s ,NUM_DELAY_CHANS );

		buffer_check( cntrl_gs ,c_dsp_sdram ); // Check if any buffer I/O required

		out_set_s = cntrl_gs.src_set; // Copy input to output
	} // while(1)

	samp_cnt = 0; // Reset sample counter

	// Loop forever
	while(1)
	{
		// Send/Receive samples over Audio coar channel
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_DELAY_CHANS; chan_cnt++)
		{
			c_dsp_aud :> cntrl_gs.src_set.samps[chan_cnt];
			c_dsp_aud <: out_set_s.samps[chan_cnt];
		}

		samp_cnt++; // Update sample counter

		// Do DSP Processing ...
		process_all_chans( cntrl_gs ,delay_param_s ,mix_set_s ,NUM_DELAY_CHANS );

		buffer_check( cntrl_gs ,c_dsp_sdram ); // Check if any buffer I/O required

		// Check current processing State
		switch(cur_proc_state)
		{
			case EFFECT: // Do DSP effect processing
				out_set_s = mix_set_s; // Copy delayed sample-set to output

				if (fx_len < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = FX2DRY; // Switch to Fade-out Effect
				} // if (fx_len < samp_cnt)
			break; // case EFFECT:

			case FX2DRY: // Fade-Out Effect
				// Create output sample-set by cross-fadeing from delayed sample-set to input sample-set
				cross_fade_sample( out_set_s.samps ,mix_set_s.samps ,cntrl_gs.src_set.samps ,NUM_DELAY_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY_ONLY; // Switch to Dry-Only Processing
// printcharln('D'); //MB~
				} // if (FADE_LEN < samp_cnt)
			break; // case FX2DRY:

			case DRY_ONLY: // No Effect (Dry signal only)
				out_set_s = cntrl_gs.src_set; // Copy input to output

				if (dry_len < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY2FX; // Switch to Fade-In Effect
				} // if (dry_len < samp_cnt)
			break; // case DRY_ONLY:

			case DRY2FX: // Fade-in Effect
				// Create output sample-set by cross-fadeing from input sample-set to delayed sample-set
				cross_fade_sample( out_set_s.samps ,cntrl_gs.src_set.samps ,mix_set_s.samps ,NUM_DELAY_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = EFFECT; // Switch to Effect-Only Processing
// printcharln('E'); //MB~
				} // if (FADE_LEN < samp_cnt)
			break; // case DRY2FX:

			default:
				assert(0 == 1); // ERROR: Unsupported state
			break; // default:
		} // switch(cur_proc_state)

	} // while(1)

} // dsp_sdram_delay
/*****************************************************************************/
// dsp_sdram_delay.xc
