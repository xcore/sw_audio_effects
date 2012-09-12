#include <xclib.h>
#include <xs1.h>
#include <print.h>

#include "global.h"
#include "codec.h"

extern buffered out port:32 p_lrclk;
extern buffered out port:32 p_bclk;
extern buffered out port:32 p_i2s_dac[I2S_PORTS_DAC];
extern buffered in port:32 p_i2s_adc[I2S_PORTS_ADC];
extern port p_mclk;

extern clock clk_audio_bclk;
extern clock clk_audio_mclk;

// Soft divide BCK off MCK
static inline void bck_32_ticks(out buffered port:32 bck)
{
#if BCLK_DIV == 2
  bck <: 0x55555555;
  bck <: 0x55555555;
#elif BCLK_DIV == 4
  bck <: 0x33333333;
  bck <: 0x33333333;
  bck <: 0x33333333;
  bck <: 0x33333333;
#elif BCLK_DIV == 8
  bck <: 0x0F0F0F0F;
  bck <: 0x0F0F0F0F;
  bck <: 0x0F0F0F0F;
  bck <: 0x0F0F0F0F;
  bck <: 0x0F0F0F0F;
  bck <: 0x0F0F0F0F;
  bck <: 0x0F0F0F0F;
  bck <: 0x0F0F0F0F;
#else
#error "MCK/BCK ratio must be 2, 4 or 8"
#endif
}

/* Config ports for I2C (CODEC Slave) */
void ConfigAudioPorts(unsigned int divide)
{
    if(divide == 1)
    {
        printstr("Divide currently not supported\n");
        while(1);
    }
    /* clock block 1 clocked off master clock port */
    set_clock_src(clk_audio_mclk, p_mclk);

    /* Clock block 2 clocked off BCK (which is generated on-chip) */
    set_clock_src(clk_audio_bclk, p_bclk);

    /*BCLK port clocked off clock block 1 (master clock) */
    set_port_clock(p_bclk, clk_audio_mclk);

    /* LRCLK and all data ports clocked off clock block 2 (bit clock) */
    set_port_clock(p_lrclk, clk_audio_bclk);
    for (int i = 0; i < I2S_PORTS_ADC; i++) 
    {
        set_port_clock(p_i2s_adc[i], clk_audio_bclk);
    }
    for (int i = 0; i < I2S_PORTS_DAC; i++) 
    {
        set_port_clock(p_i2s_dac[i], clk_audio_bclk);
    }

    /* start clock blocks after configuration */
    start_clock(clk_audio_mclk);
    start_clock(clk_audio_bclk);
}
/*****************************************************************************/
void audio_io( // Audio I/O thread
	streaming chanend c_aud_dsp // Channel between I/O and Processing threads
)
{
    int lr = 0;
    unsigned sampsAdc[I2S_CHANS_ADC];
    unsigned sampsDac[I2S_CHANS_DAC];
 
    /* Samps innit */
    for (int i = 0; i < I2S_CHANS_ADC; i++)
    {
        sampsAdc[i] = 0;
    }
    for (int i = 0; i < I2S_CHANS_DAC; i++)
    {
        sampsDac[i] = 0;
    }
    
    /* Configure the ports for audio */
    ConfigAudioPorts(BCLK_DIV);    

    /* Configure the clocking and CODECs */
    CodecInit();
    CodecConfig(SAMP_FREQ, MCLK_FREQ);   

    /* Lets do some I2S.. */
    // inputs and outputs are 32 bits at a time
    // assuming clock block is reset - initial time is 0
	// split SETPT from IN using asm - basically a split transaction with BCK generation in between
    // input is always "up to" given time, output is always "starting from" given time
	// outputs will be aligned to WCK + 1 (first output at time 32, WCK at time 31)
	// inputs will also be aligned to WCK + 1 (first input up to time 63, WCK up to time 62)
    for (int i = 0; i < I2S_PORTS_DAC; i++) 
    {
        p_i2s_dac[i] @ 32 <: 0;
    }
  
    for (int i = 0; i < I2S_PORTS_ADC; i++) 
    {
        asm("setpt res[%0], %1" :: "r"(p_i2s_adc[i]), "r"(63));
    }
		p_lrclk @ 31 <: 0;

    // clocks for previous outputs / inputs
    bck_32_ticks(p_bclk);
    bck_32_ticks(p_bclk);

#pragma unsafe arrays
    while (1) 
    {
        int p = 0;

#pragma loop unroll
        for (int i = 0; i < I2S_CHANS_ADC; i++)
            c_aud_dsp <: sampsAdc[i];
       
#pragma loop unroll
        for (int i = 0; i < I2S_CHANS_DAC; i++)
            c_aud_dsp :> sampsDac[i];
        
        // output audio data
        // expected to come from channel end as left-aligned
        p = 0;
#pragma loop unroll
        for (int i = 0; i < I2S_CHANS_DAC; i+=2) 
        { 
            p_i2s_dac[p++] <: bitrev(sampsDac[i]);
        }

        // drive word clock
        p_lrclk <: lr;
        lr = ~lr;

        // input audio data
        // will be output to channel end as left-aligned
		// compiler would insert SETC FULL on DIN input, because it doesn't know about inline SETPT above
		// hence we need inline IN too
        p = 0;
#pragma loop unroll
        for (int i = 0; i < I2S_CHANS_ADC; i+=2) 
        {
            int x;

						asm("in %0, res[%1]" : "=r"(x)  : "r"(p_i2s_adc[p++]));
            sampsAdc[i] = bitrev(x);
        }

        // drive bit clock
        bck_32_ticks(p_bclk);

        // output audio data
        // expected to come from channel end as left-aligned
        p = 0;
#pragma loop unroll
        for (int i = 1; i < I2S_CHANS_DAC; i+=2) 
        { 
            p_i2s_dac[p++] <: bitrev(sampsDac[i]);
        }

        // drive word clock
        p_lrclk <: lr;
        lr = ~lr;

        // input audio data
        // will be output to channel end as left-aligned
		// compiler would insert SETC FULL on DIN input, because it doesn't know about inline SETPT above
		// hence we need inline IN too
        p = 0;
#pragma loop unroll
        for (int i = 1; i < I2S_CHANS_ADC; i+=2) 
        {
            int x;

				    asm("in %0, res[%1]" : "=r"(x)  : "r"(p_i2s_adc[p++]));
            sampsAdc[i] = bitrev(x);
        }

        // drive bit clock
        bck_32_ticks(p_bclk);
  }
} // audio_io
/*****************************************************************************/
// audio_io.xc
