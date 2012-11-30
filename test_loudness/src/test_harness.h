/*****************************************************************************\
Include File: test_harness.h
  Author(s): Mark Beaumont

Description: Definitions and types for test_harness.c

\*****************************************************************************/

#ifndef _TEST_HARNESS_H_
#define _TEST_HARNESS_H_

#include <stdio.h>
#include <math.h>
// #include "customdefines.h" // Depreciated
#include "types64bit.h"
#include "non_linear_gain.h"

#ifdef __XC__
// XC File

#else //if __XC__
// 'C' File

#endif // else __XC__

#define SAMP_LEN 62 // No of samples to test

#define DEFAULT_DELAY 0 // Default amount of delay in milli-seconds

typedef struct TEST_DATA_TAG // Structure containing test data
{
	SAMP_CHAN_T inps[SAMP_LEN]; // Input Samples
	SAMP_CHAN_T outs[SAMP_LEN]; // Output (Filtered) Samples
} TEST_DATA_S;

/******************************************************************************/
int delay_line_wrapper( // Call filter for one sample
	S32_T inp_samp, // Unfiltered Input Sample
	S32_T cur_chan, // current channel
	S32_T delay_ms, // sample delay in milli-secs
	S32_T samp_freq // current sample frequency
); // Return Filtered Output Sample
/******************************************************************************/

#endif /* ifndef _TEST_HARNESS_H_ */
// test_harness.h
