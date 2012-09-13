/******************************************************************************\
 * File:	non_linear_gain.c
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
 * DESCRIPTION
 * -----------
 * This algorithm uses a piece-wise parabolic curve, to limit the number of multiplies required per sample.
 * Currently 2 parabola's are used, one for low-level signals (e.g. noise) and one for high-levele signals.
 *
 * The 'Lo' parabola must NOT significantly increase the gain of low-level signals. 
 * Therefore when Input=0, the parabola approximates to: Output = Input (or L = X)
 * This forces the Lo parabola to be of the form L = X(A.X + 1), 
 * Where A will be determined by boundary conditions.
 *
 * The 'Hi' parabola must not allow signal over-shoot, and acts like a 'limiter'.
 * Therefore when Input=Max, the parabola approximates to: Output = Max (or H = M)
 * This forces the Hi parabola to be of the form H = M - G(M - X)^2
 * Where G will be determined by boundary conditions.
 * 
 * At the boundary point (input = B). The Lo and Hi parabolas must meet with equal gradient.
 * Therefore for the Gradient:	2G(M - B) = 2AB + 1		--(1)
 * and for the Output:	B(AB + 1) = M - G(M - B)^2		--(2)
 *
 * Solving (1) and (2) yields the following formulea:
 *
 * A = (M - B)/(2BM)               G = (2M - B)/[2B(M - B)]
 *
 * M = 2^32 = 2^s, by forcing B to be a power of 2 (2^n) the parabolas can be evaluated with
 * just multiply, addition, and shift operators: as follows
 *
 * L = X{[ 2^(s+1) + X{ 2^(s-n) - 1} + 2^s] >> (s+1)}
 * H = 2^s - {[ Z{[gZ + 2^29] >> 30} + 2^s] >> (s+1)}				where Z = (M - X), and
 *
 * g = 2^30.[2^(h+1) - 1]/[2^h - 1]				where h = (s - n) = (number of hi-bits)
 *
 * g is (2 + fraction), and is represented in Fixed point as 2:30, so the MS-bit is 1.
 *
 * The fractional bits are mainly zero, and every 'h'th fractional bit is non-zero.
 *  E.g. for h=12,   g = 0x8004_0040
 *
\******************************************************************************/

#include "non_linear_gain.h"

// Structure containing all non-linear-gain data (NB Made global to avoid using malloc)
static GAIN_S gain_gs = { .init_done = 0, .params_set = 0 }; // Clear initialisation flags

/******************************************************************************/
void update_parameters( // Initialise parameters to generate required gain value (Approximately See below)
	GAIN_S * gain_ps, // Pointer to structure containing all gain data
	GAIN_PARAM_S * cur_param_ps // Pointer to structure containing gain parameters
)
// NB Current algorithm only supports gains that are a power-of-2, so requested gain is rounded to nearest power-of-2

{
	S32_T cur_gain = cur_param_ps->gain; // Get requested gain value
	S32_T cur_iters = 1; // Initialise to minimum number of iterations

	// Loop through LS bits of current gain	
	while (3 < cur_gain)
	{
		cur_iters++; // Increment No. of iterations
		cur_gain >>= 1; // divide gain by 2
	} // while (3 < cur_gain)

	// Check 2 most significant bits for rounding
	if (3 == cur_gain)
	{
		cur_iters++; // Round-up No. of iterations
	} // if (3 == cur_gain)

	assert( MAX_ITERS >= cur_iters ); // If fails, need more diffusion error arrays

	gain_gs.num_iters = cur_iters; // Store new iteration number
} // update_parameters
/******************************************************************************/
void init_chan( // Initialise structure for one channel of gain data
	GAIN_CHAN_S * gain_chan_ps // Pointer to structure containing gain data for one channel
)
{
	S32_T iter_cnt; // iteration counter


	// Loop through all iteration slots
	for (iter_cnt=0; iter_cnt<MAX_ITERS; iter_cnt++)
	{
		gain_chan_ps->err_s[iter_cnt] = 0; // Clear diffusion error for sample
		gain_chan_ps->err_g[iter_cnt] = 0; // Clear diffusion error for gain
	} // for iter_cnt
} // init_chan
/******************************************************************************/
void init_gain( // Initialise structure for all gain data
	GAIN_S * gain_ps // Pointer to structure containing all gain data
)
{
	S32_T chan_cnt; // channel counter
	U32_T coef_val; // Coefficient value in FIXED POINT format. E.G. 2:30
	S32_T bit_shift; // Preset bit_shift
	S32_T hi_bits = HI_BITS; // No of bits above low-level threshold


	assert (0 < hi_bits); // Check we have some hi-bits!

	coef_val = ((U32_T)2 << FRAC_BITS); // Preset coef_g to 2.0 in 2:30 fixed point format)
	bit_shift = FRAC_BITS - hi_bits; // Preset bit_shift to position of 1st fractional bit

	// Add in fractional coefficient bits. E.g. for hi_bits=12 coef_val = 0x8004_0040 ...

	// Loop through fractional bits		
	while (0 <= bit_shift)
	{
		coef_val += ((U32_T)1 << bit_shift);
		bit_shift -= hi_bits; // Update bit_shift to position of next fractional bit
	} // while (hi_bits < bit_shift)

	// Check for rounding
	if (-1 == bit_shift) coef_val++;

	gain_ps->coef_g = coef_val; // Initialise coefficient

	// Loop through all output channels
	for (chan_cnt=0; chan_cnt<NUM_GAIN_CHANS; chan_cnt++)
	{
		init_chan( &(gain_ps->chan_s[chan_cnt]) );
	} // for chan_cnt
} // init_gain
/******************************************************************************/
void config_loudness( // Configure loudness parameters
	GAIN_PARAM_S * cur_param_ps // Pointer to structure containing gain parameters
)
{
	// Check if Gain-shaping initialised
	if (0 == gain_gs.init_done)
	{ // Initialse Gain shaping
  	init_gain( &gain_gs );	// Initialise gain data structure

		gain_gs.init_done = 1; // Signal Gain-shaping initialised
	} // if (0 == gain_gs->init_done)

//printstr("GN= "); printintln( (int)cur_param_ps->gain ); // MB~

	// Calculate gain-shaping parameters
	update_parameters( &gain_gs ,cur_param_ps );

	gain_gs.params_set = 1; // Signal Gain-shaping parameters configured

} // config_loudness 
/******************************************************************************/
SAMP_CHAN_T boost_gain( // Applies non-linear gain to input sample to generate output sample
	GAIN_S * gain_ps, // Pointer to structure containing all gain data
	GAIN_CHAN_S * gain_chan_ps, // Pointer to structure containing gain data for current channel
	SAMP_CHAN_T inp_samp, // input sample at channel precision
	S32_T cur_iter // current value of iteration counter
) // Return Amplified Output Sample
{
	S32_T sgn_samp = 1; // Preset polarity of sample to positive. NB S8_T is broken in 11.11.0
	S32_T unity = ((S32_T)1 << (MAGN_BITS + 1)); // Scaled-up unity value
	S64_T full_val; // intermediate value at full precision
	S64_T redu_val; // intermediate value at reduced precision
	S32_T inp_room; // Headroom between input value and max value


	// Force positive sample value
	if (0 > inp_samp)
	{
		inp_samp = -inp_samp; // absolute value
		sgn_samp = -1; // store negative polarity 
	} // if (0 > inp_samp)

	// Check if signal above low-level threshold
	if (THRESHOLD < inp_samp)
	{ // Signal above low-level threshold
		inp_room = MAX_MAGN - inp_samp; // Calculate input head-room
		full_val = (U64_T)gain_ps->coef_g * (U64_T)inp_room; // Calculate gain

		full_val += gain_chan_ps->err_g[cur_iter]; // Add gain diffusion error for this iteration
		redu_val = ((full_val + HALF_FRAC) >> FRAC_BITS); // Down-scale gain
		gain_chan_ps->err_g[cur_iter] = full_val - (redu_val << FRAC_BITS); // Update gain diffusion error

		full_val = redu_val * (S64_T)inp_room; // Multiply head_room by gain

		full_val += gain_chan_ps->err_s[cur_iter]; // Add sample diffusion error for this iteration
		redu_val = (full_val + MAX_MAGN) >> (MAGN_BITS + 1); // Down-scale to output precision (output headroom)
		gain_chan_ps->err_s[cur_iter] = full_val - (redu_val << (MAGN_BITS + 1)); // Update sample diffusion error

		redu_val = MAX_MAGN - redu_val; // NB Calculates new output from output head-room
	} // if (THRESHOLD <= inp_samp)
	else
	{ // Signal below low-level threshold
		full_val = ((S64_T)MAX_HI * (S64_T)inp_samp) + unity; // Calculate gain
		full_val = full_val * (S64_T)inp_samp; // Multiply input by gain

		full_val += gain_chan_ps->err_s[cur_iter]; // Add sample diffusion error for this iteration
		redu_val = (full_val + MAX_MAGN) >> (MAGN_BITS + 1); // Down-scale to output precision (output headroom)
		gain_chan_ps->err_s[cur_iter] = full_val - (redu_val << (MAGN_BITS + 1)); // Update sample diffusion error
	} // else !(THRESHOLD <= inp_samp )

//	redu_val = inp_samp; // Dbg
	return (SAMP_CHAN_T)(sgn_samp * redu_val); // Re-create signed value
} // boost_gain
/******************************************************************************/
S32_T use_loudness( // Wrapper for non_linear_gain_wrapper
	S32_T inp_chan_samp, // input sample from channel
	S32_T cur_chan // current channel
) // Return Amplified Output Sample
{
	static S32_T err = 0; // Diffusion Error
	S64_T full_samp; // full precision sample
	S32_T redu_samp; // reduced precision sample
	S32_T out_samp; // amplified output sample at channel precision
	S32_T iter_cnt; // Iteration counter


	// Check if loudness parameters have been initialised
	if (0 == gain_gs.params_set)
	{
		assert(0 == 1); // Please call config_loudness() function before use_loudness() 
	} // if (0 == init_flag)
	else
	{ // process input sample
		// Scale-down 32-bit sample to 24-bit
		full_samp = (S64_T)inp_chan_samp + (S64_T)err; // Add diffusion error
		redu_samp = (S32_T)((full_samp + HALF_SCALE) >> SCALE_BITS);
		err = full_samp - ((S64_T)redu_samp << SCALE_BITS);
	
		// Loop through iterations
		for (iter_cnt = 0; iter_cnt < gain_gs.num_iters; iter_cnt++)
		{
			out_samp = boost_gain( &gain_gs ,&(gain_gs.chan_s[cur_chan]) ,redu_samp ,iter_cnt ); // Apply non-linear gain
	
			redu_samp = out_samp; // Update amplifier input ready for next iteration
		} // for (iter_cnt = 0; iter_cnt < gain_gs.num_iters; iter_cnt++)
	
		// Scale-up 24-bit sample to 32-bit
		out_samp = redu_samp << SCALE_BITS;
//	out_samp = inp_chan_samp; // Dbg
	} // else !(0 == init_flag)

	return out_samp;
} // use_loudness 
/******************************************************************************/
// non_linear_gain.xc
