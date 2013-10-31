/*****************************************************************************\
Program File: test_harness.c
  Author(s): Mark Beaumont

Description: Test Harness for BiQuad

\*****************************************************************************/

#include "test_harness.h"

/******************************************************************************/
void print_X64( // Print 64-bit value as hex code
	S64_T inp_val // 64-bit input data
)
{
	printf("0x%08x" ,(S32_T)(inp_val >> 32) );
	printf("%08x \n" ,(S32_T)(inp_val & 0xffffffff) );
} // print_X64
/*****************************************************************************/
void init_samples( // Initialise input data samples
	TEST_DATA_S * tst_data_sp, // Pointer to Structure containing test data
	SAMP_CHAN_T samples[] // Array of input samples
)
{
	S32_T samp_cnt; // Sample counter
	S32_T half_len = (SAMP_LEN >> 1); // Half Sample length
	S64_T bit = 2; // Bit Value
	S32_T val = 2; // Sample Value


	for (samp_cnt = 0; samp_cnt<half_len; samp_cnt++)
	{
		val = (SAMP_CHAN_T)(bit - 1);
		samples[half_len + samp_cnt] = val;
		samples[(half_len - 1) - samp_cnt] = -val;
		bit <<= 1;
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
	GAIN_PARAM_S def_param_s; // Structure containing default gain parameters
	S32_T samp_cnt; // Sample counter
	S32_T chan_id = 0; // Channel Identifier


	// Configure gain-shaping parameters ...

//MB~	def_param_s.gain = DEF_GAIN;
	def_param_s.gain = 32;

	config_loudness( &def_param_s );

	// Loop through test samples
	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		tst_data_sp->outs[samp_cnt] = use_loudness( tst_data_sp->inps[samp_cnt] );
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
		printf("%+012d --> %+012d \n" ,tst_data_sp->inps[samp_cnt] ,tst_data_sp->outs[samp_cnt] );
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
