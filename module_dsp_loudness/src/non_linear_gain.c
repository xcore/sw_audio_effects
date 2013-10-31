/******************************************************************************\
 * File:	non_linear_gain.c
 *
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
 * Currently 5 parabola's are used, as follows:-
 * 1) Unity-gain section (Straight line) for low-level signals (e.g. noise)
 * 2) Transition to maximum gradient (user-configurable).
 * 3) Transition from max. gradient to 'max-gain' section.
 * 4) 'max-gain' section, (Straight line) with minimum gradient (reciprocal of max-gradient)
 * 5) Transition from min. gradient back to 'unity-gain' to finish.
 *
 * These 5 sections are shown below in the rough schematic (as asterisks).
 * For reference: the Unity-gain (Output = Input) line is also shown (as dots)
 * 'M' represents the maximum +ve sample value.
 * NB There is an equivalent curve for -ve sample values.
 *
 * Output
 * ^
 * |
 * M.............................................*
 * |                                            *.
 * 5                                        **5. .
 * +                                ********  .  .
 * 4                         ***4***         .   .
 * |                  *******               .    .
 * +                **                     .     .
 * 3              3*                      .      .
 * |             *                       .       .
 * |            *                       .        .
 * |            *                      .         .
 * |            *                     .          .
 * |           *                     .           .
 * |           *                    .            .
 * |           *                   .             .
 * +           *                  .              .
 * |           *                 .               .
 * |           *                .                .
 * |           *               .                 .
 * |          *               .                  .
 * |          *              .                   .
 * |          *             .                    .
 * |          *            .                     .
 * |          *           .                      .
 * |          *          .                       .
 * |         *          .                        .
 * |         *         .                         .
 * |         *        .                          .
 * |         *       .                           .
 * |         *      .                            .
 * |        *      .                             .
 * 2        2     .                              .
 * |        *    .                               .
 * |        *   .                                .
 * |       *   .                                 .
 * |       *  .                                  .
 * |       * .                                   .
 * |      * .                                    .
 * |      *.                                     .
 * +     *.                                      .
 * |     *                                       .
 * |    *                                        .
 * |   *                                         .
 * 1  1                                          .
 * | *                                           .
 * |*                                            .
 * 0--1--+--2--+--3--+----------4----------+--5--M->Input
 *
\******************************************************************************/

#include "non_linear_gain.h"

// Structure containing all non-linear-gain data (NB Made global to avoid using malloc)
static GAIN_S gain_gs = { .init_done = 0, .params_set = 0 }; // Clear initialisation flags

/******************************************************************************/
void gen_parabola_side_info( // generate side information to allow fast compute for parabola
	PARAB_S * parab_ps // Pointer to structure containing parabola data
) // return scaled integer coef
{
	S32_T exp_diff = (parab_ps->fix_b.exp - parab_ps->fix_a.exp); // Difference of exponents


	// Check which Coef is largest. NB Largest exponent means smallest coef
	if (0 < exp_diff)
	{ // Coef_A is largest
		parab_ps->big_a = 1;
		parab_ps->diff_e = exp_diff;
		parab_ps->scale_e = parab_ps->fix_a.exp; // Scale by smallest exponent
	} // if (0 < exp_diff)
	else
	{ // Coef_B is largest
		parab_ps->big_a = 0;
		parab_ps->diff_e = -exp_diff; // NB Store positive difference
		parab_ps->scale_e = parab_ps->fix_b.exp; // Scale by smallest exponent
	} // else !(0 < exp_diff)

	// Initialise data to control rounding error ...

	parab_ps->err = 0;

	// Check for scaling factor larger than unity
	if (0 < parab_ps->scale_e)
	{ // Evaluate rounding value
		parab_ps->scale_h = (1 <<  (parab_ps->scale_e - 1)); // Half scaling factor
	} // if (0 < parab_ps->exp)

	// Check for difference factor larger than unity
	if (0 < parab_ps->diff_e)
	{ // Evaluate rounding value
		parab_ps->diff_h = (1 <<  (parab_ps->diff_e - 1)); // Half scaling factor
	} // if (0 < parab_ps->exp)
} // gen_side_info
/******************************************************************************/
void init_gain( // Initialise structure for all gain data
	GAIN_S * gain_ps // Pointer to structure containing all gain data
)
{
	// Initialise which end of parabola is origin (0 is low-value end, 1 is high value-end)
	gain_ps->parabs[0].origin = 0;
	gain_ps->parabs[1].origin = 0;
	gain_ps->parabs[2].origin = 0;
	gain_ps->parabs[3].origin = 1;
	gain_ps->parabs[4].origin = 1;

} // init_gain
/*****************************************************************************/
void update_parameters( // Calculate parabolic section data from requested gain parameters
	GAIN_S * gain_ps, // Pointer to structure containing all gain data
	GAIN_PARAM_S * cur_param_ps // Pointer to structure containing gain parameters
)
{
	BOUND_S * bds_p = &(gain_ps->bounds[0]); // Handy pointer to boundary-point data
	PARAB_S * parab_ps; // Handy pointer to structure containing current parabola data
	int sect_cnt; // parabolic Section counter
	int orig_off; // flag indicating whether low or high end of parabola is at origin { 0:Low  1:High }
	R64_T a_coef; // Coef_A for X^2
	R64_T b_coef; // Coef_B for X
	R64_T real_x; // Intermediate X co-ord
	R64_T real_y; // Intermediate Y co-ord


	// Assign Gradients
	bds_p[0].m = (R64_T)1;
	bds_p[1].m = (R64_T)1;
	bds_p[2].m = (R64_T)cur_param_ps->gain; // NB Currently, this is the only configurable parameter
	bds_p[3].m = 1/bds_p[2].m;
	bds_p[4].m = bds_p[3].m;
	bds_p[5].m = (R64_T)1;

	// Assign X Co-ords for Boundaries
	bds_p[0].x = (S64_T)BD0_YX;
	bds_p[1].x = (S64_T)BD1_YX;
	bds_p[2].x = (S64_T)BD2_X;
	bds_p[4].x = (S64_T)BD4_X;
	bds_p[5].x = (S64_T)BD5_YX;

	// Assign Y Co-ords for Boundaries
	bds_p[0].y = bds_p[0].x;
	bds_p[1].y = bds_p[1].x;
	bds_p[5].y = bds_p[5].x;


	// Calculate Y co-ords where X co-ords are available
	bds_p[2].y = (S64_T)floor(( (R64_T)bds_p[1].y + ((R64_T)(bds_p[2].x - bds_p[1].x) * (bds_p[2].m + bds_p[1].m) + (R64_T)1) / (R64_T)2) + (R64_T)0.5);
	bds_p[4].y = (S64_T)floor(( (R64_T)bds_p[5].y - (R64_T)(bds_p[5].x - bds_p[4].x) * (bds_p[5].m + bds_p[4].m) / (R64_T)2) + (R64_T)0.5);

	// Calculate Final X coord
	real_x = (R64_T)(2 * (bds_p[4].y - bds_p[2].y));
	real_x += (R64_T)bds_p[2].x * (bds_p[3].m + bds_p[2].m);
	real_x -= (R64_T)bds_p[4].x * (bds_p[4].m + bds_p[3].m);
	real_x /= (bds_p[2].m - bds_p[4].m);
	bds_p[3].x = (S64_T)floor(real_x + (R64_T)0.5);

	// Calculate Final Y coord ...
	real_y = (R64_T)bds_p[2].y + (real_x - (R64_T)bds_p[2].x) * (bds_p[3].m + bds_p[2].m) / (R64_T)2;
	bds_p[3].y = (S64_T)floor( real_y + (R64_T)0.5 );

	// Fill out rest of Parabola data
	for (sect_cnt=0; sect_cnt<NUM_SECTS; sect_cnt++)
	{
		parab_ps = &(gain_ps->parabs[sect_cnt]); // Point to current parabola data
		orig_off = parab_ps->origin; // Determine which end of parabola is origin
		parab_ps->x_off = bds_p[sect_cnt + orig_off].x; // Get X coord of origin
		parab_ps->y_off = bds_p[sect_cnt + orig_off].y; // Get Y coord of origin
		parab_ps->max_x = bds_p[sect_cnt + 1].x; // Store upper boundary limit
		b_coef = bds_p[sect_cnt + orig_off].m; // Assign origin gradient to B coef

		// Compute A coef
		a_coef = (bds_p[sect_cnt+1].m - bds_p[sect_cnt].m) / (R64_T)(bds_p[sect_cnt+1].x - bds_p[sect_cnt].x) / (R64_T)2;

		// Convert coefficients to fixed-point format
		scale_coef( &(parab_ps->fix_a) ,a_coef );
		scale_coef( &(parab_ps->fix_b) ,b_coef );

		// Generate side information for fast compute
		gen_parabola_side_info( parab_ps );
	} // for sect_cnt

} // update_parameters
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
S64_T draw_parabola( // Computes Y co-ord for current parabola, given X co-ord
	GAIN_S * gain_ps, // Pointer to structure containing gain data
	PARAB_S * parab_ps, // Pointer to structure containing current parabola data
	S64_T inp_x  // input X co-ordinate
) // Return Y co-ordinate
{
	S64_T tmp_val; // Intermediate value
	S64_T out_y; // Output Y co-ordinate


	// Determine compute mode
	if (parab_ps->big_a)
	{
		tmp_val = inp_x * (S64_T)parab_ps->fix_a.mant;

		// Check fo non-zero Coef_B
		if (0 != parab_ps->fix_b.mant)
		{
			tmp_val += ((S64_T)parab_ps->fix_b.mant + parab_ps->diff_h) >>  parab_ps->diff_e;
		} // if (0 != parab_ps->fix_b.mant)
	} // if (parab_ps->big_a)
	else
	{
		tmp_val = (S64_T)parab_ps->fix_b.mant;

		// Check fo non-zero Coef_A
		if (0 != parab_ps->fix_a.mant)
		{
			tmp_val += (((S64_T)parab_ps->fix_a.mant * inp_x) + parab_ps->diff_h) >>  parab_ps->diff_e;
		} // if (0 != parab_ps->fix_a.mant)
	} // else !(parab_ps->big_a)

	tmp_val = (tmp_val * inp_x) + parab_ps->err; // value before down-scaling, add in previous diffusion error
	out_y = (tmp_val + (S64_T)parab_ps->scale_h ) >> parab_ps->scale_e; // Output (down-scaled) Y co-ord
	parab_ps->err = tmp_val - (out_y << parab_ps->scale_e); // compute new diffusion error

	return out_y;
} // draw_parabola
/******************************************************************************/
SAMP_CHAN_T boost_gain( // Applies non-linear gain to input sample to generate output sample
	GAIN_S * gain_ps, // Pointer to structure containing all gain data
	SAMP_CHAN_T inp_samp // input sample at channel precision
) // Return Amplified Output Sample
{
	PARAB_S * parab_ps; // Pointer to structure containing current parabola data
	SAMP_CHAN_T out_abs; // Absolute value of Output sample
	S32_T sect_cnt; // Parabolic section counter
	S32_T x_coord; // X co-ordinate if parabola
	S32_T y_coord; // Y co-ordinate if parabola
	S32_T sgn_samp = 1; // Preset polarity of sample to positive. NB S8_T is broken in 11.11.0


	// Force positive sample value
	if (0 > inp_samp)
	{
		inp_samp = -inp_samp; // absolute value
		sgn_samp = -1; // store negative polarity
	} // if (0 > inp_samp)

	/* Find correct parabolic section for input sample ...
	 * NB currently this is a simple algorithm, but could be made quicker (but more complex)
	 */

	// Loop through parabolic sections until correct match
	for (sect_cnt=0; sect_cnt<NUM_SECTS; sect_cnt++)
	{
		parab_ps = &(gain_ps->parabs[sect_cnt]); // Point to data for current parabolic section

		// Skip if input-sample is greater than max allowed value for current parabola
		if (inp_samp <= parab_ps->max_x)
		{
			x_coord = (S64_T)inp_samp - (S64_T)parab_ps->x_off;

			y_coord = draw_parabola( gain_ps ,parab_ps ,x_coord ); // Compute Y co-ord of parabola given  X co-ord
			out_abs = y_coord + parab_ps->y_off;

			break; // Stop searching
		} // if (inp_samp < parab_ps->max_x)
	} // for sect_cnt

	return (SAMP_CHAN_T)(sgn_samp * out_abs); // Re-create signed value
} // boost_gain
/******************************************************************************/
S32_T use_loudness( // Wrapper for non_linear_gain_wrapper
	S32_T inp_samp // input sample from channel
) // Return Amplified Output Sample
{
	S32_T out_samp; // amplified output sample at channel precision


	// Check if loudness parameters have been initialised
	if (0 == gain_gs.params_set)
	{
		assert(0 == 1); // Please call config_loudness() function before use_loudness()

		return out_samp;
	} // if (0 == init_flag)
	else
	{ // process input sample
		out_samp = boost_gain( &gain_gs ,inp_samp ); // Apply non-linear gain
//	out_samp = inp_samp; // Dbg
	} // else !(0 == init_flag)

	return out_samp;
} // use_loudness
/******************************************************************************/
// non_linear_gain.xc
