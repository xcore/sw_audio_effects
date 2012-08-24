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

static BIQUAD_S biquad_s; // Structure containing all biquad data (NB Make global to avoid using malloc)

/******************************************************************************/
void scale_coef( // Scale and round floating point coeffiecient
	FIX_POINT_S * fix_coef_p, // pointer to structure containing coef in fixed point format
	REAL_T un_coef // input unscaled floating point coef.
) // return scaled integer coef
{
	S32_T sign_coef = 1; // sign of coef. preset to postive. NB S8_T has bug


	fix_coef_p->err = 0;
	fix_coef_p->exp = 0;

	// Check for special case of zero coef.
	if (0 == un_coef )
	{
		fix_coef_p->mant = 0;
		fix_coef_p->half = 0;
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
			fix_coef_p->exp++; // Increment exponent
			un_coef *= 2; // Double input
		} // while (scale_coef < 0x40000000)

		// Coef should now be in range 0x4000_0000 .. 0x7fff_ffff
		fix_coef_p->mant = sign_coef * (COEF_T)floor( (REAL_T)0.5 + un_coef ); // calculate signed mantissa

		// Check for scaling factor larger than unity
		if (0 < fix_coef_p->exp)
		{ // Evaluate rounding value
			fix_coef_p->half = (1 <<  (fix_coef_p->exp - 1)); // Half scaling factor
		} // if (0 < fix_coef_p->exp)
	} // else !(0 == un_coef )
  
} // scale_coef
/******************************************************************************/
void init_lopass_coefs( // Initialise set of BiQuad coeffs for Low-Pass Filter
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[], // Array of un-normalised IIR filter coefficients
	S32_T samp_freq // current sample frequency
)
{
	// NB Example values given for LPF_NOMINAL_FREQ= 1KHz, samp_freq = 48KHz 
	REAL_T ang_w0 = 2 * PI * LPF_NOMINAL_FREQ / samp_freq; // Angular position 0.1309
	REAL_T sin_w0 = sin( ang_w0 ); // 0.1305
	REAL_T cos_w0 = cos( ang_w0 ); // 0.9914
	REAL_T alpha = sin_w0 / (2 * LPF_QUAL_FACT); // 0.06526


	assert (3 == NUM_TAPS); // Check correct array size for BiQuad

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
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[], // Array of un-normalised IIR filter coefficients
	S32_T samp_freq // current sample frequency
)
{
	// NB Example values given for LPF_NOMINAL_FREQ= 1KHz, samp_freq = 48KHz 
	REAL_T ang_w0 = 2 * PI * LPF_NOMINAL_FREQ / samp_freq; // Angular position 0.1309
	REAL_T sin_w0 = sin( ang_w0 ); // 0.1305
	REAL_T cos_w0 = cos( ang_w0 ); // 0.9914
	REAL_T alpha = sin_w0 / (2 * LPF_QUAL_FACT); // 0.06526


	assert (3 == NUM_TAPS); // Check correct array size for BiQuad

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
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[], // Array of un-normalised IIR filter coefficients
	S32_T samp_freq // current sample frequency
)
{
	// NB Example values given for LPF_NOMINAL_FREQ= 1KHz, samp_freq = 48KHz 
	REAL_T ang_w0 = 2 * PI * LPF_NOMINAL_FREQ / samp_freq; // Angular position 0.1309
	REAL_T sin_w0 = sin( ang_w0 ); // 0.1305
	REAL_T cos_w0 = cos( ang_w0 ); // 0.9914
	REAL_T alpha = sin_w0 / (2 * LPF_QUAL_FACT); // 0.06526


	assert (3 == NUM_TAPS); // Check correct array size for BiQuad

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
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[], // Array of un-normalised IIR filter coefficients
	S32_T samp_freq // current sample frequency
)
{
	// NB Example values given for LPF_NOMINAL_FREQ= 1KHz, samp_freq = 48KHz 
	REAL_T ang_w0 = 2 * PI * LPF_NOMINAL_FREQ / samp_freq; // Angular position 0.1309
	REAL_T sin_w0 = sin( ang_w0 ); // 0.1305
	REAL_T cos_w0 = cos( ang_w0 ); // 0.9914
	REAL_T alpha = sin_w0 / (2 * LPF_QUAL_FACT); // 0.06526


	assert (3 == NUM_TAPS); // Check correct array size for BiQuad

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
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[], // Array of un-normalised IIR filter coefficients
	S32_T samp_freq // current sample frequency
)
{
	// NB Example values given for LPF_NOMINAL_FREQ= 1KHz, samp_freq = 48KHz 
	REAL_T ang_w0 = 2 * PI * LPF_NOMINAL_FREQ / samp_freq; // Angular position 0.1309
	REAL_T sin_w0 = sin( ang_w0 ); // 0.1305
	REAL_T cos_w0 = cos( ang_w0 ); // 0.9914
	REAL_T alpha = sin_w0 / (2 * LPF_QUAL_FACT); // 0.06526


	assert (3 == NUM_TAPS); // Check correct array size for BiQuad

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
	REAL_T un_b[], // Array of un-normalised FIR filter coefficients
	REAL_T un_a[], // Array of un-normalised IIR filter coefficients
	S32_T samp_freq // current sample frequency
)
{
	S32_T tap_cnt; // filter-tap counter


	un_b[0] = 1;
	for (tap_cnt=1; tap_cnt<NUM_TAPS; tap_cnt++)
	{
		un_b[tap_cnt] = 1;
		un_a[tap_cnt] = -1;
	} // for tap_cnt

	// Sum normalisation value
	un_a[0] = un_b[0];
	for (tap_cnt=1; tap_cnt<NUM_TAPS; tap_cnt++)
	{
		un_a[0] += (un_b[tap_cnt] - un_a[tap_cnt]);
	} // for tap_cnt

} // init_custom_coefs
/******************************************************************************/
void init_common_coefs( // Initialise Common BiQuad Coeffs. Edit as to select required Filter
	BIQUAD_COEF_S * bq_coef_sp, // Pointer to structure containing BiQuad coefficients for current channel
	S32_T samp_freq // current sample frequency
)
{
	REAL_T un_b[NUM_TAPS]; // Array of un-normalised FIR filter coefficients
	REAL_T un_a[NUM_TAPS]; // Array of un-normalised IIR filter coefficients
	S32_T tap_cnt; // filter-tap counter


	// Check for sensible sample frequency
	if (samp_freq < MIN_AUDIO_FREQ)
	{
		samp_freq = MIN_AUDIO_FREQ;
	} // if (samp_freq < MIN_AUDIO_FREQ)

	// Clear coef arrays
	for (tap_cnt=0; tap_cnt<NUM_TAPS; tap_cnt++)
	{
		un_b[tap_cnt] = 0;
		un_a[tap_cnt] = 0;
		scale_coef( &(bq_coef_sp->b[tap_cnt]) ,(FILT_T)0.0 );
		scale_coef( &(bq_coef_sp->a[tap_cnt]) ,(FILT_T)0.0 );
	} // for tap_cnt

	// Manually select filter type
//	init_lopass_coefs( un_b ,un_a ,samp_freq );
//	init_hipass_coefs( un_b ,un_a ,samp_freq );
//	init_bandpass_coefs( un_b ,un_a ,samp_freq );
//	init_notch_coefs( un_b ,un_a ,samp_freq );
	init_allpass_coefs( un_b ,un_a ,samp_freq );
//	init_custom_coefs( un_b ,un_a ,samp_freq );

	assert( 0 != un_a[0] ); // Check for divide by zero

	// Normalise and store Coefs
	for (tap_cnt=0; tap_cnt<NUM_TAPS; tap_cnt++)
	{
		scale_coef( &(bq_coef_sp->b[tap_cnt]) ,(REAL_T)( un_b[tap_cnt] / un_a[0] ) );
		scale_coef( &(bq_coef_sp->a[tap_cnt]) ,(REAL_T)( un_a[tap_cnt] / un_a[0] ) );
	} // for tap_cnt
} // init_common_coefs
/******************************************************************************/
void init_biquad_chan( // Initialise BiQuad data for one channel
	BIQUAD_CHAN_S * bq_chan_sp // Pointer to structure containing current BiQuad data
)
{
	S32_T tap_cnt; // filter-tap counter


	for (tap_cnt=0; tap_cnt<NUM_TAPS; tap_cnt++)
	{
		bq_chan_sp->iir[tap_cnt] = 0;
		bq_chan_sp->inp[tap_cnt] = 0;
		bq_chan_sp->filt[tap_cnt] = 0;
	} // for tap_cnt

	bq_chan_sp->iir_err = 0; // Clear IIR Error
	bq_chan_sp->inp_err = 0; // Clear Input Error
	bq_chan_sp->filt_err = 0; // Clear Output Error
} // init_biquad_chan
/******************************************************************************/
void init_biquad( // Create structure for all biquad data, and initialise
	BIQUAD_S * biquad_sp, // Pointer to structure containing all biquad data
	S32_T samp_freq // current sample frequency
) // Return pointer to BiQuad structure
{
	S32_T chan_cnt; // filter-tap counter


	/*	Set-up BiQuad coefficients with default-frequency.
	 * This may be changed dynamically once audio stream starts 
	 *	NB Currently using same coefs for each channel
	 */

  init_common_coefs( &(biquad_sp->common_coefs_s) ,samp_freq );

	// Loop through all output channels
	for (chan_cnt=0; chan_cnt<NUM_USB_CHAN_OUT; chan_cnt++)
	{
		init_biquad_chan( &(biquad_sp->bq_chan_s[chan_cnt]) );
	} // for chan_cnt

} // init_biquad
/******************************************************************************/
FILT_T fix_point_mult( // Multiply a sample by a fixed point coefficient
	FIX_POINT_S * fix_coef_p, // pointer to structure containing coef in fixed point format
	FILT_T inp_samp  // input sample to multiply
) // Return scaled result of multiply
{
	FILT_T full_res; // Full precision result of multiply
	FILT_T redu_res; // output result of multiply scaled back to sample range


	full_res = ((FILT_T)fix_coef_p->mant * inp_samp + (FILT_T)fix_coef_p->err); // Full precison result
	redu_res = (full_res + (FILT_T)fix_coef_p->half) >> fix_coef_p->exp; // Compute reduced precision result
	fix_coef_p->err = (COEF_T)(full_res - (redu_res << fix_coef_p->exp)); // Update diffusion error

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

	return inp_full_samp; // Return unclipped value
} // clip_sample 
/******************************************************************************/
SAMP_CHAN_T biquad_filter_chan( // Create filtered output sample for one channel
	SAMP_CHAN_T inp_chan_samp, // Unfiltered input sample at channel precision
	BIQUAD_CHAN_S * bq_chan_sp, // Pointer to structure containing current BiQuad data
	BIQUAD_COEF_S * bq_coef_sp // pointer to structure containing BiQuad coefficients for current channel
) // Return Filtered Output Sample
{
	FILT_T inp_full_samp; // reduced precision input sample
	S32_T inp_redu_samp; // reduced precision input sample
	FILT_T out_full_samp; // full precision filtered output sample, returned to channel
	S32_T tap_cnt; // filter-tap counter


	inp_full_samp = (FILT_T)inp_chan_samp + (FILT_T)bq_chan_sp->inp_err; // Add-in Input diffusion error

	inp_redu_samp = (inp_full_samp + (FILT_T)HALF_HEAD) >> HEAD_BITS; // Compute reduced precision input sample
	bq_chan_sp->inp_err = (COEF_T)(inp_full_samp - ((FILT_T)inp_redu_samp << HEAD_BITS)); // Update diffusion error

	// Compute Intermediate IIR filter value
	bq_chan_sp->iir[0] = fix_point_mult( &(bq_coef_sp->a[0]) ,(FILT_T)inp_redu_samp );

	for (tap_cnt=1; tap_cnt<NUM_TAPS; tap_cnt++)
	{
		bq_chan_sp->iir[0] -= fix_point_mult( &(bq_coef_sp->a[tap_cnt]) ,bq_chan_sp->iir[tap_cnt] );
	} // for tap_cnt

	// Compute BiQuad filtered output value from intermediate IIR values
	bq_chan_sp->filt[0] = fix_point_mult( &(bq_coef_sp->b[0]) ,bq_chan_sp->iir[0] );

	for (tap_cnt=1; tap_cnt<NUM_TAPS; tap_cnt++)
	{
		bq_chan_sp->filt[0] += fix_point_mult( &(bq_coef_sp->b[tap_cnt]) ,bq_chan_sp->iir[tap_cnt] );
	} // for tap_cnt

	// Update previous stored results
	for (tap_cnt=(NUM_TAPS - 1); tap_cnt>0; tap_cnt--)
	{
		bq_chan_sp->inp[tap_cnt] =	bq_chan_sp->inp[tap_cnt-1]; 
		bq_chan_sp->iir[tap_cnt] =	bq_chan_sp->iir[tap_cnt-1]; 
		bq_chan_sp->filt[tap_cnt] =	bq_chan_sp->filt[tap_cnt-1]; 
	} // for tap_cnt

	out_full_samp = (bq_chan_sp->filt[0] << HEAD_BITS); // Compute full precision filtered output sample

	out_full_samp = clip_sample( out_full_samp ); // Clip or full precision sample into channel range.

	return (SAMP_CHAN_T)out_full_samp;
} // biquad_filter_chan
/******************************************************************************/
S32_T biquad_filter_wrapper( // Wrapper for biquad_filter_wrapper
	S32_T inp_samp, // Unfiltered input sample from channel
	S32_T cur_chan, // current channel
	S32_T samp_freq // current sample frequency
) // Return filtered Output Sample
{
	static S32_T init_flg = 0; // Flag indicating BiQuad is initialised
	static S32_T old_freq = 0; // old sample frequency
	S32_T out_samp = inp_samp; // Preset output sample to input sample (I.E. No Filtering)


	// Check if filter initialised
	if (0 == init_flg)
	{
  	init_biquad( &biquad_s ,samp_freq );	// Initialise Bi-Quad data structure
		init_flg = 1;	// Set initialisation-done flag
	} // if (0 == init_flag)

	// Check Sample frequency is NOT a control code.
	if (MIN_AUDIO_FREQ < samp_freq)
	{
		// Check for sample frequency change
		if (old_freq != samp_freq)
		{
			/* When coefficients are re-calculated mid-stream timing may be broken, but works so far!
  	   * Audio clicks can be heard, but I assume normally this would only occur at a program change,
    	 * so this would mask the click.
			 */

			// recalculate filter coefficiants based on new sample frequency
		  init_common_coefs( &(biquad_s.common_coefs_s) ,samp_freq );
		} // if (samp_freq != old_freq)
		else
		{
			// Call biquad on current sample
			out_samp = (S32_T)biquad_filter_chan( (SAMP_CHAN_T)inp_samp ,&(biquad_s.bq_chan_s[cur_chan]) ,&(biquad_s.common_coefs_s) );
//		out_samp = inp_samp; // Dbg
		} // else !(samp_freq != old_freq)
	} // if (MIN_AUDIO_FREQ < samp_freq)

	old_freq = samp_freq; // Store updated sample frequency
	return out_samp;
} // biquad_filter_wrapper
/******************************************************************************/
// biquad_simple.xc
