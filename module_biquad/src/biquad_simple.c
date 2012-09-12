/******************************************************************************\
 * File:	biquad_simple.c
 * Author: Mark Beaumont
 * Description: Functions for Bi-Quad filter
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

#include "biquad_simple.h"

// Structure containing all BiQuad data (NB Made global to avoid using malloc)
static BIQUAD_S biquad_gs = { .init_done = 0, .params_set = 0 }; // Clear initialisation flags

/******************************************************************************/
void scale_coef( // Scale and round floating point coeffiecient
	FIX_POINT_S * fix_coef_ps, // pointer to structure containing coef in fixed point format
	REAL_T un_coef // input unscaled floating point coef.
) // return scaled integer coef
{
	S32_T sign_coef = 1; // sign of coef. preset to postive. NB S8_T has bug


	fix_coef_ps->err = 0;
	fix_coef_ps->exp = 0;

	// Check for special case of zero coef.
	if (0 == un_coef )
	{
		fix_coef_ps->mant = 0;
		fix_coef_ps->half = 0;
	} // if (0 == un_coef )
	else
	{	// Non-zero coef.

		// Create absolute and sign components
		if (un_coef < 0)
		{
			sign_coef = -1; // Update sign
			un_coef = -un_coef; // NB un_coef is now absolute value
		} // if (un_coef < 0)

		// Loop while mantissa NOT at full accuracy
		while (un_coef < (FILT_T)0x40000000)
		{
			fix_coef_ps->exp++; // Increment exponent
			un_coef *= 2; // Double input
		} // while (scale_coef < 0x40000000)

		// Coef should now be in range 0x4000_0000 .. 0x7fff_ffff
		fix_coef_ps->mant = sign_coef * (COEF_T)floor( (REAL_T)0.5 + un_coef ); // calculate signed mantissa

		// Check for scaling factor larger than unity
		if (0 < fix_coef_ps->exp)
		{ // Evaluate rounding value
			fix_coef_ps->half = (1 <<  (fix_coef_ps->exp - 1)); // Half scaling factor
		} // if (0 < fix_coef_ps->exp)
	} // else !(0 == un_coef )
  
} // scale_coef
/******************************************************************************/
void init_lopass_coefs( // Initialise set of BiQuad coeffs for Low-Pass Filter
	BIQUAD_PARAM_S * cur_param_ps, // Pointer to structure containing current biquad filter parameters
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[] // Array of un-normalised IIR filter coefficients
)
{
	// NB Example values given for cur_param_ps->sig_freq = 1KHz, cur_param_ps->samp_freq = 48KHz 
	REAL_T ang_w0 = 2 * PI * cur_param_ps->sig_freq / cur_param_ps->samp_freq; // Angular position 0.1309
	REAL_T sin_w0 = sin( ang_w0 ); // 0.1305
	REAL_T cos_w0 = cos( ang_w0 ); // 0.9914
	REAL_T alpha = ((REAL_T)DEF_QUAL_FACT * sin_w0) / (2 * cur_param_ps->qual); // 0.06526


	assert (3 == NUM_FILT_TAPS); // Check correct array size for BiQuad

	// Assign Non-zero Unnormalised Coefs
	un_b[0] = (1 - cos_w0)/2; // 0.004278
	un_b[1] = (1 - cos_w0);	// 0.008555
	un_b[2] = (1 - cos_w0)/2; // 0.004278

	un_a[0] = (1 + alpha); // 1.065
	un_a[1] = (-2 * cos_w0); // -1.983
	un_a[2] = (1 - alpha); // 0.9347

} // init_lopass_coefs
/******************************************************************************/
void init_hipass_coefs( // Initialise set of BiQuad coeffs for Hi-Pass Filter
	BIQUAD_PARAM_S * cur_param_ps, // Pointer to structure containing current biquad filter parameters
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[] // Array of un-normalised IIR filter coefficients
)
{
	// NB Example values given for cur_param_ps->sig_freq= 1KHz, cur_param_ps->samp_freq = 48KHz 
	REAL_T ang_w0 = 2 * PI * cur_param_ps->sig_freq / cur_param_ps->samp_freq; // Angular position 0.1309
	REAL_T sin_w0 = sin( ang_w0 ); // 0.1305
	REAL_T cos_w0 = cos( ang_w0 ); // 0.9914
	REAL_T alpha = ((REAL_T)DEF_QUAL_FACT * sin_w0) / (2 * cur_param_ps->qual); // 0.06526


	assert (3 == NUM_FILT_TAPS); // Check correct array size for BiQuad

	// Assign Non-zero Unnormalised Coefs
	un_b[0] = (1 + cos_w0)/2; // 0.9957
	un_b[1] = -(1 + cos_w0);	// -1.991
	un_b[2] = (1 + cos_w0)/2; // 0.9957

	un_a[0] = (1 + alpha); // 1.065
	un_a[1] = (-2 * cos_w0); // -1.983
	un_a[2] = (1 - alpha); // 0.9347

} // init_hipass_coefs
/******************************************************************************/
void init_bandpass_coefs( // Initialise set of BiQuad coeffs for Band-Pass Filter (Zero Peak Gain)
	BIQUAD_PARAM_S * cur_param_ps, // Pointer to structure containing current biquad filter parameters
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[] // Array of un-normalised IIR filter coefficients
)
{
	// NB Example values given for cur_param_ps->sig_freq= 1KHz, cur_param_ps->samp_freq = 48KHz 
	REAL_T ang_w0 = 2 * PI * cur_param_ps->sig_freq / cur_param_ps->samp_freq; // Angular position 0.1309
	REAL_T sin_w0 = sin( ang_w0 ); // 0.1305
	REAL_T cos_w0 = cos( ang_w0 ); // 0.9914
	REAL_T alpha = ((REAL_T)DEF_QUAL_FACT * sin_w0) / (2 * cur_param_ps->qual); // 0.06526


	assert (3 == NUM_FILT_TAPS); // Check correct array size for BiQuad

	// Assign Non-zero Unnormalised Coefs
	un_b[0] = alpha;	// 0.06526
	un_b[1] = 0;			// 0
	un_b[2] = -alpha; // -0.06526 

	un_a[0] = (1 + alpha); // 1.065
	un_a[1] = (-2 * cos_w0); // -1.983
	un_a[2] = (1 - alpha); // 0.9347

} // init_bandpass_coefs
/******************************************************************************/
void init_notch_coefs( // Initialise set of BiQuad coeffs for Notch Filter
	BIQUAD_PARAM_S * cur_param_ps, // Pointer to structure containing current biquad filter parameters
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[] // Array of un-normalised IIR filter coefficients
)
{
	// NB Example values given for cur_param_ps->sig_freq= 1KHz, cur_param_ps->samp_freq = 48KHz 
	REAL_T ang_w0 = 2 * PI * cur_param_ps->sig_freq / cur_param_ps->samp_freq; // Angular position 0.1309
	REAL_T sin_w0 = sin( ang_w0 ); // 0.1305
	REAL_T cos_w0 = cos( ang_w0 ); // 0.9914
	REAL_T alpha = ((REAL_T)DEF_QUAL_FACT * sin_w0) / (2 * cur_param_ps->qual); // 0.06526


	assert (3 == NUM_FILT_TAPS); // Check correct array size for BiQuad

	// Assign Non-zero Unnormalised Coefs
	un_b[0] = 1;							// 1.0
	un_b[1] = (-2 * cos_w0);	// -1.983 
	un_b[2] = 1;							// 1.0

	un_a[0] = (1 + alpha); // 1.065
	un_a[1] = (-2 * cos_w0); // -1.983
	un_a[2] = (1 - alpha); // 0.9347

} // init_notch_coefs
/******************************************************************************/
void init_allpass_coefs( // Initialise set of BiQuad coeffs for All-Pass Filter (Phase change only)
	BIQUAD_PARAM_S * cur_param_ps, // Pointer to structure containing current biquad filter parameters
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[] // Array of un-normalised IIR filter coefficients
)
{
	// NB Example values given for cur_param_ps->sig_freq= 1KHz, cur_param_ps->samp_freq = 48KHz 
	REAL_T ang_w0 = 2 * PI * cur_param_ps->sig_freq / cur_param_ps->samp_freq; // Angular position 0.1309
	REAL_T sin_w0 = sin( ang_w0 ); // 0.1305
	REAL_T cos_w0 = cos( ang_w0 ); // 0.9914
	REAL_T alpha = ((REAL_T)DEF_QUAL_FACT * sin_w0) / (2 * cur_param_ps->qual); // 0.06526


	assert (3 == NUM_FILT_TAPS); // Check correct array size for BiQuad

	// Assign Non-zero Unnormalised Coefs
	un_b[0] = (1 -alpha);	// 0.9347
	un_b[1] = (-2 * cos_w0); // -1.983
	un_b[2] = (1 + alpha); // 1.065

	un_a[0] = (1 + alpha); // 1.065
	un_a[1] = (-2 * cos_w0); // -1.983
	un_a[2] = (1 - alpha); // 0.9347

} // init_allpass_coefs
/******************************************************************************/
void init_custom_coefs( // Initialise set of coeffs to customised values (NB Used for debug)
	BIQUAD_PARAM_S * cur_param_ps, // Pointer to structure containing current biquad filter parameters
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[] // Array of un-normalised IIR filter coefficients
)
{
	S32_T tap_cnt; // filter-tap counter


	un_b[0] = 1;
	for (tap_cnt=1; tap_cnt<NUM_FILT_TAPS; tap_cnt++)
	{
		un_b[tap_cnt] = 1;
		un_a[tap_cnt] = -1;
	} // for tap_cnt

	// Sum normalisation value
	un_a[0] = un_b[0];
	for (tap_cnt=1; tap_cnt<NUM_FILT_TAPS; tap_cnt++)
	{
		un_a[0] += (un_b[tap_cnt] - un_a[tap_cnt]);
	} // for tap_cnt

} // init_custom_coefs
/******************************************************************************/
void init_common_coefs( // Initialise Common BiQuad Coeffs. Edit as to select required Filter
	BIQUAD_COEF_S * bq_coef_ps, // pointer to structure containing BiQuad coefficients for current channel
	BIQUAD_PARAM_S * cur_param_ps // Pointer to structure containing current biquad filter parameters
)
{
	REAL_T un_b[NUM_FILT_TAPS]; // Array of un-normalised FIR filter coefficients
	REAL_T un_a[NUM_FILT_TAPS]; // Array of un-normalised IIR filter coefficients
	S32_T tap_cnt; // filter-tap counter


	// Check for sensible sample frequency
	if (cur_param_ps->samp_freq < MIN_SAMP_FREQ)
	{
		cur_param_ps->samp_freq = MIN_SAMP_FREQ;
	} // if (cur_param_ps->samp_freq < MIN_SAMP_FREQ)

	// Clear coef arrays
	for (tap_cnt=0; tap_cnt<NUM_FILT_TAPS; tap_cnt++)
	{
		un_b[tap_cnt] = 0;
		un_a[tap_cnt] = 0;
		scale_coef( &(bq_coef_ps->b[tap_cnt]) ,(FILT_T)0.0 );
		scale_coef( &(bq_coef_ps->a[tap_cnt]) ,(FILT_T)0.0 );
	} // for tap_cnt

	// Check current Filter-Mode
	switch(cur_param_ps->filt_mode)
	{
		case LO_PASS: // Initialise Low-Pass Filter
			init_lopass_coefs( cur_param_ps ,un_b ,un_a );
		break; // case LO_PASS:

		case HI_PASS: // Initialise High-Pass Filter
			init_hipass_coefs( cur_param_ps ,un_b ,un_a );
		break; // case HI_PASS:

		case BAND_PASS: // Initialise Band-Pass Filter
			init_bandpass_coefs( cur_param_ps ,un_b ,un_a );
		break; // case BAND_PASS:

		case BAND_STOP: // Initialise Band-Stop (Notch) Filter
			init_notch_coefs( cur_param_ps ,un_b ,un_a );
		break; // case BAND_STOP:

		case ALL_PASS: // Initialise Phase-shift Filter
			init_allpass_coefs( cur_param_ps ,un_b ,un_a );
		break; // case ALL_PASS:

		case CUSTOM: // Initialise Custom Filter
			init_custom_coefs( cur_param_ps ,un_b ,un_a );
		break; // case CISTOM_PASS:

		default:
			assert(0 == 1); // ERROR: Filter-Mode
		break; // default:
	} // switch(cur_proc_state)

	assert( 0 != un_a[0] ); // Check for divide by zero

	// Normalise and store Coefs
	for (tap_cnt=0; tap_cnt<NUM_FILT_TAPS; tap_cnt++)
	{
		scale_coef( &(bq_coef_ps->b[tap_cnt]) ,(REAL_T)( un_b[tap_cnt] / un_a[0] ) );
		scale_coef( &(bq_coef_ps->a[tap_cnt]) ,(REAL_T)( un_a[tap_cnt] / un_a[0] ) );
	} // for tap_cnt
} // init_common_coefs
/******************************************************************************/
void init_biquad_chan( // Initialise BiQuad data for one channel
	BIQUAD_CHAN_S * bq_chan_ps // Pointer to structure containing current BiQuad data
)
{
	S32_T tap_cnt; // filter-tap counter


	for (tap_cnt=0; tap_cnt<NUM_FILT_TAPS; tap_cnt++)
	{
		bq_chan_ps->iir[tap_cnt] = 0;
		bq_chan_ps->inp[tap_cnt] = 0;
		bq_chan_ps->filt[tap_cnt] = 0;
	} // for tap_cnt

	bq_chan_ps->iir_err = 0; // Clear IIR Error
	bq_chan_ps->inp_err = 0; // Clear Input Error
	bq_chan_ps->filt_err = 0; // Clear Output Error
} // init_biquad_chan
/******************************************************************************/
void init_biquad( // Create structure for all biquad data, and initialise
	BIQUAD_S * biquad_ps // Pointer to structure containing all biquad data
) // Return pointer to BiQuad structure
{
	S32_T chan_cnt; // filter-tap counter


	// Loop through all output channels
	for (chan_cnt=0; chan_cnt<NUM_BIQUAD_CHANS; chan_cnt++)
	{
		init_biquad_chan( &(biquad_ps->bq_chan_s[chan_cnt]) );
	} // for chan_cnt

} // init_biquad
/******************************************************************************/
void config_biquad_filter( // Configure BiQuad filter 
	BIQUAD_PARAM_S * cur_param_ps // Pointer to structure containing current biquad filter parameters
)
{
	// Check if BiQuad filter initialised
	if (0 == biquad_gs.init_done)
	{ // Initialse BiQuad filter
  	init_biquad( &biquad_gs );	// Initialise biquad data structure

		biquad_gs.init_done = 1; // Signal Bi-Quad filter initialised
	} // if (0 == biquad_gs->init_done)

// printstr("FM= "); printintln( (int)cur_param_ps->filt_mode ); // MB~

	/* When coefficients are re-calculated mid-stream timing may be broken, but works so far!
   * Audio clicks can be heard, but I assume normally this would only occur at a program change,
	 * so this would mask the click.
	 */

	// recalculate filter coefficiants based on new sample frequency
  init_common_coefs( &(biquad_gs.common_coefs_s) ,cur_param_ps );

	biquad_gs.params_set = 1; // Signal BiQuad filter parameters configured
} // config_biquad_filter
/******************************************************************************/
FILT_T fix_point_mult( // Multiply a sample by a fixed point coefficient
	FIX_POINT_S * fix_coef_ps, // pointer to structure containing coef in fixed point format
	FILT_T inp_samp  // input sample to multiply
) // Return scaled result of multiply
{
	FILT_T full_res; // Full precision result of multiply
	FILT_T redu_res; // output result of multiply scaled back to sample range


	full_res = ((FILT_T)fix_coef_ps->mant * inp_samp + (FILT_T)fix_coef_ps->err); // Full precison result
	redu_res = (full_res + (FILT_T)fix_coef_ps->half) >> fix_coef_ps->exp; // Compute reduced precision result
	fix_coef_ps->err = (COEF_T)(full_res - (redu_res << fix_coef_ps->exp)); // Update diffusion error

	return redu_res;
} // fix_point_mult
/******************************************************************************/
FILT_T clip_sample( // Clip full precision sample into channel range
	FILT_T inp_full_samp // reduced precision input sample
) // Return Clipped Output Sample

#define MAX_SAMP (((FILT_T)1 << SAMP_BITS) - 1) // Maximum sample value allowed on channel (E.g 0x7fff_ffff)
#define MAX_CLIP 1000 // Max. number of clips allowed before failure

{
	static S32_T pos_clip = 0; // Counts No. of +ve clipping events
	static S32_T neg_clip = 0; // Counts No. of -ve clipping events
	FILT_T out_full_samp; // preset Clipped output sample to input value


	if (inp_full_samp > (FILT_T)MAX_SAMP)
	{ // +ve Clipping
		out_full_samp = (FILT_T)MAX_SAMP; // Clip value to maximum 

		pos_clip++;
		if (pos_clip > MAX_CLIP) assert( 0 == 1 );
	} // if (inp_full_samp > (FILT_T)MAX_SAMP)
	else
	{
		if (inp_full_samp < (FILT_T)(-MAX_SAMP))
		{ // -ve Clipping
			out_full_samp = (FILT_T)(-MAX_SAMP); // Clip value to minimum

			neg_clip++;
			if (neg_clip > MAX_CLIP) assert( 0 == -1 );
		} // if (out_full_samp < (-(FILT_T)MAX_SAMP))
		else
		{ // No Clipping
			out_full_samp = inp_full_samp; // set output sample to input value
		} // else !(out_full_samp < (-(FILT_T)MAX_SAMP))
	} // else !(inp_full_samp > (FILT_T)MAX_SAMP)

	return out_full_samp; // Return clipped value
} // clip_sample 
/******************************************************************************/
SAMP_CHAN_T biquad_filter_chan( // Create filtered output sample for one channel
	SAMP_CHAN_T inp_chan_samp, // Unfiltered input sample at channel precision
	BIQUAD_CHAN_S * bq_chan_ps, // Pointer to structure containing current BiQuad data
	BIQUAD_COEF_S * bq_coef_ps // pointer to structure containing BiQuad coefficients for current channel
) // Return Filtered Output Sample
{
	FILT_T inp_full_samp; // reduced precision input sample
	S32_T inp_redu_samp; // reduced precision input sample
	FILT_T out_full_samp; // full precision filtered output sample
	FILT_T clip_full_samp; // full precision sample, clipped to reduced precision range, returned to channel
	S32_T tap_cnt; // filter-tap counter


	inp_full_samp = (FILT_T)inp_chan_samp + (FILT_T)bq_chan_ps->inp_err; // Add-in Input diffusion error

	inp_redu_samp = (inp_full_samp + (FILT_T)HALF_HEAD) >> HEAD_BITS; // Compute reduced precision input sample
	bq_chan_ps->inp_err = (COEF_T)(inp_full_samp - ((FILT_T)inp_redu_samp << HEAD_BITS)); // Update diffusion error

	// Compute Intermediate IIR filter value
	bq_chan_ps->iir[0] = fix_point_mult( &(bq_coef_ps->a[0]) ,(FILT_T)inp_redu_samp );

	for (tap_cnt=1; tap_cnt<NUM_FILT_TAPS; tap_cnt++)
	{
		bq_chan_ps->iir[0] -= fix_point_mult( &(bq_coef_ps->a[tap_cnt]) ,bq_chan_ps->iir[tap_cnt] );
	} // for tap_cnt

	// Compute BiQuad filtered output value from intermediate IIR values
	bq_chan_ps->filt[0] = fix_point_mult( &(bq_coef_ps->b[0]) ,bq_chan_ps->iir[0] );

	for (tap_cnt=1; tap_cnt<NUM_FILT_TAPS; tap_cnt++)
	{
		bq_chan_ps->filt[0] += fix_point_mult( &(bq_coef_ps->b[tap_cnt]) ,bq_chan_ps->iir[tap_cnt] );
	} // for tap_cnt

	// Update previous stored results
	for (tap_cnt=(NUM_FILT_TAPS - 1); tap_cnt>0; tap_cnt--)
	{
		bq_chan_ps->inp[tap_cnt] =	bq_chan_ps->inp[tap_cnt-1]; 
		bq_chan_ps->iir[tap_cnt] =	bq_chan_ps->iir[tap_cnt-1]; 
		bq_chan_ps->filt[tap_cnt] =	bq_chan_ps->filt[tap_cnt-1]; 
	} // for tap_cnt

	out_full_samp = (bq_chan_ps->filt[0] << HEAD_BITS); // Compute full precision filtered output sample

	clip_full_samp = clip_sample( out_full_samp ); // Clip or full precision sample into channel range.

	return (SAMP_CHAN_T)clip_full_samp;
} // biquad_filter_chan
/******************************************************************************/
S32_T use_biquad_filter( // Use BiQuad filter on one sample from one channel
	S32_T inp_samp, // Unfiltered input sample from channel
	S32_T cur_chan // current channel
) // Return filtered Output Sample
{
	S32_T out_samp; // Output sample


	// Check if biquad parameters have been initialised
	if (0 == biquad_gs.params_set)
	{
		assert(0 == 1); // Please call config_biquad_filter() function before use_biquad_filter() 
	} // if (0 == biquad_gs.params_set)
	else
	{
		// Call biquad on current sample
		out_samp = (S32_T)biquad_filter_chan( (SAMP_CHAN_T)inp_samp ,&(biquad_gs.bq_chan_s[cur_chan]) ,&(biquad_gs.common_coefs_s) );
//		out_samp = inp_samp; // Dbg
	} // else !(0 == biquad_gs.params_set)

	return out_samp;
} // use_biquad_filter
/******************************************************************************/
// biquad_simple.xc
