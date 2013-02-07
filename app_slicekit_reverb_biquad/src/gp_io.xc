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
on stdcore[GPIO_TILE]: port p_PORT_BUT_1 = XS1_PORT_4C;

/*****************************************************************************/
void gp_io( // Audio I/O coar
	chanend c_gpio // GPIO end of channel between GPIO and DSP coars
)
{
	unsigned led_value = 0x0E;
	unsigned button_press_1;
	unsigned button_press_2;
	int button = 1;
	unsigned cur_time;
	timer clk; // timer


	p_PORT_BUT_1:> button_press_1;


	set_port_drive_low( p_PORT_BUT_1 );

//::Config
	clk :> cur_time;


	printstrln("GPIO Runnig ...");

	// Loop forever
	while(1)
	{
		select
		{
			case button => p_PORT_BUT_1 when pinsneq(button_press_1):> button_press_1: //checks if any button is pressed
				button=0;
				clk:>cur_time;
			break; // case button => p_PORT_BUT_1

			 // Waits for 20ms and checks if the same button is pressed or not
			case !button => clk when timerafter(cur_time + debounce_time) :> void:
				p_PORT_BUT_1 :> button_press_2;

				if(button_press_1 == button_press_2)
				{
					if(button_press_1 == BUTTON_PRESS_VALUE) //Button 1 is pressed
					{
						printstrln("Button 1 Pressed");
						p_led <: led_value;
						led_value = led_value << 1;
						led_value |= 0x01;
						led_value = led_value & 0x0F;

						if(led_value == 15)
						{
							led_value = 0x0E;
						} // if (led_value == 15)
					} // if (button_press_1 == BUTTON_PRESS_VALUE) //Button 1 is pressed
				} // if (button_press_1==button_press_2)

				if(button_press_1 == BUTTON_PRESS_VALUE-1) //Button 2 is pressed
				{
					printstrln("Button 2 Pressed");
				} // if (button_press_1 == BUTTON_PRESS_VALUE-1) //Button 2 is pressed

				button = 1;

			break; // case !button
		} // select
	} // while(1)

} // gp_io
/*****************************************************************************/
// gp_io.xc
