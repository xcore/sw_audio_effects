/**
 * Module:  app_usb_aud_l1
 * Version: 3v1
 * Build:   b9a8ea617bb00b44e9ff9d509cd45128a8675d95
 * File:    audiostream.xc
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
#include "port32A.h"

/* Functions that handle functions that must occur on stream 
 * start/stop e.g. DAC mute/un-mute.
 * These need implementing for a specific design.
 *
 * Implementations for the L1 USB Audio Reference Design 
 */

/* Any actions required for stream start e.g. DAC un-mute - run every 
 * stream start.
 * For L1 USB Audio Reference Design we illuminate LED B (connected 
 * to port 32A)
 */
void AudioStreamStart(void) {
    int x;
    x = port32A_peek();
    x |= P32A_LED_B;
    port32A_out(x);    
}

/* Any actions required on stream stop e.g. DAC mute - run every
 * stream stop 
 * For L1 USB Audio Reference Design we extinguish LED B (connected
 * to port 32A)
 */
void AudioStreamStop(void) {
    int x;
    x = port32A_peek();
    x &= (~P32A_LED_B);
    port32A_out(x);
}

