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
void get_sample_delays( // Calculate delay taps in samples
	DELAY_S * delay_sp, // Pointer to structure containing all delay data
	U32_T samp_delays[], // delays measured in samples
	S32_T samp_freq, // current sample frequency
	S32_T delay_ms // current delay time (in milli-secs)
	) // Return delay in samples
{
	U32_T chk_delay; // current delay measured in samples
	U64_T samps_1000; // No of samples (in 1000 seconds)
	S32_T tap_cnt; // delay measured in samples


	// Check for sensible sample frequency
	if (samp_freq < MIN_AUDIO_FREQ)
	{
		samp_freq = MIN_AUDIO_FREQ;
	} // if (samp_freq < MIN_AUDIO_FREQ)

	// Check Max delay in samples. NB divide by 1000 as delay_ms in milli-secs
	samps_1000 = (S64_T)samp_freq * (S64_T)delay_ms; // Handy intermediate value
	chk_delay = (S32_T)((samps_1000 + (S64_T)500) / (S64_T)1000);
  
	// Check buffer is large enough
	if (chk_delay > MAX_DELAY )
	{ // Down-size requested max delay to fit in memory
		delay_ms = (S32_T)(((S64_T)delay_ms * (S64_T)MAX_DELAY) / (S64_T)chk_delay); // Reduce to max allowed delay
	} // if (sample_delay >= DELAY_SIZE )

	// Loop through all delay taps.
	for (tap_cnt = 0; tap_cnt < NUM_TAPS; tap_cnt++)
	{
		// Calculate delay paremeters in samples. NB divide by 1000 as delay_ms in milli-secs
		samp_delays[tap_cnt] = (S32_T)((samps_1000 * (S64_T)delay_sp->tap_ratios[tap_cnt] + ((S64_T)500 << TAP_BITS) ) / ((S64_T)1000 << TAP_BITS) );
	} // for tap_cnt

} // get_sample_delays
/******************************************************************************/
void update_delay_chan( // Update delays for one channel
	DELAY_CHAN_S * chan_sp, // Pointer to structure containing current Delay data
	U32_T samp_delays[]  // delays measured in samples
)
{
	S32_T tap_cnt; // delay measured in samples
	U32_T cur_delay; // current delay in samples


	// Loop through all delay taps.
	for (tap_cnt = 0; tap_cnt < NUM_TAPS; tap_cnt++)
	{
		cur_delay = samp_delays[tap_cnt]; // Get current sample delay
		chan_sp->delays[tap_cnt] = cur_delay; // Update sample delay
		chan_sp->outs[tap_cnt] = (chan_sp->inp + DELAY_SIZE - cur_delay) % DELAY_SIZE; // Update output offset
	} // for tap_cnt

} // update_delay_chan
/******************************************************************************/
void update_common_delays( // Update delays for each channel
	DELAY_S * delay_sp, // Pointer to structure containing all delay data
	S32_T samp_freq, // current sample frequency
	S32_T delay_ms // current delay time (in milli-secs)
)
{
	U32_T samp_delays[NUM_TAPS]; // delays measured in samples
	S32_T chan_cnt; // channel counter


	// Get sample delay
	get_sample_delays( delay_sp ,samp_delays ,samp_freq ,delay_ms );

	// Loop through all output channels
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		update_delay_chan( &(delay_sp->chan_s[chan_cnt]) ,samp_delays );
	} // for chan_cnt

} // update_common_delays
/******************************************************************************/
void init_delay_chan( // Initialise delay data for one channel. WARNING chan_sp->delay must have been previously initialised 
	DELAY_CHAN_S * chan_sp, // Pointer to structure containing current Delay data
	U32_T samp_delays[]  // delays measured in samples
)
{
	S32_T delay_cnt; // delay-line counter
	S32_T tap_cnt; // delay measured in samples
	U32_T cur_delay; // current delay in samples
	U32_T max_delay; // maximum delay in samples


	// Loop through delay slots
	for (delay_cnt=0; delay_cnt<DELAY_SIZE; delay_cnt++)
	{
		chan_sp->buf[delay_cnt] = 0; // Clear delay slot
	} // for delay_cnt

	max_delay = samp_delays[NUM_TAPS - 1]; // Get maximum delay (in samples)
	chan_sp->inp = max_delay; // Set offset for input sample to maximum delay

	// Loop through all delay taps.
	for (tap_cnt = 0; tap_cnt < NUM_TAPS; tap_cnt++)
	{
		cur_delay = samp_delays[tap_cnt]; // Get current sample delay
		chan_sp->delays[tap_cnt] = cur_delay; // Update sample delay
		chan_sp->outs[tap_cnt] = max_delay - cur_delay; // Set offset for current output sample
	} // for tap_cnt

} // init_delay_chan
/******************************************************************************/
void init_delay( // Create structure for all delay data, and initialise
	DELAY_S * delay_sp, // Pointer to structure containing all delay data
	S32_T samp_freq, // current sample frequency
	S32_T delay_ms // current delay time (in milli-secs)
)
{
	S32_T chan_cnt; // delay-line counter
	U32_T samp_delays[NUM_TAPS]; // delays measured in samples


	assert(4 ==  NUM_TAPS); // If fails, update below for different No of taps
	delay_sp->tap_ratios[0] = TAP_0;
	delay_sp->tap_ratios[1] = TAP_1;
	delay_sp->tap_ratios[2] = TAP_2;
	delay_sp->tap_ratios[3] = TAP_3;

	assert(MAX_TAP == delay_sp->tap_ratios[NUM_TAPS - 1]); // Calculations depend on Max tap value of MAX_TAP

	// Get sample delay
	get_sample_delays( delay_sp ,samp_delays ,samp_freq ,delay_ms );

	// Loop through all output channels
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		init_delay_chan( &(delay_sp->chan_s[chan_cnt]) ,samp_delays );
	} // for chan_cnt

} // init_delay
/******************************************************************************/
void delay_line_chan( // Retrieve output sample from buffer, and store input sample
	SAMP_CHAN_T out_samps[], // array of delayed output samples at channel precision
	SAMP_CHAN_T inp_samp, // input sample at channel precision
	DELAY_CHAN_S * chan_sp // Pointer to structure containing current Delay data
) // Return Filtered Output Sample
{
	S32_T tap_cnt; // delay measured in samples


	chan_sp->buf[ chan_sp->inp ] = inp_samp ; // Store input sample in buffer 
	chan_sp->inp = (chan_sp->inp + 1) % DELAY_SIZE; // increment input offset and wrap

	// Loop through all delay taps.
	for (tap_cnt = 0; tap_cnt < NUM_TAPS; tap_cnt++)
	{
		out_samps[tap_cnt] = chan_sp->buf[ chan_sp->outs[tap_cnt] ]; // Retrieve output sample from buffer 
		chan_sp->outs[tap_cnt] = (chan_sp->outs[tap_cnt] + 1) % DELAY_SIZE; // increment output offset and wrap
	} // for tap_cnt

} // delay_line_chan
/******************************************************************************/
void delay_line_wrapper( // Wrapper for delay_line function
	SAMP_CHAN_T out_samps[], // array of delayed output samples at channel precision
	SAMP_CHAN_T inp_samp, // input sample from channel
	S32_T cur_chan, // current channel
	S32_T samp_freq, // current sample frequency
	S32_T delay_ms  // sample delay in milli-secs
) // Return filtered Output Sample
{
	static S32_T init_flg = 0; // Flag indicating Delay is initialised
	static S32_T old_delay = 0; // old sample delay
	static S32_T old_freq = 0; // old sample frequency
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

	// Insert new sample into delay-line, and retrieve a set of delayed samples
	delay_line_chan( out_samps ,inp_samp ,&(delay_s.chan_s[cur_chan]) );

//	out_samp[0] = inp_samp; //MB~ DBG

} // delay_line_wrapper
/******************************************************************************/
// delay_line.xc
