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
	S32_T half_len = (SAMP_LEN >> 1); // Half number of samples


	for (samp_cnt = 0; samp_cnt<half_len; samp_cnt++)
	{
		samples[samp_cnt] = (SAMP_CHAN_T)MAX_SAMP;
	} //for samp_cnt

	for (samp_cnt = half_len; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		samples[samp_cnt] = (SAMP_CHAN_T)(-MAX_SAMP);
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
 // Default biquad filter parameters
	BIQUAD_PARAM_S def_param_s = { LO_PASS ,DEF_SAMP_FREQ ,DEF_SIG_FREQ ,DEF_QUAL_FACT };

	S32_T samp_cnt; // Sample counter
	S32_T chan_id = 0; // Channel Identifier


	// Configure gain-shaping parameters ...
	config_biquad_filter( &def_param_s );

	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		tst_data_sp->outs[samp_cnt] = use_biquad_filter( tst_data_sp->inps[samp_cnt] ,chan_id );
	} //for samp_cnt

} // do_test
/*****************************************************************************/
void print_results( // Perform test
	TEST_DATA_S * tst_data_sp  // Pointer to Structure containing test data
)
{
	S32_T samp_cnt; // Sample counter


	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		printf("%010d --> %010d \n" ,tst_data_sp->inps[samp_cnt] ,tst_data_sp->outs[samp_cnt] );
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
