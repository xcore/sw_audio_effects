/******************************************************************************\
 * File:	gp_io.xc
 *
 * Description: GPIO Coar
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

#include "gp_io.h"

//::Port configuration
on stdcore[GPIO_TILE]: out port p_led = XS1_PORT_4A;
on stdcore[GPIO_TILE]: port p4_PORT_BUT_1 = XS1_PORT_4C; // 4-bit button port: bit_0 & bit_1 encode button state (See gp_io.h)

/*****************************************************************************/
void init_gpio( // Initialise GPIO data structure
	GPIO_S &gpio_gs // Reference to structure containing GPIO data
)
{
	gpio_gs.curr_buttons = BUTTONS_OFF;
	gpio_gs.prev_buttons = BUTTONS_OFF;
	gpio_gs.leds = NO_LEDS;
} // init_gpio
/*****************************************************************************/
void one_button_state( // Processing when Just one button pressed
	GPIO_S &gpio_gs, // Reference to structure containing GPIO data
	chanend c_gpio, // GPIO end of channel between GPIO and DSP coars
	LED_STATES_ENUM led_mask // Current LED mask (Active low)
)
{
	// Check for valid transition
	if (BUTTONS_OFF == gpio_gs.prev_buttons)
	{
		gpio_gs.leds |= led_mask; // Clear all other LED's
		gpio_gs.leds ^= ~led_mask; // Toogle status of Selected LED

		c_gpio <: gpio_gs.curr_buttons; // Send button state to DSP control thread
	} // if (BUTTONS_OFF == gpio_gs.prev_buttons)
} // one_button_state
/*****************************************************************************/
void process_new_buttons_state( // Do processing for confirmed new button state
	GPIO_S &gpio_gs, // Reference to structure containing GPIO data
	chanend c_gpio // GPIO end of channel between GPIO and DSP coars
)
// NB LED's are active low. Therefore bit-selects are inverted
{ 

	switch( gpio_gs.curr_buttons )
	{
		case ALL_BUTTONS : // Both buttons are pressed
			p_led <: NO_LEDS; // Switch off all LED's
			printstrln("Both Buttons Pressed");
		break; // case ALL_BUTTONS
	
		case BUTTON_2 : // Just button 2 (SW2) pressed
			one_button_state( gpio_gs ,c_gpio ,LED_3 ); 
		break; // case BUTTON_2 
	
		case BUTTON_1 :	// Just button 1 (SW1) pressed
			one_button_state( gpio_gs ,c_gpio ,LED_0 ); 
		break; // case BUTTON_1 
	
		case BUTTONS_OFF : // Both buttons Released
			p_led <: gpio_gs.leds; // Switch on/off requested LED's
		break; // case BUTTONS_OFF 

		default:	
			printstr("ERROR: Button state NOT supported :");
			printintln( gpio_gs.curr_buttons );
			assert(0 == 1);
		break; // default
	} // switch( gpio_gs.curr_buttons )

	gpio_gs.prev_buttons = gpio_gs.curr_buttons; // Update previous button value
} // process_new_buttons_state
/*****************************************************************************/
void gp_io( // GPIO coar
	chanend c_gpio // GPIO end of channel between GPIO and DSP coars
)
{
	GPIO_S gpio_gs; // Global data structure
	BUTTON_STATES_ENUM early_buttons = BUTTONS_OFF; // preset early buttons state to 'all buttons off'
	int confirmed = 1; // Preset new button state to 'confirmed'
	unsigned cur_time;
	timer clk; // timer


	init_gpio( gpio_gs ); // Initialise GPIO global data structure

	p4_PORT_BUT_1 :> early_buttons;

	set_port_drive_low( p4_PORT_BUT_1 ); // Set un-used pins low

//	printstrln("GPIO Running ...");

	// Loop forever
	while(1)
	{	/*	The 'confirmed state' is used to handle 'switch bounce':
		 *	We requires 2 identical button readings, separated by debounce_time (20ms)
		 *	to confirm a button selection.
		 */

		// Check if in confirmed state
		if (confirmed)
		{ // Confirmed state
			// Wait for button values to change, then load them
			p4_PORT_BUT_1 when pinsneq(early_buttons) :> early_buttons;
			clk :> cur_time; // Get time when button values changed

			confirmed = 0; // Switch to UN-confirmed state
		} // if (confirmed)
		else
		{ // UN-confirmed state
			// Wait for 'debounce_time', then re-load button values
			clk when timerafter(cur_time + debounce_time) :> void;
			p4_PORT_BUT_1 :> gpio_gs.curr_buttons;	// Re-load same values again
			clk :> cur_time; // Get time when button values reloaded

			// Check if early and late values are identical
			if(early_buttons == gpio_gs.curr_buttons)
			{
				process_new_buttons_state( gpio_gs ,c_gpio );

				confirmed = 1; // Switch to confirmed state
			} // if (early_buttons==gpio_gs.curr_buttons)
			else
			{
				early_buttons = gpio_gs.curr_buttons; // Update early value, and try again
			} // if (early_buttons==gpio_gs.curr_buttons)
		} // else !(confirmed)
	} // while(1)

} // gp_io
/*****************************************************************************/
// gp_io.xc
