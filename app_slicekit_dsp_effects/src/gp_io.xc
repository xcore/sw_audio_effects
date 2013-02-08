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
void process_new_buttons_state( // Analyse new button state, and act accordingly!-)
	chanend c_gpio, // GPIO end of channel between GPIO and DSP coars
	BUTTON_STATES_ENUM button_state // current button state
)
{
	static unsigned led_select = 0x5; // Preset LED select to 1st and 3rd LED


	switch( button_state )
	{
		case ALL_BUTTONS : // port value returned when both buttons are pressed
			printstrln("Both Buttons Pressed");
		break; // case ALL_BUTTONS
	
		case BUTTON_2 : // port value returned for when just button 2 (SW2) pressed
//			printstrln("Button 2 Pressed");

			p_led <: ~(led_select & 0xc); // Light either 3rd or 4th LED for BUTTON_2
			led_select = ~led_select ; // Invert LED select

			c_gpio <: button_state; // Send button state to DSP control thread
		break; // case BUTTON_2 
	
		case BUTTON_1 :	// port value returned for when just button 1 (SW1) pressed
//			printstrln("Button 1 Pressed");

			p_led <: ~(led_select & 0x3); // Light either 1st and 2nd LED for BUTTON_1
			led_select = ~led_select ; // Invert LED select

			c_gpio <: button_state; // Send button state to DSP control thread
		break; // case BUTTON_1 
	
		case BUTTONS_OFF : // port value returned for when both buttons are pressed
//			printstrln("No Buttons Pressed");
		break; // case BUTTONS_OFF 

		default:	
			printstr("ERROR: Button state NOT supported :");
			printintln( button_state );
			assert(0 == 1);
		break; // default
	} // switch( button_state )
} // process_new_buttons_state
/*****************************************************************************/
void gp_io( // GPIO coar
	chanend c_gpio // GPIO end of channel between GPIO and DSP coars
)
{
	BUTTON_STATES_ENUM early_buttons = BUTTONS_OFF; // preset early buttons state to 'all buttons off'
	BUTTON_STATES_ENUM late_buttons; // late buttons state
	int confirmed = 1; // Preset new button state to 'confirmed'
	unsigned cur_time;
	timer clk; // timer


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
			p4_PORT_BUT_1 :> late_buttons;	// Re-load same values again
			clk :> cur_time; // Get time when button values reloaded

			// Check if early and late values are identical
			if(early_buttons == late_buttons)
			{
				process_new_buttons_state( c_gpio ,late_buttons );

				confirmed = 1; // Switch to confirmed state
			} // if (early_buttons==late_buttons)
			else
			{
				early_buttons = late_buttons; // Update early value, and try again
			} // if (early_buttons==late_buttons)
		} // else !(confirmed)
	} // while(1)

} // gp_io
/*****************************************************************************/
// gp_io.xc
