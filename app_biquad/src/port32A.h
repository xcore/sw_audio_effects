/**
 * Module:  app_usb_aud_l1
 * Version: 3v0
 * Build:   74d56e62d5f22803dd8aa38c1c75021fb1f6e7ec
 * File:    port32A.h
 *
 * The copyrights, all other intellectual and industrial 
 * property rights are retained by XMOS and/or its licensors. 
 * Terms and conditions covering the use of this code can
 * be found in the Xmos End User License Agreement.
 *
 * Copyright XMOS Ltd 2010
 *
 * In the case where this code is a modification of existing code
 * under a separate license, the separate license terms are shown
 * below. The modifications to the code are still covered by the 
 * copyright notice above.
 *
 **/                                   
/**
 * @file        port32A.h
 * @brief       Port 32A access functions and bit defs for L1 USB Audio board 
 * @author      Ross Owen, XMOS Semiconductor 
 * @version     1.0
 * @date        30/01/2010
 */

#ifndef _PORT32A_OUT_
#define _PORT32A_OUT_ 

/* Bit defs */
#define P32A_USB_RST       0x01           /* ULPI rst line */  
#define P32A_COD_RST       0x02           /* Codec rst line */
#define P32A_CLK_SEL       0x04           /* MClk Select line */
#define P32A_LED_A         0x08           /* LED A */
#define P32A_LED_B         0x10           /* LED B */

/* Outputs passed value to port 32A */
void port32A_out(int value);

/* Performs peek operation on port 32A and returns value on pins */
unsigned port32A_peek();

#endif
