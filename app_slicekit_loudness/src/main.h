/******************************************************************************\
 * Header:  main
 * File:    main.h
 * Author: Mark Beaumont
 * Description: Definitions, types, and prototypes for main.xc
 *
 * Version: 0v1
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

#ifndef _MAIN_H_
#define _MAIN_H_

#include <xs1.h>
#include <platform.h>
#include <stdio.h>
#include <xscope.h>
#include <print.h>
#include <xclib.h>

#include "global.h"
#include "codec.h"
#include "audio_io.h"
#include "dsp_loudness.h"

/* Audio I/O */
#if 2 == I2S_PORTS_DAC
on stdcore[AUDIO_IO_CORE] : buffered out port:32 p_i2s_dac[I2S_PORTS_DAC] = {PORT_I2S_DAC_DATA0, PORT_I2S_DAC_DATA1};
#elif 1 == I2S_PORTS_DAC
on stdcore[AUDIO_IO_CORE] : buffered out port:32 p_i2s_dac[I2S_PORTS_DAC] = {PORT_I2S_DAC_DATA0};
#else // 1 == I2S_PORTS_DAC
#error Only 1 or 2 DAC Ports Supported
#endif // 2 < I2S_PORTS_DAC

#if 2 == I2S_PORTS_ADC
on stdcore[AUDIO_IO_CORE] : buffered in port:32 p_i2s_adc[I2S_PORTS_ADC] = {PORT_I2S_ADC_DATA0, PORT_I2S_ADC_DATA1}; 
#elif 1 == I2S_PORTS_ADC
on stdcore[AUDIO_IO_CORE] : buffered in port:32 p_i2s_adc[I2S_PORTS_ADC] = {PORT_I2S_ADC_DATA0}; 
#else // 1 == I2S_PORTS_ADC
#error Only 1 or 2 ADC Ports Supported
#endif // 2 < I2S_PORTS_ADC

#endif // _MAIN_H_
/******************************************************************************/
// main.h
