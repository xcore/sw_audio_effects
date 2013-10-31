/*****************************************************************************\
Program File: test_harness.c
  Author(s): Mark Beaumont

Description: Test Harness for BiQuad

\*****************************************************************************/

#include "test_harness.h"

/*****************************************************************************/
void init_samples( // Initialise input data samples
	TEST_DATA_S * tst_data_sp, // Pointer to Structure containing test data
	SAMP_CHAN_T samples[] // Array of input samples
)
{
	S32_T samp_cnt; // Sample counter


	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		samples[samp_cnt] = (SAMP_CHAN_T)(10 + samp_cnt);
	} //for samp_cnt

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
	DELAY_PARAM_S def_param_s; // Structure containing delay-line parameters
	S32_T delay_us = 4000; // Smallest delay in micro-secs
	S32_T tap_cnt; // tap counter
	S32_T samp_cnt; // Sample counter
	S32_T chan_id = 0; // Channel Identifier


	def_param_s.num = DEF_TAPS;
	def_param_s.freq = DEF_SAMP_FREQ;

	// Initialise Configuration parameter structure
	for (tap_cnt = 0; tap_cnt<DEF_TAPS; tap_cnt++)
	{
		def_param_s.us_delays[tap_cnt] = delay_us;
		delay_us <<= 1; // Double delay

	} //for tap_cnt

	config_delay_line( &def_param_s );

	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		use_delay_line( tst_data_sp->outs[samp_cnt] ,tst_data_sp->inps[samp_cnt] ,chan_id );
	} //for samp_cnt


} // do_test
/*****************************************************************************/
void print_results( // Perform test
	TEST_DATA_S * tst_data_sp  // Pointer to Structure containing test data
)
{
	S32_T samp_cnt; // Sample counter
	S32_T tap_cnt; // Sample counter


	// Loop through all input samples
	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		printf("%010d " ,tst_data_sp->inps[samp_cnt] );

		// Loop through all delay taps.
		for (tap_cnt = 0; tap_cnt < DEF_TAPS; tap_cnt++)
		{
			printf("--> %010d " ,tst_data_sp->outs[samp_cnt][tap_cnt] );
		} // for tap_cnt
		printf("\n");
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
