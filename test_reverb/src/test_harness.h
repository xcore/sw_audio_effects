/*****************************************************************************\
Include File: test_harness.h
  Author(s): Mark Beaumont

Description: Definitions and types for test_harness.c

\*****************************************************************************/

#ifndef _TEST_HARNESS_H_
#define _TEST_HARNESS_H_

#include <stdio.h>
#include <math.h>
#include "types64bit.h"
#include "reverb.h"

#ifdef __XC__
// XC File

// #include "dsp.h"
// #include <xs1.h>
// #include <safestring.h>
// #include <print.h>
// #include "devicedefines.h"
// #include "coeffs.h"

#else //if __XC__
// 'C' File

#endif // else __XC__

#define SAMP_LEN 2048 // No of samples to test

#define DEFAULT_FREQ 48000

// Channel definitions
#define LEFT 0
#define RITE 1

typedef	SAMP_CHAN_T CHAN_SET_A[NUM_REVERB_CHANS]; // Type for Array of Samples for each channel

typedef struct TEST_DATA_TAG // Structure containing test data
{
	CHAN_SET_A inps[SAMP_LEN]; // Input Samples (for all channels)
	CHAN_SET_A outs[SAMP_LEN]; // Output Samples with Reverb (for all channels)
} TEST_DATA_S;

#endif /* ifndef _TEST_HARNESS_H_ */
// test_harness.h
