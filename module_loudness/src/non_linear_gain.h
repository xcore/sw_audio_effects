/******************************************************************************\
 * File:	non_linear_gain.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for module non_linear_gain
 *
 * Version: 3v3rc1
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

#ifndef _NON_LINEAR_GAIN_H_
#define _NON_LINEAR_GAIN_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types64bit.h"
#include "common_utils.h"

#ifndef NUM_GAIN_CHANS 
#define NUM_GAIN_CHANS 2 // XPD tool expect a definition in the header, but please define in Makefile
#define NO_CHANS 1 // Flag no channel definitions
#else // ifndef NUM_GAIN_CHANS 
#define NO_CHANS 0
#endif // NUM_GAIN_CHANS

#define DEF_GAIN 4 // Default Gain.

typedef struct GAIN_PARAM_TAG // Structure containing Gain parameters
{
	S32_T gain; // Maximum desired gain
} GAIN_PARAM_S;

/******************************************************************************/
S32_T use_loudness( // Call non-linear-gain for one sample
	S32_T inp_samp // Input Sample
); // Return Filtered Output Sample
/******************************************************************************/

#ifdef __XC__
// XC File
#include <print.h>

/******************************************************************************/
void config_loudness( // Configure gain parameters. NB Must be called before use_loudness
	GAIN_PARAM_S &cur_param_s // Reference to structure containing gain parameters
);
/******************************************************************************/

#else //if __XC__
// 'C' File

/******************************************************************************/
void config_loudness( // Configure gain parameters. NB Must be called before use_loudness
	GAIN_PARAM_S * cur_param_ps // Pointer to structure containing gain parameters
);
/******************************************************************************/

#define NUM_SECTS 5 // Number of parabolic sections for gain-shaping curve
#define NUM_PNTS (NUM_SECTS + 1) // number of boundary points for gain-shaping curve

#define RANGE_BITS 2 // Determines input sample-size, ( 2:32-bit  1:16-bit)

#define BD3_RATIO 3 // Bit-size Ratio for boundary 1
#define BD4_RATIO 4 // Bit-size Ratio for boundary 2
#define BD7_RATIO 7 // Bit-size Ratio for boundary 4
#define BD8_RATIO 8 // Bit-size Ratio for boundary 5

// Table of sample-size boundaries. Expressed as No. Of Bits
#define BD1_BITS (BD3_RATIO << RANGE_BITS) // e.g. 12
#define BD2_BITS (BD4_RATIO << RANGE_BITS) // e.g. 16
#define BD4_BITS ((BD7_RATIO << RANGE_BITS) - 1) // e.g. 27
#define BD5_BITS ((BD8_RATIO << RANGE_BITS) - 1) // e.g. 31

#define GRAD5_BITS 5 // Gradient magnitude. Expressed as No of Bits represent

/* For 'S' Parabolic sections, there are 3S free parameters.
 * Each Boundary can specify an X co-ord, Y co-ord and Gradient.
 * This is 3(S+1) contraints. Therefore not all constraints can be satisfied.
 * That is why there are gaps in the tables below. These need to be calculated from the other
 * constraints.
 * 	Boundary_0 is always at (0 ,0) and of Gradient 1
 * 	Boundary_1 always has Gradient 1
 * 	Boundary_3 always has Inverse of gradient at Boundary_2 
 * 	Boundary_4 always has Gradient of Boundary_3
 * 	Boundary_5 is always at (MAX_SAMP ,MAX_SAMP) and of Gradient 1
 *
 * That 10 constraints. The remaining 5 can be configured in the tables below
 */

// table of unity-gain boundary points (NB  X=Y )
#define BD0_YX 0
#define BD1_YX ((S64_T)1 << BD1_BITS)
#define BD5_YX ((S64_T)1 << BD5_BITS)

// table of configurable boundary X-Coordinates (NB X<>Y)
#define BD2_X ((S64_T)1 << BD2_BITS)
#define BD4_X ((S64_T)1 << BD4_BITS)

// table of configurable boundary Gradients
#define BD3_G ((S64_T)1 << GRAD5_BITS)


typedef struct BOUND_TAG // Structure containing boundary condition data (between 2 Parabolic sections)
{
	S64_T x; // X co-ordinate
	S64_T y; // Y co-ordinate
	R64_T m; // Gradient
} BOUND_S;

typedef struct PARAB_TAG // Structure containing Parabolic section data (between 2 boundary points)
{
	FIX_POINT_S fix_a; // Coef_A for X^2 in fixed point format
	FIX_POINT_S fix_b; // Coef_B for X in fixed point format
	S64_T x_off; // X Offset (to place origin at boundary point)
	S64_T y_off; // Y Offset (to place origin at boundary point)
	S64_T max_x; // Maximum sample value for this parabolic sectiom
	S32_T big_a; // Flag set if Coef_A is larger than Coef_B (Used for fast compute)
	S32_T scale_e; // Exponent used in final scaling (Used for fast compute)
	S32_T scale_h; // Half scaling factor (Used for rounding)
	S32_T diff_e; // Absolute difference of Coefficient exponent (Used for fast compute)
	S32_T diff_h; // Half of difference factor (used in rounding)
	S32_T origin; // 0/1 selects respectively Lower/Upper boundary point as origin for parabola
	S32_T err; // rounding error Used for error-diffusion
	S32_T dbg; // 0/1 selects respectively Lower/Upper boundary point as origin for parabola
} PARAB_S;

typedef struct GAIN_TAG // Structure containing gain data for all channels
{
	BOUND_S bounds[NUM_PNTS]; // Array of structures containing boundary point data
	PARAB_S parabs[NUM_SECTS]; // Array of structures containing parabolic section data
	S32_T init_done;	// Flag indicating gain-shaping is initialised
	S32_T params_set; // Flag indicating Parameters are set
} GAIN_S;

#endif // else __XC__

#endif /* _NON_LINEAR_GAIN_H_ */

// non_linear_gain.h
