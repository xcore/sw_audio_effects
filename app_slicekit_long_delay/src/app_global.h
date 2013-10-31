/******************************************************************************\
 * Header:  global
 * File:    global.h
 *
 * Description: Global Definitions, types, and prototypes for reverb app
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

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define	AUDIO_IO_TILE 1	// Tile connected to Audio Slice board
#define	MEM_TILE 1	// Tile connected to Memory Slice board
#define	DSP_TILE MEM_TILE	// Tile used by DSP functions

#define	AUDIO_IO_TYPE 1	// Port-Type connected to Audio Slice board
#define	MEM_TYPE 0	// Port-Type connected to Memory Slice board

// Audio sample frequency (Hz)
#define SAMP_FREQ			48000

// Audio clocking defines
// Master clock defines (Hz)
#define MCLK_FREQ_441       (512*44100)   // 44.1, 88.2 etc
#define MCLK_FREQ_48        (512*48000)   // 48, 96 etc

#if (SAMP_FREQ%22050==0)
#define MCLK_FREQ           MCLK_FREQ_441
#elif (SAMP_FREQ%24000==0)
#define MCLK_FREQ           MCLK_FREQ_48
#else
#error Unsupported sample frequency
#endif

// Bit clock divide
#define BCLK_DIV        (MCLK_FREQ / (SAMP_FREQ * 64))

// Ports
#if (AUDIO_IO_TYPE == 1)
#define PORT_I2S_DAC0		XS1_PORT_1D
#define PORT_I2S_DAC1  		XS1_PORT_1H
#define PORT_I2S_ADC0  		XS1_PORT_1K
#define PORT_I2S_ADC1		XS1_PORT_1L
#define PORT_I2S_LRCLK		XS1_PORT_1I
#define PORT_I2S_BCLK		XS1_PORT_1A
#define PORT_MCLK_IN		XS1_PORT_1E

#define PORT_GPIO			XS1_PORT_4E
#define PORT_I2C			XS1_PORT_4F

#else
#error Currently un-supported Audio Slice H/W Type
#endif


#if MEM_TYPE == 1
#define PORT_SEL	XS1_PORT_16B	// Byte Select
#define PORT_CAS	XS1_PORT_1J		// Column Address Strobe
#define PORT_RAS	XS1_PORT_1I		// Row Address Strobe
#define PORT_WE		XS1_PORT_1K		// Write Enable
#define PORT_CLK	XS1_PORT_1L		// Clock

#elif MEM_TYPE == 0
#define PORT_SEL	XS1_PORT_16A	// Byte Select
#define PORT_CAS	XS1_PORT_1B		// Column Address Strobe
#define PORT_RAS	XS1_PORT_1G		// Row Address Strobe
#define PORT_WE		XS1_PORT_1C		// Write Enable
#define PORT_CLK	XS1_PORT_1F		// Clock

#else
#error Currently un-supported Memory Slice H/W Type
#endif

#endif // ifndef _GLOBAL_H_
/*****************************************************************************/
// global.h
