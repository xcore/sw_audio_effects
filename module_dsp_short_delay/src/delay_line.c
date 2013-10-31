/******************************************************************************\
 * File:	delay_line.c
 *
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

// Structure containing all delay data (NB Make global to avoid using malloc)
static DELAY_S delay_gs = { .init_done = 0, .params_set = 0 }; // Clear initialisation flags

/******************************************************************************/
void get_sample_delays( // Calculate delay taps in samples
	U32_T samp_delays[], // delays measured in samples
	DELAY_PARAM_S * cur_param_ps // Pointer to structure containing delay-line parameters
) // Return delay in samples
{
	S32_T num_taps = cur_param_ps->num; // Get Number of taps to calculate
	S32_T tap_cnt; // delay measured in samples
	U32_T chk_delay; // current delay measured in samples


	// Loop through all delay taps.
	for (tap_cnt = 0; tap_cnt < num_taps; tap_cnt++)
	{
		// Calculate current delay in samples
		chk_delay = ((S64_T)cur_param_ps->freq * (S64_T)cur_param_ps->us_delays[tap_cnt] + (S64_T)500000) / (S64_T)1000000;

		assert(DELAY_SIZE > chk_delay); // Check buffer is large enough

		samp_delays[tap_cnt] = chk_delay; // Assign delay in samples
	} // for tap_cnt

} // get_sample_delays
/******************************************************************************/
void update_delay_chan( // Update delays for one channel
	DELAY_CHAN_S * chan_ps, // Pointer to structure containing current Delay data
	DELAY_PARAM_S * cur_param_ps, // Pointer to structure containing delay-line parameters
	U32_T samp_delays[]  // delays measured in samples
)
{
	S32_T tap_cnt; // delay measured in samples
	U32_T cur_delay; // current delay in samples


	chan_ps->tap_num = cur_param_ps->num; // Update number of delay taps in use on this channel

	// Loop through all delay taps.
	for (tap_cnt = 0; tap_cnt < chan_ps->tap_num; tap_cnt++)
	{
		cur_delay = samp_delays[tap_cnt]; // Get current common sample delay
		chan_ps->delays[tap_cnt] = cur_delay; // Update channel sample delay
		chan_ps->outs[tap_cnt] = (chan_ps->inp + DELAY_SIZE - cur_delay) % DELAY_SIZE; // Update output offset
	} // for tap_cnt

} // update_delay_chan
/******************************************************************************/
void update_common_delays( // Update delays for each channel
	DELAY_S * delay_ps, // Pointer to structure containing all delay data
	DELAY_PARAM_S * cur_param_ps // Pointer to structure containing delay-line parameters
)
{
	U32_T samp_delays[NUM_DELAY_TAPS]; // delays measured in samples
	S32_T chan_cnt; // channel counter


	// Calculate delays in samples
	get_sample_delays( samp_delays ,cur_param_ps );

// printstr("DL= "); printintln( (int)samp_delays[0] ); // MB~

	// Loop through all output channels
	for (chan_cnt=0; chan_cnt<NUM_DELAY_CHANS; chan_cnt++)
	{
		update_delay_chan( &(delay_ps->chan_s[chan_cnt]) ,cur_param_ps ,samp_delays );
	} // for chan_cnt

} // update_common_delays
/******************************************************************************/
void init_delay_chan( // Initialise delay data for one channel.
	DELAY_CHAN_S * chan_ps // Pointer to structure containing current Delay data
)
{
	S32_T delay_cnt; // delay-line counter


	// Loop through delay slots
	for (delay_cnt=0; delay_cnt<DELAY_SIZE; delay_cnt++)
	{
		chan_ps->buf[delay_cnt] = 0; // Clear delay slot
	} // for delay_cnt

	chan_ps->inp = MAX_DELAY; // Set offset for input sample to last slot in delay-line
} // init_delay_chan
/******************************************************************************/
void config_delay_line( // Configure delay_line parameters. NB Must be called before use_delay_line
	DELAY_PARAM_S * cur_param_ps // Pointer to structure containing delay-line parameters
)
{
	S32_T chan_cnt; // delay-line counter


	// Check if Delay-Line initialised
	if (0 == delay_gs.init_done)
	{ // Initialse Delay-line
		for (chan_cnt=0; chan_cnt<NUM_DELAY_CHANS; chan_cnt++)
		{
			init_delay_chan( &(delay_gs.chan_s[chan_cnt]) );
		} // for chan_cnt

		delay_gs.init_done = 1; // Signal Delay-line initialised
	} // if (0 == delay_gs->init_done)

	// Check for sensible frequency
	if (MIN_AUDIO_FREQ < cur_param_ps->freq)
	{
		// recalculate sample delay
	  update_common_delays( &(delay_gs) ,cur_param_ps );

		delay_gs.params_set = 1; // Signal Delay-line parameters configured
	} // if (MIN_AUDIO_FREQ < cur_param_ps->freq)

} // config_delay_line
/******************************************************************************/
void delay_line_chan( // Retrieve output sample from buffer, and store input sample
	SAMP_CHAN_T out_samps[], // array of delayed output samples at channel precision
	SAMP_CHAN_T inp_samp, // input sample at channel precision
	DELAY_CHAN_S * chan_ps // Pointer to structure containing current Delay data
) // Return Filtered Output Sample
{
	S32_T tap_cnt; // delay measured in samples


	chan_ps->buf[ chan_ps->inp ] = inp_samp ; // Store input sample in buffer
	chan_ps->inp = increment_circular_offset( (chan_ps->inp + 1) ,DELAY_SIZE ); // increment input offset and wrap

	// Loop through all delay taps.
	for (tap_cnt = 0; tap_cnt < chan_ps->tap_num; tap_cnt++)
	{
		out_samps[tap_cnt] = chan_ps->buf[ chan_ps->outs[tap_cnt] ]; // Retrieve output sample from buffer
		chan_ps->outs[tap_cnt] = increment_circular_offset( (chan_ps->outs[tap_cnt] + 1) ,DELAY_SIZE ); // increment output offset and wrap
	} // for tap_cnt

} // delay_line_chan
/******************************************************************************/
void use_delay_line( // exercise delay_line for one input sample. NB Must have previously called config_delay_line
	SAMP_CHAN_T out_samps[], // array of delayed output samples at channel precision
	SAMP_CHAN_T inp_samp, // input sample from channel
	S32_T cur_chan // current channel
)
{
	// Check if delay-line parameters have been initialised
	if (0 == delay_gs.params_set)
	{
		assert(0 == 1); // Please call config_delay_line() function before use_delay_line()
	} // if (0 == init_flag)
	else
	{
		// Insert new sample into delay-line, and retrieve a set of delayed samples
		delay_line_chan( out_samps ,inp_samp ,&(delay_gs.chan_s[cur_chan]) );
//	out_samp[0] = inp_samp; //MB~ DBG
	} // else !(0 == init_flag)

} // use_delay_line
/******************************************************************************/
// delay_line.c
