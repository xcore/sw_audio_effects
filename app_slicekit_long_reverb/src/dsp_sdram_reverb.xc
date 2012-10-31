/******************************************************************************\
 * File:	dsp_sdram_reverb.xc
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

#include "dsp_sdram_reverb.h"

// DSP-control coar.

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
	} // if (cntrl_gs.do_write)

	// Check if any read-buffers are empty
	for (tap_cnt=0; tap_cnt<NUM_REVERB_TAPS; tap_cnt++)
	{
		if (cntrl_gs.reads[tap_cnt].do_buf)
		{
			read_buffer_from_sdram( c_dsp_sdram ,cntrl_gs.reads[tap_cnt].mem_adr ,cntrl_gs.reads[tap_cnt].buf_adr ,cntrl_gs.reads[tap_cnt].wrd_siz );

			cntrl_gs.reads[tap_cnt].do_buf = 0; // Clear read flag
		} // if (cntrl_gs.do_read)
	} //for tap_cnt

} // buffer_check
/*****************************************************************************/
void dsp_sdram_reverb( // Controls audio stream processing for reverb application using dsp functions
	streaming chanend c_aud_dsp, // Channel connecting to Audio I/O coar (bi-directional)
	streaming chanend c_dsp_eq, // Channel connecting to Equalisation coar (bi-directional)
	streaming chanend c_dsp_gain, // Channel connecting to Loudness coar (bi-directional)
  chanend c_dsp_sdram // DSP end of channel between DSP coar and SDRAM coar (bi-directional)
)
{
	CNTRL_SDRAM_S cntrl_gs; // Structure containing data to control SDRAM buffering
	CHAN_SET_S inp_set_s;	// Structure containing Input audio sample-set
	CHAN_SET_S uneq_set_s;	// Structure containing Unequalised audio sample-set
	CHAN_SET_S unamp_set_s;	// Structure containing Unamplified audio sample-set
	CHAN_SET_S ampli_set_s;	// Structure containing Amplified audio sample-set
	CHAN_SET_S out_set_s;	// Structure containing output audio sample-set
	CHAN_SET_S fade_set_s;	// Structure containing Cross-fade audio sample-set

	S32_T dry_len = SWAP_NUM;	// time spent in dry state (~8 secs)
	S32_T fx_len = SWAP_NUM;//	(SWAP_NUM << 1); // time spent in effect state (to ~16 secs)
	S32_T samp_cnt = 0;	// Sample counter
	S32_T chan_cnt; // Channel counter

	PROC_STATE_TYP cur_proc_state	= START; // Initialise processing state
	// Default Reverb parameters
	REVERB_PARAM_S def_param_s = {{ DEF_DRY_LVL ,DEF_FX_LVL ,DEF_FB_LVL ,DEF_CROSS_MIX } 
																,DEF_ROOM_SIZE ,DEF_SIG_FREQ ,DEF_SAMP_FREQ ,DEF_GAIN };


	init_sdram_buffers( cntrl_gs ); // Initialise buffers for SDRAM access

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
			c_dsp_eq :> cntrl_gs.src_set.samps[chan_cnt]; // Receive Equalised samples back from EQ coar  
		} // for chan_cnt

#pragma loop unroll
		for (chan_cnt = 0; chan_cnt < NUM_REVERB_CHANS; chan_cnt++)
		{
			// Service channels in chronological order
			c_dsp_gain <: unamp_set_s.samps[chan_cnt]; // Send Unamplified samples to Loudness coar   
			c_dsp_gain :> ampli_set_s.samps[chan_cnt]; // Receive Amplified samples back from Loudness coar  

		} // for chan_cnt

		samp_cnt++; // Update sample counter

		// Do DSP Processing ...
		use_sdram_reverb( cntrl_gs ,inp_set_s ,uneq_set_s ,unamp_set_s ,fade_set_s ,ampli_set_s );

		buffer_check( cntrl_gs ,c_dsp_sdram ); // Check if any buffer I/O required

		// Check current processing State
		switch(cur_proc_state)
		{
			case EFFECT: // Do Effect processing
				out_set_s = fade_set_s; // No fade to copy to output

				if (fx_len < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = FX2DRY; // Switch to Fade-out Effect
//					cur_proc_state = EFFECT; // MB~ Dbg
				} // if (fx_len < samp_cnt)
			break; // case EFFECT:

			case FX2DRY: // Fade-Out Effect
				cross_fade_sample( out_set_s.samps ,fade_set_s.samps ,inp_set_s.samps ,NUM_REVERB_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY_ONLY; // Switch to Dry-Only Processing
					printstrln("Dry");
				} // if (SWAP_NUM < samp_cnt)
			break; // case FX2DRY:

			case DRY_ONLY: // No Effect (Dry signal only)
				out_set_s = inp_set_s;

				if (dry_len < samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = DRY2FX; // Switch to Fade-In Effect

				} // if (dry_len < samp_cnt)
			break; // case DRY_ONLY:

			case DRY2FX: // Fade-in Effect
				cross_fade_sample( out_set_s.samps ,inp_set_s.samps ,fade_set_s.samps ,NUM_REVERB_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = EFFECT; // Switch to Effect Processing
					printstrln("Effect");
				} // if (SWAP_NUM < samp_cnt)
			break; // case DRY2FX:

			case START: // Fade-in Effect
				fade_in_sample( out_set_s.samps ,fade_set_s.samps ,NUM_REVERB_CHANS ,samp_cnt );

				if (FADE_LEN <= samp_cnt)
	 			{
					samp_cnt = 0; // Reset sample counter
					cur_proc_state = EFFECT; // Switch to Effect Processing
				} // if (SWAP_NUM < samp_cnt)
			break; // case DRY2FX:

			default:
				assert(0 == 1); // ERROR: Unsupported state
			break; // default:
		} // switch(cur_proc_state)

	} // while(1)

} // dsp_sdram_reverb
/*****************************************************************************/
// dsp_sdram_reverb.xc
