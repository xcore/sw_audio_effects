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
//	BIQUAD_PARAM_S hi_pass_param_s = { HI_PASS ,DEF_SAMP_FREQ ,(DEF_SIG_FREQ << 1) ,(DEF_QUAL_FACT << 2) };
//	BIQUAD_PARAM_S band_pass_param_s = { BAND_PASS ,DEF_SAMP_FREQ ,(DEF_SIG_FREQ >> 3) ,(DEF_QUAL_FACT >> 1) }; // MB~Clips

	// Default Reverb Configuration parameters
	REVERB_PARAM_S def_reverb_param_s = {{ DEF_DRY_LVL ,DEF_FX_LVL ,DEF_FB_LVL ,DEF_CROSS_MIX } 
																,DEF_ROOM_SIZE ,DEF_SIG_FREQ ,DEF_SAMP_FREQ ,DEF_REVERB_GAIN };

	// Default Loudness Configuration parameters. NB Currently Unused.
	GAIN_PARAM_S def_gain_param_s = { DEF_GAIN };


	S32_T sig_cnt; // Signal counter


	// Assign default configuration parameters
	dspfx_gs.reverb_params = def_reverb_param_s;
	dspfx_gs.gain_params = def_gain_param_s;

	dspfx_gs.biquad_params_rvrb = def_biquad_param_s;
	dspfx_gs.biquad_params_solo = lo_pass_param_s;

	// Initialise signal names
	for (sig_cnt = 0; sig_cnt < NUM_OUT_SIGNALS; sig_cnt++)
	{
		safestrcpy( dspfx_gs.fx_names[sig_cnt].str ,"NoDefn" );
	}	// for sig_cnt

	// Assign Signal names
	safestrcpy( dspfx_gs.fx_names[INPUT].str ,"Dry" );
	safestrcpy( dspfx_gs.fx_names[BIQUAD].str ,"BiQuad" );
	safestrcpy( dspfx_gs.fx_names[REVERB].str ,"Reverb" );

	dspfx_gs.fade_on = 0; // Initialise to Cross-fade OFF
	dspfx_gs.dry_state	= FX_ON; // Initialise processing state
	dspfx_gs.old_sig = INPUT; // Initialise old audio effect
	dspfx_gs.cur_sig = REVERB; // Initialise current audio effect
	dspfx_gs.new_effect = REVERB; // Initialise new audio effect
	dspfx_gs.pending = 0; // Clear New Effect Request pending
	dspfx_gs.fx_len = SWAP_NUM; // time spent in effect state (to ~8 secs)
	dspfx_gs.samp_cnt = 0;	// Sample counter

	printstrln( dspfx_gs.fx_names[dspfx_gs.cur_sig].str );
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
	streaming chanend c_dsp_eq_arr[NUM_APP_BIQUADS], // Array of channels connecting to Equalisation coars (bi-directional)
	streaming chanend c_dsp_gain // Channel connecting to Loudness coar (bi-directional)
)
{
	assert( NUM_APP_BIQUADS > 1 ); // This configuration requires 2 BiQuad threads

	// Send data to other coars ...
	send_biquad_config( dspfx_gs.biquad_params_solo ,c_dsp_eq_arr[0] );		// Send BiQuad filter configuration data for solo effect
	send_biquad_config( dspfx_gs.biquad_params_rvrb ,c_dsp_eq_arr[1] );		// Send BiQuad filter configuration data for reverb effect
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
OUT_SIGNAL_ENUM decode_button_state( // Analyse new button state, and return requested audio effect type
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
			// Decode requested effect from button state, and store
			dspfx_gs.new_effect = decode_button_state( button_state );
			dspfx_gs.pending = 1; // Set New Effect request pending flag
		break; // case c_dsp_gpio :> button_state

		default:
			// Nothing to do. NB prevents blocking on above case
		break; // default
	} // select

} // check_for_effect_change
/*****************************************************************************/
void service_user_effect_change( // Attempt to service new user effect request
	DSP_EFFECT_S &dspfx_gs  // Reference to structure containing data to control DSP Effects
)
/*	The new effect request will only be serviced if NOT doing a cross-fade.
 *	This helps reduce the number of states to consider, and reduces clicks when switching between effects.
 *	When switching effects a cross-fade is always used to prevent clicks.
 */
{
	dspfx_gs.old_sig = dspfx_gs.cur_sig; // Store old output signal state

	// Update Dry/Effect state based on signal history
	switch( dspfx_gs.dry_state )
	{
		case FX_ON : // Current Effect is active
			// Check for Effect change
			if (dspfx_gs.new_effect != dspfx_gs.cur_sig)
			{ // Different from previous, so change Effect, but stay in FX_ON state
				dspfx_gs.cur_sig = dspfx_gs.new_effect; // Assign current output signal to New Effect
			} // if (dspfx_gs.new_effect !=  dspfx_gs.cur_sig)
			else
			{ // Same as previous, so toggle to Dry (No Effect) state
				dspfx_gs.dry_state = DRY_ON;
				dspfx_gs.cur_sig = INPUT; // Assign current output signal the Input(Dry) Signal
			} // else !(dspfx_gs.new_effect !=  dspfx_gs.cur_sig)
		break; // case FX_ON

		case DRY_ON : // Dry signal is active
			// Switch on new effect
			dspfx_gs.dry_state = FX_ON;
			dspfx_gs.cur_sig = dspfx_gs.new_effect; // Assign current output signal to New Effect
		break; // case FX_ON

		default :
			assert( 0 == 1); // ERROR: Unsupported Dry/Effect state
		break; // default
	} // switch( dspfx_gs.dry_state )

	dspfx_gs.samp_cnt = 0; // Restart sample count for cross-fade
	dspfx_gs.fade_on = 1; // Set Cross-fade flag
	dspfx_gs.pending = 0; // Clear new effect request pending
} // service_user_effect_change
/*****************************************************************************/
void service_timed_effect_change( // service timed effect change
	DSP_EFFECT_S &dspfx_gs  // Reference to structure containing data to control DSP Effects
)
/*	The timed effect change will only be serviced if NOT doing a cross-fade.
 *	This helps reduce the number of states to consider, and reduces clicks when switching between effects.
 *	When switching effects a cross-fade is always used to prevent clicks.
 */
{
	// Toggle Dry/Effect signals
	switch( dspfx_gs.dry_state )
	{
		case FX_ON : // Current Effect is active
			// Toggle to Dry signal
			dspfx_gs.dry_state = DRY_ON;
			dspfx_gs.old_sig = dspfx_gs.cur_sig;
			dspfx_gs.cur_sig = INPUT; // Assign current output signal the Input(Dry) Signal
		break; // case FX_ON

		case DRY_ON : // Dry signal is active
			// Toggle to Effect signal
			dspfx_gs.dry_state = FX_ON;
			dspfx_gs.cur_sig = dspfx_gs.old_sig; // Assign current output signal the Old output signal
			dspfx_gs.old_sig = INPUT; // Assign old output signal the input(dry) signal
		break; // case FX_ON

		default :
			assert( 0 == 1); // ERROR: Unsupported Dry/Effect state
		break; // default
	} // switch( dspfx_gs.dry_state )

	dspfx_gs.samp_cnt = 0; // Restart sample count for cross-fade
	dspfx_gs.fade_on = 1; // Set Cross-fade flag
} // service_timed_effect_change
/*****************************************************************************/
void control_crossfade_mix( // Controls dynamic Cross-fade mix of inputs to output
	DSP_EFFECT_S &dspfx_gs,  // Reference to structure containing data to control DSP Effects
	CHAN_SET_S &out_set_s,	// Reference to structure containing output audio sample-set
	CHAN_SET_S &old_set_s,	// Reference to structure containing old input sample-set
	CHAN_SET_S &cur_set_s		// Reference to structure containing current input sample-set
)
{
	// Determine which output-mix state we are in, and act accordingly!-)
	if (dspfx_gs.fade_on)
	{ // Do Cross-fade
		cross_fade_sample( out_set_s.samps ,old_set_s.samps ,cur_set_s.samps ,NUM_REVERB_CHANS ,dspfx_gs.samp_cnt );

		// Check for end of Fade
		if (FADE_LEN <= dspfx_gs.samp_cnt)
		{
			dspfx_gs.samp_cnt = 0; // Reset sample counter
			dspfx_gs.fade_on = 0; // Clear Cross-fade flag
			printstrln( dspfx_gs.fx_names[dspfx_gs.cur_sig].str ); // Print new signal name
		} // if (SWAP_NUM < dspfx_gs.samp_cnt)
	} // if( dspfx_gs.fade_on)
	else
	{ // No cross-fade
		out_set_s = cur_set_s; // No fade so copy current input to output
	} // else !(dspfx_gs.fade_on)

	dspfx_gs.samp_cnt++; // Update sample counter
} // control_crossfade_mix
/*****************************************************************************/
void dsp_effects( // Controls audio stream processing for Reverb & BiQuad dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O coar (bi-directional)
	streaming chanend c_dsp_eq_arr[NUM_APP_BIQUADS], // Arrays of channels connecting to Equalisation coars (bi-directional)
	streaming chanend c_dsp_gain, // Channel connecting to Loudness coar (bi-directional)
  chanend c_dsp_sdram, // DSP end of channel between DSP coar and SDRAM coar (bi-directional)
	chanend c_dsp_gpio // DSP end of channel between GPIO and DSP coars
)
/* Both the Reverb and BiQuad effects are constantly being generated.
 * The Buttons on the GPIO board are used to select which of the Reverb/BiQuad/Dry streams
 * are sent to the output.
 * When a switch between states is made, a cross-fade it used to prevent clicks in the audio.
 * It is NOT possible to switch states again, until the cross-fade is complete.
 * Again, this is to prevent clicks in the output stream.
 */
{
	CNTRL_SDRAM_S sdram_gs; // Structure containing data to control SDRAM buffering
	DSP_EFFECT_S dspfx_gs;  // Structure containing data to control DSP Effects
	CHAN_SET_S uneq_set_s;	// Structure containing Unequalised audio sample-set
	CHAN_SET_S unamp_set_s;	// Structure containing Unamplified audio sample-set
	CHAN_SET_S ampli_set_s;	// Structure containing Amplified audio sample-set
	CHAN_SET_S out_set_s;	// Structure containing output audio sample-set
	CHAN_SET_S fx_sets[NUM_OUT_SIGNALS];	// Array of possible output sample-sets

	S32_T chan_cnt; // Channel counter
	S32_T sig_cnt; // Signal counter


	init_sdram_buffers( sdram_gs ); // Initialise buffers for SDRAM access
	init_dsp_effects( dspfx_gs ); // Initialise DSP Effects

	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
	{
		out_set_s.samps[chan_cnt] = 0;
	}	// for chan_cnt

	// initialise samples buffers
	for (sig_cnt = 0; sig_cnt < NUM_OUT_SIGNALS; sig_cnt++)
	{
		fx_sets[sig_cnt] = out_set_s;
	}	// for sig_cnt

	uneq_set_s = out_set_s;
	unamp_set_s = out_set_s;
	ampli_set_s = out_set_s;

	// Configure DSP Effects
	config_dsp_effects( dspfx_gs ,c_dsp_eq_arr ,c_dsp_gain );

	// Loop forever
	while(1)
	{
		// Send/Receive audio samples over all channels ...

#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_aud_dsp :> fx_sets[INPUT].samps[chan_cnt];  // Receive input samples from Audio I/O coar 
			c_aud_dsp <: out_set_s.samps[chan_cnt];  // Send Output samples back to Audio I/O coar 
		} // for chan_cnt

#pragma loop unroll
		// 1st BiQuad channel for Solo Filter Effect
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_eq_arr[0] <: fx_sets[INPUT].samps[chan_cnt]; // Send Input samples to EQ coar  
			c_dsp_eq_arr[0] :> fx_sets[BIQUAD].samps[chan_cnt]; // Receive Equalised samples back from EQ coar  
		} // for chan_cnt

#pragma loop unroll
		// 2nd BiQuad channel for Reverb Effect
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_eq_arr[1] <: uneq_set_s.samps[chan_cnt]; // Send Unequalised samples to EQ coar  
			c_dsp_eq_arr[1] :> sdram_gs.src_set.samps[chan_cnt]; // Receive Equalised samples back from EQ coar  
		} // for chan_cnt

#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_gain <: unamp_set_s.samps[chan_cnt]; // Send Unamplified samples to Loudness coar   
			c_dsp_gain :> ampli_set_s.samps[chan_cnt]; // Receive Amplified samples back from Loudness coar  
		} // for chan_cnt

		// Check if GPIO buttons have been pressed
		check_for_effect_change( dspfx_gs ,c_dsp_gpio );

		// If NOT doing cross-fade, service any effect/signal changes
		if (!dspfx_gs.fade_on)
		{
			// Check for pending user effect request
			if (dspfx_gs.pending)
			{
				service_user_effect_change( dspfx_gs );
			} // if (dspfx_gs.pending)

			// Check if time to switch to Dry/Effect signals
			if (dspfx_gs.fx_len < dspfx_gs.samp_cnt)
			{
				service_timed_effect_change( dspfx_gs );
			} // if (dspfx_gs.fx_len < dspfx_gs.samp_cnt)
		} // if (!dspfx_gs.fade_on)

		/* Call reverb routines. 
		 * As reverb is a delay based effect, it is still called even when NOT selected, ready for the switch back to Reverb
		 */
		use_sdram_reverb( sdram_gs ,fx_sets[INPUT] ,uneq_set_s ,unamp_set_s ,fx_sets[REVERB] ,ampli_set_s );
		buffer_check( sdram_gs ,c_dsp_sdram ); // Check if any buffer I/O required

		// Check for BiQuad Effect Processing ...
		if (BIQUAD == dspfx_gs.cur_sig)
		{
			// Mix BiQuad with original
			for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
			{
				fx_sets[BIQUAD].samps[chan_cnt] = (fx_sets[BIQUAD].samps[chan_cnt] + fx_sets[INPUT].samps[chan_cnt]) >> 1;
			}	// for chan_cnt
		} // if (BIQUAD == dspfx_gs.cur_sig)

		// Control Cross-fade to Output mix
		control_crossfade_mix( dspfx_gs ,out_set_s ,fx_sets[dspfx_gs.old_sig] ,fx_sets[dspfx_gs.cur_sig] );
	} // while(1)

} // dsp_effects
/*****************************************************************************/
// dsp_effects.xc
