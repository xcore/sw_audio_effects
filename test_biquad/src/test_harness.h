/*****************************************************************************\
Include File: test_harness.h
  Author(s): Mark Beaumont

Description: Definitions and types for test_harness.c

\*****************************************************************************/

#ifndef _TEST_HARNESS_H_
#define _TEST_HARNESS_H_

#include <stdio.h>
#include <math.h>
#include "customdefines.h"
#include "types64bit.h"
#include "biquad_simple.h"

#ifdef __XC__
// XC File

#else //if __XC__
// 'C' File

#define SAMP_LEN 256 // No of samples to test
#define MAX_SAMP 0x7fffffff // Maximum sample value

typedef struct TEST_DATA_TAG // Structure containing test data
{
	SAMP_CHAN_T inps[SAMP_LEN]; // Input Samples
	SAMP_CHAN_T outs[SAMP_LEN]; // Output (Filtered) Samples
} TEST_DATA_S;

#endif // else __XC__

#endif /* ifndef _TEST_HARNESS_H_ */
// test_harness.h
