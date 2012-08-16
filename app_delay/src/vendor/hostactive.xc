/**
 * Module:  app_usb_aud_l1
 * Version: 3v1
 * Build:   b9a8ea617bb00b44e9ff9d509cd45128a8675d95
 * File:    hostactive.xc
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


#include <xs1.h>
#include "devicedefines.h"
#include "port32A.h"

#ifdef HOST_ACTIVE_CALL

void VendorHostActive(int active)
{
    int x;
   
    if(active)
    {
        x = port32A_peek();
        x |= P32A_LED_A;
        port32A_out(x);  
    }
    else
    {
        x = port32A_peek();
        x &= (~P32A_LED_A);
        port32A_out(x);
    }  
}


#endif
