/******************************************************************************\
 * Header:  gp_io
 * File:    gp_io.h
 *
 * Description: Definitions, types, and prototypes for gp_io.xc
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

#ifndef _GP_IO_H_
#define _GP_IO_H_

#include <assert.h>

#include <platform.h>
#include<xs1.h>
#include<print.h>

//MB~ #include "i2c.h"
#include "app_global.h"

//MB~ #define I2C_NO_REGISTER_ADDRESS 1
#define debounce_time XS1_TIMER_HZ/50


// Different LED states: NB LED Selects are active low
typedef enum LED_STATES_TAG //
{
	NO_LEDS = 0xF, // All LED's Off
	LED_0 = 0xE,
	LED_1 = 0xD,
	LED_2 = 0xB,
	LED_3 = 0x7,
	ALL_LEDS = 0x0,  // All LED's On
  NUM_LED_STATES   // Handy Value!-)
} LED_STATES_ENUM;

/*	Different Button States
 *	The 'button port' on the GPIO slice returns values depending on which buttons are pressed
 *	The button states are 'active off' as shown by the port values in the following table
 *
 *          | BUTTON_2 (SW1)
 * BUTTON_1 | --------------
 *   (SW1)  |   OFF    ON
 * ---------|   ---    --
 *   OFF    |    3      1
 *    ON    |    2      0
 */
typedef enum BUTTON_STATES_TAG //
{
	ALL_BUTTONS = 0,	// port value returned when both buttons are pressed
	BUTTON_2,					// port value returned for when just button 2 (SW2) pressed
	BUTTON_1,					// port value returned for when just button 1 (SW1) pressed
	BUTTONS_OFF,			// port value returned for when both buttons are pressed
  NUM_BUTTON_STATES   // Handy Value!-)
} BUTTON_STATES_ENUM;

/** Structure containing GPIO parameters */
typedef struct GPIO_TAG //
{
	BUTTON_STATES_ENUM curr_buttons; // current stable button state
	BUTTON_STATES_ENUM prev_buttons; // previous stable button state
	LED_STATES_ENUM leds; // current state of LED's
} GPIO_S;


/******************************************************************************/
void gp_io(
	chanend c_gpio // GPIO end of channel between GPIO and DSP coar
);
/******************************************************************************/

#endif // _GP_IO_H_
/******************************************************************************/
// gp_io.h
