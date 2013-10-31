/*****************************************************************************\
Program File: test_harness.c
  Author(s): Mark Beaumont

Description: Test Harness for BiQuad

\*****************************************************************************/

#include "test_harness.h"

/*****************************************************************************/
void init_samples( // Initialise input data samples
	TEST_DATA_S * tst_data_sp, // Pointer to Structure containing test data
	CHAN_SET_A all_samps[] // Array of structures containing all Input samples
 // Array of input samples
)
{
	S32_T chan_cnt; // Channel counter
	S32_T samp_cnt; // Sample counter


	// Clear all input samples
	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		for (chan_cnt = 0; chan_cnt<NUM_REVERB_CHANS; chan_cnt++)
		{
			all_samps[samp_cnt][chan_cnt] = 0;
		} //for chan_cnt
	} //for samp_cnt

	// Add impulse on left channel
	all_samps[1][LEFT] = MAX_SAMP;
} // init_samples
/*****************************************************************************/
void init_test( // Initialise test data
	TEST_DATA_S * tst_data_sp // Pointer to Structure containing test data
)
{
	init_samples( tst_data_sp ,tst_data_sp->inps );
} // init_test
/*****************************************************************************/
void do_test( // Perform test
	TEST_DATA_S * tst_data_sp // Pointer to Structure containing test data
)
{
 // Default Reverb parameters
	REVERB_PARAM_S def_param_s = { DEF_ROOM_SIZE ,DEF_SIG_FREQ ,DEF_SAMP_FREQ ,DEF_GAIN };

	S32_T samp_cnt; // Sample counter
	S32_T chan_cnt; // Channel counter
	SAMP_CHAN_T uneq_buf[NUM_REVERB_CHANS]; // Buffer for unequalised samples
	SAMP_CHAN_T equal_buf[NUM_REVERB_CHANS]; // Buffer for equalised samples
	SAMP_CHAN_T unamp_buf[NUM_REVERB_CHANS]; // Buffer for unamplified samples
	SAMP_CHAN_T ampli_buf[NUM_REVERB_CHANS]; // Buffer for amplified samples


	for (chan_cnt = 0; chan_cnt<NUM_REVERB_CHANS; chan_cnt++)
	{
		uneq_buf[chan_cnt] = 0;
		equal_buf[chan_cnt] = 0;
		unamp_buf[chan_cnt] = 0;
		ampli_buf[chan_cnt] = 0;
	} //for chan_cnt

	// Configure gain-shaping parameters ...
	config_reverb( &def_param_s );

	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		use_reverb( uneq_buf ,unamp_buf ,tst_data_sp->outs[samp_cnt] ,tst_data_sp->inps[samp_cnt] ,equal_buf ,ampli_buf );

		// No EQ or Gain : So copy input to output
		for (chan_cnt = 0; chan_cnt<NUM_REVERB_CHANS; chan_cnt++)
		{
			equal_buf[chan_cnt] = uneq_buf[chan_cnt];
			ampli_buf[chan_cnt] = unamp_buf[chan_cnt];
		} //for chan_cnt
	} //for samp_cnt

} // do_test
/*****************************************************************************/
void print_results( // Perform test
	TEST_DATA_S * tst_data_sp  // Pointer to Structure containing test data
)
{
	S32_T samp_cnt; // Sample counter


	// Loop through all input samples
	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		printf("%6d: %010d %010d --> " ,samp_cnt ,tst_data_sp->inps[samp_cnt][LEFT] ,tst_data_sp->inps[samp_cnt][RITE] );
		printf("%010d %010d \n" ,tst_data_sp->outs[samp_cnt][LEFT] ,tst_data_sp->outs[samp_cnt][RITE] );
	} //for samp_cnt

} // print_results
/*****************************************************************************/
void test_harness()
{
	TEST_DATA_S tst_data_s; // Structure containing test data


	init_test( &tst_data_s );

	do_test( &tst_data_s );

	print_results( &tst_data_s );
} // test_harness
/*****************************************************************************/
// test_harness.c
