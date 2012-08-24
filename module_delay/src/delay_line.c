/******************************************************************************\
 * File:	delay_line.c
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

#include "delay_line.h"

static DELAY_S delay_s; // Structure containing all delay data (NB Make global to avoid using malloc)

/******************************************************************************/
S32_T get_sample_delay( // Calculate delay in samples
	S32_T samp_freq, // current sample frequency
	S32_T delay_ms // current delay time (in milli-secs)
	) // Return delay in samples
{
	S32_T sample_delay; // delay measured in samples


	// Check for sensible sample frequency
	if (samp_freq < MIN_AUDIO_FREQ)
	{
		samp_freq = MIN_AUDIO_FREQ;
	} // if (samp_freq < MIN_AUDIO_FREQ)

	// Calculate delay paremeters in samples. NB divide by 1000 as delay_ms in milli-secs
	sample_delay = (S32_T)(( (S64_T)samp_freq * (S64_T)delay_ms + (S64_T)500 ) / (S64_T)1000 );

	// Check buffer is large enough
	if (sample_delay > MAX_DELAY )
	{
		sample_delay = MAX_DELAY; // Reduce to max allowed delay
	} // if (sample_delay >= DELAY_SIZE )

	return sample_delay;
} // get_sample_delay
/******************************************************************************/
void update_common_delays( // Update delays for each channel
	DELAY_S * delay_sp, // Pointer to structure containing all delay data
	S32_T samp_freq, // current sample frequency
	S32_T delay_ms // current delay time (in milli-secs)
)
{
	DELAY_CHAN_S * chan_sp; // Pointer to structure containing delay data for one channel
	S32_T chan_cnt; // channel counter
	S32_T sample_delay; // delay measured in samples


	// Get sample delay
	sample_delay = get_sample_delay( samp_freq ,delay_ms );

	// Loop through all channels
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		chan_sp = &(delay_sp->chan_s[chan_cnt]); // Point to structure for current channel

		chan_sp->delay = sample_delay; // Update sample delay
		chan_sp->out = (chan_sp->inp + DELAY_SIZE - sample_delay) % DELAY_SIZE; // Update output offset
	} // for chan_cnt

} // update_common_delays
/******************************************************************************/
void init_delay_chan( // Initialise delay data for one channel. WARNING chan_sp->delay must have been previously initialised 
	DELAY_CHAN_S * chan_sp, // Pointer to structure containing current Delay data
	S32_T sample_delay // delay measured in samples
)
{
	S32_T delay_cnt; // delay-line counter


	// Loop through delay slots
	for (delay_cnt=0; delay_cnt<DELAY_SIZE; delay_cnt++)
	{
		chan_sp->buf[delay_cnt] = 0; // Clear delay slot
	} // for delay_cnt

	chan_sp->delay = sample_delay; // Update sample delay
	chan_sp->inp = sample_delay; // Set offset for input sample
	chan_sp->out = 0; // Set offset for output sample
} // init_delay_chan
/******************************************************************************/
void init_delay( // Create structure for all delay data, and initialise
	DELAY_S * delay_sp, // Pointer to structure containing all delay data
	S32_T samp_freq, // current sample frequency
	S32_T delay_ms // current delay time (in milli-secs)
)
{
	S32_T chan_cnt; // delay-line counter
	S32_T sample_delay; // delay measured in samples


	// Get sample delay
	sample_delay = get_sample_delay( samp_freq ,delay_ms );

	// Loop through all output channels
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		init_delay_chan( &(delay_sp->chan_s[chan_cnt]) ,sample_delay );
	} // for chan_cnt

} // init_delay
/******************************************************************************/
SAMP_CHAN_T delay_line_chan( // Retrieve output sample from buffer, and store input sample
	SAMP_CHAN_T inp_samp, // input sample at channel precision
	DELAY_CHAN_S * chan_sp // Pointer to structure containing current Delay data
) // Return Filtered Output Sample
{
	SAMP_CHAN_T out_samp; // delayed output sample, returned to channel


	chan_sp->buf[ chan_sp->inp ] = inp_samp ; // Store input sample in buffer 
	out_samp = chan_sp->buf[ chan_sp->out ]; // Retrieve output sample from buffer 

	// Update offsets
	chan_sp->inp = (chan_sp->inp + 1) % DELAY_SIZE; // increment input offset and wrap
	chan_sp->out = (chan_sp->out + 1) % DELAY_SIZE; // increment output offset and wrap

	return out_samp;
} // delay_line_chan
/******************************************************************************/
S32_T delay_line_wrapper( // Wrapper for delay_line function
	S32_T inp_samp, // input sample from channel
	S32_T cur_chan, // current channel
	S32_T samp_freq, // current sample frequency
	S32_T delay_ms  // sample delay in milli-secs
) // Return filtered Output Sample
{
	static S32_T init_flg = 0; // Flag indicating Delay is initialised
	static S32_T old_delay = 0; // old sample delay
	static S32_T old_freq = 0; // old sample frequency
	static S32_T delay_samp; // Delayed sample
	static S32_T out_samp = 0; // delayed output sample at channel precision
	S32_T valid_change = 0; // Preset flag to `parameters unchanged`


	// Check if filter initialised
	if (0 == init_flg)
	{
  	init_delay( &delay_s ,samp_freq ,delay_ms );	// Initialise delay data structure
		init_flg = 1;	// Set initialisation-done flag
	} // if (0 == init_flag)

	// Check for valid parameter change ...

	if (old_freq != samp_freq)
	{
		// Check for sensible frequency
		if (MIN_AUDIO_FREQ < samp_freq)
		{
			valid_change = 1; // Signal valid parameter change
		} // if (20 < samp_freq)

		old_freq = samp_freq; // Store updated sample frequency
	} // if (samp_freq != old_freq)

	if (old_delay != delay_ms)
	{
		valid_change = 1; // Signal valid parameter change
		old_delay = delay_ms;
	} // if (old_delay != delay_ms)

	// Check if delay-times need updating
	if (1 == valid_change)
	{
		// recalculate sample delay
	  update_common_delays( &(delay_s) ,samp_freq ,delay_ms );
	} // if (samp_freq != old_freq)


	// Create output sample by mixing input with delayed sample
	out_samp = (inp_samp + (delay_samp << 1) + delay_samp + 2) >> 2;

	// Insert new sample into delay-line, and retrieve a delayed sample
	delay_samp = (S32_T)delay_line_chan( (SAMP_CHAN_T)out_samp ,&(delay_s.chan_s[cur_chan]) );

//	out_samp = inp_samp; //MB~ DBG

	return out_samp;
} // delay_line_wrapper
/******************************************************************************/
// delay_line.xc
