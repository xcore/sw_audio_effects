/******************************************************************************\
 * File:	dsp_effects.xc
 *  
 * Description: Control coar for Reverb, also handles delay functionality, 
 *	calls Loudness and Equalisation coars
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

#include "dsp_effects.h"

// DSP-control coar.

/******************************************************************************/
void init_dsp_effects( // Initialisation for DSP effects global data structure
	DSP_EFFECT_S &dspfx_gs // Reference to structure containing data to control DSP Effects
)
{
	dspfx_gs.fade_state	= START; // Initialise processing state
	dspfx_gs.cur_effect = REVERB; // Initialise audio effect
	dspfx_gs.fx_len = SWAP_NUM; // time spent in effect state (to ~8 secs)
	dspfx_gs.dry_len = SWAP_NUM;	// time spent in dry state (~8 secs)
	dspfx_gs.samp_cnt = 0;	// Sample counter
} // init_sdram_buffers
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

		for (tap_cnt=0; tap_cnt<NUM_REVERB_TAPS; tap_cnt++)
		{
			cntrl_gs.delay_sets[tap_cnt].samps[chan_cnt] = 0;
		} //for tap_cnt
	}

	// Initialise SDRAM control structure ...

	cntrl_gs.write.do_buf = 0;
	for (tap_cnt=0; tap_cnt<NUM_REVERB_TAPS; tap_cnt++)
	{
		cntrl_gs.reads[tap_cnt].do_buf = 0;
	} //for tap_cnt
} // init_sdram_buffers
/*****************************************************************************/
void send_biquad_config( // Send BiQuad filter configuration data
	streaming chanend c_dsp_eq, // Channel connecting to Equalisation coar (bi-directional)
	REVERB_PARAM_S &reverb_param_s // Reference to structure containing reverb parameters
)
{
	// Default BiQuad Configuration
	BIQUAD_PARAM_S biquad_param_s = { DEF_FILT_MODE ,DEF_SAMP_FREQ ,DEF_SIG_FREQ ,DEF_QUAL_FACT };


	biquad_param_s.samp_freq = reverb_param_s.samp_freq; // Assign requested sample frequency
	biquad_param_s.sig_freq = reverb_param_s.sig_freq; // Assign requested significant (lo-pass) frequency

	soutct( c_dsp_eq ,CFG_BIQUAD ); // Signal BiQuad configuration data transmission

	c_dsp_eq <: biquad_param_s; // Send BiQuad filter configuration data
} // send_biquad_config
/*****************************************************************************/
void send_loudness_config( // Send Loudness configuration data
	streaming chanend c_dsp_gain, // Channel connecting to Loudness coar (bi-directional)
	REVERB_PARAM_S &reverb_param_s // Reference to structure containing reverb parameters
)
{
	GAIN_PARAM_S gain_param_s = { DEF_GAIN }; // Default Loudness Configuration


	gain_param_s.gain = reverb_param_s.gain; // Assign requested maximum gain

	soutct( c_dsp_gain ,CFG_GAIN ); // Signal Gain configuration data transmission

	c_dsp_gain <: gain_param_s; // Send Loudness configuration data
} // send_loudness_config
/*****************************************************************************/
void sync_reverb_config( // Synchronise a reverb configuration (with other coars)
	streaming chanend c_dsp_eq, // Channel connecting to Equalisation coar (bi-directional)
	streaming chanend c_dsp_gain, // Channel connecting to Loudness coar (bi-directional)
	REVERB_PARAM_S &reverb_param_s // Structure containing Reverb configuration parameters
)
{
	// Send data to other coars ...
	send_biquad_config( c_dsp_eq ,reverb_param_s );		// Send BiQuad filter configuration data
	send_loudness_config( c_dsp_gain ,reverb_param_s );	// Send Gain-shaping configuration data

	config_sdram_reverb( reverb_param_s ); // Configure remaining reverb parameters in this coar (Delay-line)

} // sync_reverb_config
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

		return; // Not enough time to service more than one buffer transfer/sample
	} // if (cntrl_gs.do_write)

	// Check if any read-buffers are empty
	for (tap_cnt=0; tap_cnt<NUM_REVERB_TAPS; tap_cnt++)
	{
		if (cntrl_gs.reads[tap_cnt].do_buf)
		{
			read_buffer_from_sdram( c_dsp_sdram ,cntrl_gs.reads[tap_cnt].mem_adr ,cntrl_gs.reads[tap_cnt].buf_adr ,cntrl_gs.reads[tap_cnt].wrd_siz );

			cntrl_gs.reads[tap_cnt].do_buf = 0; // Clear read flag

			return; // Not enough time to service more than one buffer transfer/sample
		} // if (cntrl_gs.do_read)
	} //for tap_cnt

} // buffer_check
/*****************************************************************************/
EFFECT_ENUM decode_button_state( // Analyse new button state, and return requested audio effect type
	BUTTON_STATES_ENUM button_state // current button state
)
{
	switch( button_state )
	{
		case BUTTON_1 :	// port value returned for when just button 1 (SW1) pressed
			printstrln("BiQuad");
			return BIQUAD;
		break; // case BUTTON_1 
	
		case BUTTON_2 : // port value returned for when just button 2 (SW2) pressed
			printstrln("Reverb");
			return REVERB;
		break; // case BUTTON_2 
	
		default:	
			printstr("ERROR: Found following UN-supported Button-state = ");
			printintln( button_state );
			assert(0 == 1);
		break; // default
	} // switch( button_state )

	return 0; // NB Unreachable, but stops warnings!
} // decode_button_state
/*****************************************************************************/
void check_for_effect_change( // Monitors GPIO buttons to see if effect change requested
	DSP_EFFECT_S &dspfx_gs,  // Reference to structure containing data to control DSP Effects
	chanend c_dsp_gpio // DSP end of channel between GPIO and DSP coars
)
{
	BUTTON_STATES_ENUM button_state; // button state


	// Check for new button state
	select
	{
		case c_dsp_gpio :> button_state :
			dspfx_gs.cur_effect = decode_button_state( button_state ); // Decode requested effect from button state

			// Reset Cross-fade state
			dspfx_gs.fade_state = EFFECT;
			dspfx_gs.samp_cnt = 0;
		break;

		default:
			// Nothing to do. NB prevents blocking on above case
		break;
	} // select

} // check_for_effect_change
/*****************************************************************************/
void control_output_mix( // Controls Effect/Dry dynamic Output Mix
	DSP_EFFECT_S &dspfx_gs,  // Reference to structure containing data to control DSP Effects
	CHAN_SET_S &out_set_s,	// Reference to structure containing output audio sample-set
	CHAN_SET_S &fade_set_s,	// Reference to structure containing Cross-fade audio sample-set
	CHAN_SET_S &inp_set_s		// Reference to structure containing Input audio sample-set
)
{
	// Determine which output-mix state we are in, and act accordingly!-)
	switch(dspfx_gs.fade_state)
	{
		case EFFECT: // Full Effect (Send Effect to Output)
			out_set_s = fade_set_s; // No fade so copy to output

			if (dspfx_gs.fx_len < dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = FX2DRY; // Switch to Fade-out Effect
			} // if (dspfx_gs.fx_len < dspfx_gs.samp_cnt)
		break; // case EFFECT:

		case FX2DRY: // Fade-Out Effect (Crossfade Effect to Dry)
			cross_fade_sample( out_set_s.samps ,fade_set_s.samps ,inp_set_s.samps ,NUM_REVERB_CHANS ,dspfx_gs.samp_cnt );

			if (FADE_LEN <= dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = DRY_ONLY; // Switch to Dry-Only Processing
				printstrln("Dry");
			} // if (SWAP_NUM < dspfx_gs.samp_cnt)
		break; // case FX2DRY:

		case DRY_ONLY: // No Effect (Send Input to Output)
			out_set_s = inp_set_s;
			
			if (dspfx_gs.dry_len < dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = DRY2FX; // Switch to Fade-In Effect
			} // if (dspfx_gs.dry_len < dspfx_gs.samp_cnt)
		break; // case DRY_ONLY:

		case DRY2FX: // Fade-in Effect (Crossfade Dry to Effect)
			cross_fade_sample( out_set_s.samps ,inp_set_s.samps ,fade_set_s.samps ,NUM_REVERB_CHANS ,dspfx_gs.samp_cnt );

			if (FADE_LEN <= dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = EFFECT; // Switch to Effect Processing
				printstrln("Effect");
			} // if (SWAP_NUM < dspfx_gs.samp_cnt)
		break; // case DRY2FX:

		case START: // Fade-in Effect
			fade_in_sample( out_set_s.samps ,fade_set_s.samps ,NUM_REVERB_CHANS ,dspfx_gs.samp_cnt );

			if (FADE_LEN <= dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = EFFECT; // Switch to Effect Processing
			} // if (SWAP_NUM < dspfx_gs.samp_cnt)
		break; // case DRY2FX:

		default:
			assert(0 == 1); // ERROR: Unsupported state
		break; // default:
	} // switch(dspfx_gs.fade_state)

	dspfx_gs.samp_cnt++; // Update sample counter
} // control_output_mix
/*****************************************************************************/
void dsp_effects( // Controls audio stream processing for reverb application using dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O coar (bi-directional)
	streaming chanend c_dsp_eq, // Channel connecting to Equalisation coar (bi-directional)
	streaming chanend c_dsp_gain, // Channel connecting to Loudness coar (bi-directional)
  chanend c_dsp_sdram, // DSP end of channel between DSP coar and SDRAM coar (bi-directional)
	chanend c_dsp_gpio // DSP end of channel between GPIO and DSP coars
)
{
	CNTRL_SDRAM_S sdram_gs; // Structure containing data to control SDRAM buffering
	DSP_EFFECT_S dspfx_gs;  // Structure containing data to control DSP Effects
	CHAN_SET_S inp_set_s;	// Structure containing Input audio sample-set
	CHAN_SET_S uneq_set_s;	// Structure containing Unequalised audio sample-set
	CHAN_SET_S unamp_set_s;	// Structure containing Unamplified audio sample-set
	CHAN_SET_S ampli_set_s;	// Structure containing Amplified audio sample-set
	CHAN_SET_S out_set_s;	// Structure containing output audio sample-set
	CHAN_SET_S fade_set_s;	// Structure containing Cross-fade audio sample-set

	S32_T chan_cnt; // Channel counter

	// Default Reverb parameters
	REVERB_PARAM_S def_param_s = {{ DEF_DRY_LVL ,DEF_FX_LVL ,DEF_FB_LVL ,DEF_CROSS_MIX } 
																,DEF_ROOM_SIZE ,DEF_SIG_FREQ ,DEF_SAMP_FREQ ,DEF_GAIN };


	init_sdram_buffers( sdram_gs ); // Initialise buffers for SDRAM access
	init_dsp_effects( dspfx_gs ); // Initialise DSP Effects

	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
	{
		inp_set_s.samps[chan_cnt] = 0;
	}	// for chan_cnt
	uneq_set_s = inp_set_s;
	unamp_set_s = inp_set_s;
	ampli_set_s = inp_set_s;
	fade_set_s = inp_set_s;
	out_set_s = inp_set_s;

	// Synchronise a reverb configuration (with other coars) using default parameters
	def_param_s.gain = 8; // Bring-up gain
	def_param_s.room_size = 100; // Fine-tune Room-size

	sync_reverb_config( c_dsp_eq ,c_dsp_gain ,def_param_s );
	printstrln("Effect");

	// Loop forever
	while(1)
	{
		// Send/Receive audio samples over sample coar channels
#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_aud_dsp :> inp_set_s.samps[chan_cnt];  // Receive input samples from Audio I/O coar 
			c_aud_dsp <: out_set_s.samps[chan_cnt];  // Send Output samples back to Audio I/O coar 
		} // for chan_cnt

#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_eq <: uneq_set_s.samps[chan_cnt]; // Send Unequalised samples to EQ coar  
			c_dsp_eq :> sdram_gs.src_set.samps[chan_cnt]; // Receive Equalised samples back from EQ coar  
		} // for chan_cnt

#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_gain <: unamp_set_s.samps[chan_cnt]; // Send Unamplified samples to Loudness coar   
			c_dsp_gain :> ampli_set_s.samps[chan_cnt]; // Receive Amplified samples back from Loudness coar  

		} // for chan_cnt

		check_for_effect_change( dspfx_gs ,c_dsp_gpio ); // Checks if GPIO buttons pressed

		// Select DSP Effect Processing ...
		switch( dspfx_gs.cur_effect)
		{
			case REVERB:
				// Call reverb routines
				use_sdram_reverb( sdram_gs ,inp_set_s ,uneq_set_s ,unamp_set_s ,fade_set_s ,ampli_set_s );
				buffer_check( sdram_gs ,c_dsp_sdram ); // Check if any buffer I/O required
			break; // case REVERB

			case BIQUAD:
				// Connect I/O directly to EQ thread
				uneq_set_s = inp_set_s;
				fade_set_s = sdram_gs.src_set;
			break; // case BIQUAD
		} // switch( dspfx_gs.cur_effect)

		control_output_mix( dspfx_gs ,out_set_s ,fade_set_s ,inp_set_s ); // Select Effect/Dry Output mix

	} // while(1)

} // dsp_effects
/*****************************************************************************/
// dsp_effects.xc