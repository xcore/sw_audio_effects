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
	// Default BiQuad Configuration (Reverb)
	BIQUAD_PARAM_S def_biquad_param_s = { DEF_FILT_MODE ,DEF_SAMP_FREQ ,DEF_SIG_FREQ ,DEF_QUAL_FACT };
	BIQUAD_PARAM_S lo_pass_param_s = { LO_PASS ,DEF_SAMP_FREQ ,(DEF_SIG_FREQ >> 1) ,DEF_QUAL_FACT };
	BIQUAD_PARAM_S hi_pass_param_s = { HI_PASS ,DEF_SAMP_FREQ ,(DEF_SIG_FREQ << 1) ,(DEF_QUAL_FACT << 2) };
	BIQUAD_PARAM_S band_pass_param_s = { BAND_PASS ,DEF_SAMP_FREQ ,(DEF_SIG_FREQ >> 3) ,(DEF_QUAL_FACT >> 1) };

	// Default Reverb Configuration parameters
	REVERB_PARAM_S def_reverb_param_s = {{ DEF_DRY_LVL ,DEF_FX_LVL ,DEF_FB_LVL ,DEF_CROSS_MIX } 
																,DEF_ROOM_SIZE ,DEF_SIG_FREQ ,DEF_SAMP_FREQ ,DEF_REVERB_GAIN };

	// Default Loudness Configuration parameters. NB Currently Unused.
	GAIN_PARAM_S def_gain_param_s = { DEF_GAIN };

	// Assign default configuration parameters
	dspfx_gs.gain_params = def_gain_param_s;
	dspfx_gs.biquad_params_rvrb = def_biquad_param_s;
	dspfx_gs.biquad_params_solo = band_pass_param_s;
	dspfx_gs.reverb_params = def_reverb_param_s;

	// Assing Effect names
	safestrcpy( dspfx_gs.fx_names[REVERB].str ,"Reverb" );
	safestrcpy( dspfx_gs.fx_names[BIQUAD].str ,"BiQuad" );

	dspfx_gs.fade_state	= START_F; // Initialise processing state
	dspfx_gs.fade_on = 0; // Initialise to Cross-fade OFF
	dspfx_gs.new_effect = NO_FX; // Initialise to No request
	dspfx_gs.cur_effect = REVERB; // Initialise audio effect
	dspfx_gs.fx_len = SWAP_NUM; // time spent in effect state (to ~8 secs)
	dspfx_gs.dry_len = SWAP_NUM;	// time spent in dry state (~8 secs)
	dspfx_gs.samp_cnt = 0;	// Sample counter

	printstrln( dspfx_gs.fx_names[dspfx_gs.cur_effect].str );
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
	BIQUAD_PARAM_S &cur_biquad_param_s, // Reference to structure containing biquad parameters
	streaming chanend c_dsp_eq // Channel connecting to Equalisation coar (bi-directional)
)
{
	soutct( c_dsp_eq ,CFG_BIQUAD ); // Signal BiQuad configuration data transmission

	c_dsp_eq <: cur_biquad_param_s; // Send BiQuad filter configuration data
} // send_biquad_config
/*****************************************************************************/
void send_loudness_config( // Send Loudness configuration data
	GAIN_PARAM_S &cur_gain_param_s, // Reference to current gain configuration parameters
	streaming chanend c_dsp_gain // Channel connecting to Loudness coar (bi-directional)
)
{
	soutct( c_dsp_gain ,CFG_GAIN ); // Signal Gain configuration data transmission

	c_dsp_gain <: cur_gain_param_s; // Send Loudness configuration data
} // send_loudness_config
/*****************************************************************************/
void config_dsp_effects( // Synchronise a reverb configuration (with other coars)
	DSP_EFFECT_S &dspfx_gs, // Reference to structure containing data to control DSP Effects
	streaming chanend c_dsp_eq[NUM_BIQUADS], // Channel connecting to Equalisation coar (bi-directional)
	streaming chanend c_dsp_gain // Channel connecting to Loudness coar (bi-directional)
)
{
	assert( NUM_BIQUADS > 1 ); // This configuration requires 2 BiQuad threads

	// Send data to other coars ...
	send_biquad_config( dspfx_gs.biquad_params_solo ,c_dsp_eq[0] );		// Send BiQuad filter configuration data
	send_biquad_config( dspfx_gs.biquad_params_rvrb ,c_dsp_eq[1] );		// Send BiQuad filter configuration data
	send_loudness_config( dspfx_gs.gain_params ,c_dsp_gain );	// Send Gain-shaping configuration data

	config_sdram_reverb( dspfx_gs.reverb_params ); // Configure remaining reverb parameters in this coar (Delay-line)

} // config_dsp_effects
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
			return BIQUAD;
		break; // case BUTTON_1 
	
		case BUTTON_2 : // port value returned for when just button 2 (SW2) pressed
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
void do_different_effect( // Updates states for when different effect requested
	DSP_EFFECT_S &dspfx_gs,  // Reference to structure containing data to control DSP Effects
	streaming chanend c_dsp_eq	// Channel connecting to Equalisation coar (bi-directional)
)
{
	// Re-configure for new effect
	switch( dspfx_gs.new_effect )
	{
		case REVERB:
			dspfx_gs.fade_state = BQ2REV_F; // Fade from BiQuad to Reverb state

			// Configure Reverb parameters. NB Currently only BiQuad needs re-configuring
			send_biquad_config( dspfx_gs.biquad_params_rvrb ,c_dsp_eq ); // Send BiQuad config. data for reverb
		break; // case REVERB

		case BIQUAD:
			dspfx_gs.fade_state = REV2BQ_F; // Fade from Reverb to BiQuad state

			// Configure BiQuad parameters. NB Currently only BiQuad needs re-configuring
			send_biquad_config( dspfx_gs.biquad_params_solo ,c_dsp_eq ); // Send BiQuad config. data for solo use
		break; // case BIQUAD
	} // switch( dspfx_gs.new_effect)
} // do_different_effect
/*****************************************************************************/
void do_same_effect( // Updates states for when same effect requested
	DSP_EFFECT_S &dspfx_gs  // Reference to structure containing data to control DSP Effects
)
{ // Toggle 'Fade-state' of current effect
	if (EFFECT_F == dspfx_gs.fade_state)
	{ // Currently in EFFECT_F state
		dspfx_gs.fade_state = FX2DRY_F;  // Fade-out effect
	} // if ((EFFECT_F != dspfx_gs.fade_state) && (FX2DRY_F != dspfx_gs.fade_state))
	else
	{ // Currently in DRY state
		dspfx_gs.fade_state = DRY2FX_F; // Fade-In Effect
	} // else !((EFFECT_F != dspfx_gs.fade_state) && (FX2DRY_F != dspfx_gs.fade_state))
} // do_same_effect
/*****************************************************************************/
void service_effect_change( // Attempts to service new effect request
	DSP_EFFECT_S &dspfx_gs,  // Reference to structure containing data to control DSP Effects
	streaming chanend c_dsp_eq	// Channel connecting to Equalisation coar (bi-directional)
)
/*	The new effect request will only be serviced if NOT doing a cross-fade.
 *	This helps reduce the number of states to consider, and reduces clicks when switching between effects.
 *	When switching effects modes a cross-fade is always used to prevent clicks.
 */
{
	// Check for cross-fade
	if (!dspfx_gs.fade_on)
	{
		// Check if Different Effect requested
		if (dspfx_gs.new_effect !=  dspfx_gs.cur_effect)
		{ 
			dspfx_gs.cur_effect = dspfx_gs.new_effect; // Update current effect
	
			do_different_effect( dspfx_gs ,c_dsp_eq ); 
		} // if (dspfx_gs.new_effect !=  dspfx_gs.prev_effect)
		else
		{
			do_same_effect( dspfx_gs ); 
		} // else !(dspfx_gs.new_effect !=  dspfx_gs.prev_effect)
	
		dspfx_gs.samp_cnt = 0; // Restart sample count for cross-fade
		dspfx_gs.fade_on = 1; // Set Cross-fade flag
		dspfx_gs.new_effect = NO_FX; // Clear new effect request
	} // if (!dspfx_gs.fade_on)
} // service_effect_change
/*****************************************************************************/
void check_for_effect_change( // Monitors GPIO buttons to see if effect change requested
	DSP_EFFECT_S &dspfx_gs,  // Reference to structure containing data to control DSP Effects
	streaming chanend c_dsp_eq,	// Channel connecting to Equalisation coar (bi-directional)
	chanend c_dsp_gpio // DSP end of channel between GPIO and DSP coars
)
{
	BUTTON_STATES_ENUM button_state; // button state


	// Check for new button state
	select
	{
		case c_dsp_gpio :> button_state :
			// Decode requested effect from button state, and store
			dspfx_gs.new_effect = decode_button_state( button_state );
		break; // case c_dsp_gpio :> button_state

		default:
			// Nothing to do. NB prevents blocking on above case
		break; // default
	} // select

} // check_for_effect_change
/*****************************************************************************/
void control_output_mix( // Controls Effect/Dry dynamic Output Mix
	DSP_EFFECT_S &dspfx_gs,  // Reference to structure containing data to control DSP Effects
	CHAN_SET_S &out_set_s,	// Reference to structure containing output audio sample-set
	CHAN_SET_S &biquad_set_s,	// Reference to structure containing BiQuad output sample-set
	CHAN_SET_S &reverb_set_s,	// Reference to structure containing Reverb output sample-set
	CHAN_SET_S &inp_set_s		// Reference to structure containing Input audio sample-set
)
{
	// Determine which output-mix state we are in, and act accordingly!-)
	switch(dspfx_gs.fade_state)
	{
		case EFFECT_F: // Full Effect (Send Effect to Output)
			out_set_s = reverb_set_s; // No fade so copy to output

			if (dspfx_gs.fx_len < dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = FX2DRY_F; // Switch to Fade-out Effect
				dspfx_gs.fade_on = 1; // Set Cross-fade flag
			} // if (dspfx_gs.fx_len < dspfx_gs.samp_cnt)
		break; // case EFFECT_F:

		case FX2DRY_F: // Fade-Out Effect (Crossfade Effect to Dry)
			cross_fade_sample( out_set_s.samps ,reverb_set_s.samps ,inp_set_s.samps ,NUM_REVERB_CHANS ,dspfx_gs.samp_cnt );

			if (FADE_LEN <= dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = DRY_ONLY_F; // Switch to Dry-Only Processing
				dspfx_gs.fade_on = 0; // Clear Cross-fade flag
				printstrln("Dry");
			} // if (SWAP_NUM < dspfx_gs.samp_cnt)
		break; // case FX2DRY_F:

		case DRY_ONLY_F: // No Effect (Send Input to Output)
			out_set_s = inp_set_s;
			
			if (dspfx_gs.dry_len < dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = DRY2FX_F; // Switch to Fade-In Effect
				dspfx_gs.fade_on = 1; // Set Cross-fade flag
			} // if (dspfx_gs.dry_len < dspfx_gs.samp_cnt)
		break; // case DRY_ONLY_F:

		case DRY2FX_F: // Fade-in Effect (Crossfade Dry to Effect)
			cross_fade_sample( out_set_s.samps ,inp_set_s.samps ,reverb_set_s.samps ,NUM_REVERB_CHANS ,dspfx_gs.samp_cnt );

			if (FADE_LEN <= dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = EFFECT_F; // Switch to Effect Processing
				dspfx_gs.fade_on = 0; // Clear Cross-fade flag
				printstrln( dspfx_gs.fx_names[dspfx_gs.cur_effect].str );
			} // if (SWAP_NUM < dspfx_gs.samp_cnt)
		break; // case DRY2FX_F:

		case BQ2REV_F: // Cross-Fade BiQuad to Reverb Effect
			cross_fade_sample( out_set_s.samps ,biquad_set_s.samps ,reverb_set_s.samps ,NUM_REVERB_CHANS ,dspfx_gs.samp_cnt );

			if (FADE_LEN <= dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = EFFECT_F; // Switch to Effect Processing
				dspfx_gs.fade_on = 0; // Clear Cross-fade flag
				printstrln( dspfx_gs.fx_names[dspfx_gs.cur_effect].str );
			} // if (SWAP_NUM < dspfx_gs.samp_cnt)
		break; // case BQ2REV_F 

		case REV2BQ_F: // Cross-Fade BiQuad to Reverb Effect
			cross_fade_sample( out_set_s.samps ,reverb_set_s.samps ,biquad_set_s.samps ,NUM_REVERB_CHANS ,dspfx_gs.samp_cnt );

			if (FADE_LEN <= dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = EFFECT_F; // Switch to Effect Processing
				dspfx_gs.fade_on = 0; // Clear Cross-fade flag
				printstrln( dspfx_gs.fx_names[dspfx_gs.cur_effect].str );
			} // if (SWAP_NUM < dspfx_gs.samp_cnt)
		break; // case REV2BQ_F

		case START_F: // Fade-in Effect
			fade_in_sample( out_set_s.samps ,reverb_set_s.samps ,NUM_REVERB_CHANS ,dspfx_gs.samp_cnt );

			if (FADE_LEN <= dspfx_gs.samp_cnt)
 			{
				dspfx_gs.samp_cnt = 0; // Reset sample counter
				dspfx_gs.fade_state = EFFECT_F; // Switch to Effect Processing
				dspfx_gs.fade_on = 0; // Clear Cross-fade flag
			} // if (SWAP_NUM < dspfx_gs.samp_cnt)
		break; // case START_F:

		default:
			assert(0 == 1); // ERROR: Unsupported state
		break; // default:
	} // switch(dspfx_gs.fade_state)

	dspfx_gs.samp_cnt++; // Update sample counter
} // control_output_mix
/*****************************************************************************/
void dsp_effects( // Controls audio stream processing for reverb application using dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O coar (bi-directional)
	streaming chanend c_dsp_eq[NUM_BIQUADS], // Arrays of channels connecting to Equalisation coar (bi-directional)
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
	CHAN_SET_S biquad_set_s;	// Structure containing BiQuad output sample-set
	CHAN_SET_S reverb_set_s;	// Structure containing Reverb output sample-set

	S32_T chan_cnt; // Channel counter


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
	reverb_set_s = inp_set_s;
	out_set_s = inp_set_s;

	config_dsp_effects( dspfx_gs ,c_dsp_eq ,c_dsp_gain );

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
		// 1st BiQuad channel for Solo Filter Effect
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_eq[0] <: inp_set_s.samps[chan_cnt]; // Send Input samples to EQ coar  
			c_dsp_eq[0] :> biquad_set_s.samps[chan_cnt]; // Receive Equalised samples back from EQ coar  
		} // for chan_cnt

		// 2nd BiQuad channel for Reverb Effect
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_eq[1] <: uneq_set_s.samps[chan_cnt]; // Send Unequalised samples to EQ coar  
			c_dsp_eq[1] :> sdram_gs.src_set.samps[chan_cnt]; // Receive Equalised samples back from EQ coar  
		} // for chan_cnt

#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_gain <: unamp_set_s.samps[chan_cnt]; // Send Unamplified samples to Loudness coar   
			c_dsp_gain :> ampli_set_s.samps[chan_cnt]; // Receive Amplified samples back from Loudness coar  

		} // for chan_cnt

		check_for_effect_change( dspfx_gs ,c_dsp_eq[1] ,c_dsp_gpio ); // Checks if GPIO buttons pressed

		// Check for pending effect request
		if (NO_FX != dspfx_gs.new_effect)
		{
			service_effect_change( dspfx_gs ,c_dsp_eq[1] );
		} // if (NO_FX != dspfx_gs.new_effect)

		// Call reverb routines. NB Still called even for BiQuad, ready for switch back to Reverb
		use_sdram_reverb( sdram_gs ,inp_set_s ,uneq_set_s ,unamp_set_s ,reverb_set_s ,ampli_set_s );
		buffer_check( sdram_gs ,c_dsp_sdram ); // Check if any buffer I/O required

		// Check for BiQuad EffectSelect DSP Effect Processing ...
		if (BIQUAD == dspfx_gs.cur_effect)
		{
			// Add BiQuad to original
			for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
			{
				biquad_set_s.samps[chan_cnt] += inp_set_s.samps[chan_cnt];
			}	// for chan_cnt
		} // if (BIQUAD == dspfx_gs.cur_effect)

		control_output_mix( dspfx_gs ,out_set_s ,biquad_set_s ,reverb_set_s ,inp_set_s ); // Select Effect/Dry Output mix

	} // while(1)

} // dsp_effects
/*****************************************************************************/
// dsp_effects.xc
