/*****************************************************************************\
Program File: test_harness.c
  Author(s): Mark Beaumont

Description: Test Harness for BiQuad

\*****************************************************************************/

#include "test_harness.h"

/******************************************************************************/
void init_dsp( // Initialisation for DSP control of SDRAM-delay
	CNTRL_S * cntrl_ps, // Pointer to structure containing data to control SDRAM buffering
	DELAY_PARAM_S * delay_param_ps // Reference to structure containing SDRAM-Delay configuration data
)
{
//MB~	S32_T delay_us = (83 * TWIN_SAMPS / 4); // Smallest delay in micro-secs
	S32_T delay_us = (83 * TWIN_SAMPS * 2); // Maximu delay in micro-secs
	S32_T tap_cnt; // tap counter
	S32_T chan_cnt; // Channel counter
	

	// initialise samples buffers
	for (chan_cnt = 0; chan_cnt < NUM_DELAY_CHANS; chan_cnt++)
	{
		cntrl_ps->inp_set.samps[chan_cnt] = 0;

		for (tap_cnt=0; tap_cnt<DEF_TAPS; tap_cnt++)
		{
			cntrl_ps->delay_sets[tap_cnt].samps[chan_cnt] = 0;
		} //for tap_cnt
	}

	// Initialise SDRAM control structure ...

	cntrl_ps->write.do_buf = 0;
	for (tap_cnt=0; tap_cnt<DEF_TAPS; tap_cnt++)
	{
		cntrl_ps->reads[tap_cnt].do_buf = 0;
	} //for tap_cnt

	// Initialsie Delay-line configuration parameters ...
	delay_param_ps->num = DEF_TAPS;
	delay_param_ps->freq = DEF_SAMP_FREQ;
//	delay_us = DELAY_SAMPS * (1000000 / delay_param_ps->freq); // Approx largest delay in micro-secs

	// Initialise Configuration parameter structure
	for (tap_cnt = (DEF_TAPS - 1); tap_cnt>=0; tap_cnt--)
	{
		delay_param_ps->us_delays[tap_cnt] = delay_us;
		delay_us >>= 1; // Double delay
	} //for tap_cnt
} // init_dsp
/*****************************************************************************/
void init_samples( // Initialise input data samples 
	TEST_DATA_S * tst_data_ps, // Pointer to Structure containing test data
	CHAN_SET_S samples[] // Array of input samples
) 
{
	S32_T samp_cnt; // Sample counter
	S32_T chan_cnt; // Channel counter


	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		samples[samp_cnt].samps[0] = (SAMP_CHAN_T)(10 * samp_cnt + 1);

		for (chan_cnt = 1; chan_cnt<NUM_DELAY_CHANS; chan_cnt++)
		{
			samples[samp_cnt].samps[chan_cnt] = samples[samp_cnt].samps[chan_cnt-1] + 1;
		} //for chan_cnt
	} //for samp_cnt

} // init_samples
/*****************************************************************************/
void init_test( // Initialise test data
	TEST_DATA_S * tst_data_ps // Pointer to Structure containing test data
) 
{
	tst_data_ps->dbg = 0;
	init_samples( tst_data_ps ,tst_data_ps->inp_set );
} // init_test
/*****************************************************************************/
void mm_read_words(
	TEST_DATA_S * tst_data_ps, // Pointer to Structure containing test data
	unsigned mem_addr, 
	unsigned words, 
	unsigned buf_addr
)
{
	memcpy( (void *)buf_addr ,(const void *)mem_addr ,(size_t)(words << 2) );
} // mm_read_words
/*****************************************************************************/
void mm_write_words(
	TEST_DATA_S * tst_data_ps, // Pointer to Structure containing test data
	unsigned mem_addr, 
	unsigned words, 
	unsigned buf_addr
)
{
	memcpy( (void *)mem_addr ,(const void *)buf_addr ,(size_t)(words << 2) );
} // mm_write_words
/*****************************************************************************/
void do_test( // Perform test
	TEST_DATA_S * tst_data_ps // Pointer to Structure containing test data
) 
{
	CNTRL_S * cntrl_ps = &(tst_data_ps->cntrl_dsp_s); // Pointer to Structure containing data to control SDRAM buffering
	DELAY_PARAM_S def_param_s; // Structure containing delay-line parameters
	BUF_S * buf_p; // Pointer to Structure containing buffer data
	S32_T tap_cnt; // tap counter
	S32_T samp_cnt; // Sample counter


	init_dsp( cntrl_ps ,&def_param_s ); // Initialise DSP control data

	def_param_s.dbg = tst_data_ps->dbg;
	def_param_s.dbg = 0;

	config_sdram_delay( &def_param_s );

	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{

		cntrl_ps->inp_set = tst_data_ps->inp_set[samp_cnt];

		use_sdram_delay( cntrl_ps );

if (def_param_s.dbg) printf("DT: %12ld\n" ,((CHAN_SET_S *)cntrl_ps->write.buf_adr)->samps[0]); //MB~

		// Check if write-buffer is full
		if (cntrl_ps->write.do_buf)
		{
			mm_write_words_p( tst_data_ps ,cntrl_ps->write.mem_adr ,cntrl_ps->write.wrd_siz ,cntrl_ps->write.buf_adr );

			cntrl_ps->write.do_buf = 0; // Clear write flag
		} // if (cntrl_ps->do_write)

		// Check if any read-buffers are empty
		for (tap_cnt=0; tap_cnt<DEF_TAPS; tap_cnt++)
		{
			buf_p = &(cntrl_ps->reads[tap_cnt]); // Point to current read buffer

			if (buf_p->do_buf)
			{
				mm_read_words_p( tst_data_ps ,buf_p->mem_adr ,buf_p->wrd_siz ,buf_p->buf_adr );

				buf_p->do_buf = 0; // Clear read flag
			} // if (cntrl_ps->do_read)
		} //for tap_cnt

		for (tap_cnt = 0; tap_cnt<DEF_TAPS; tap_cnt++)
		{
			tst_data_ps->out_sets[samp_cnt][tap_cnt] = cntrl_ps->delay_sets[tap_cnt];
		} //for tap_cnt
	} //for samp_cnt

} // do_test
/*****************************************************************************/
void print_results( // Perform test
	TEST_DATA_S * tst_data_ps  // Pointer to Structure containing test data
) 
{
	S32_T samp_cnt; // Sample counter
	S32_T tap_cnt; // Delay-tap counter
	S32_T chan_cnt; // Channel counter


	// Loop through all input samples
	for (samp_cnt = 0; samp_cnt<SAMP_LEN; samp_cnt++)
	{
		// Loop through all channels
//MB~		for (chan_cnt = 0; chan_cnt < NUM_DELAY_CHANS; chan_cnt++)
		for (chan_cnt = 0; chan_cnt < 1; chan_cnt++)
		{
			printf("%010d->" ,tst_data_ps->inp_set[samp_cnt].samps[chan_cnt]  );

			// Loop through all taps
			for (tap_cnt = 0; tap_cnt < DEF_TAPS; tap_cnt++)
			{
				printf("%010d " ,tst_data_ps->out_sets[samp_cnt][tap_cnt].samps[chan_cnt] );
			} // for tap_cnt
			printf("  " );
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
