/**
 * Module:  app_usb_aud_l1
 * Version: 3v3rc3
 * Build:   be601766ebcbdfdd84de2b4fb8b48b824b5bc6c3
 * File:    codec.xc
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
#include <print.h>
#include "port32A.h"
#include "devicedefines.h"

//:codec_init
void CodecInit(chanend ?c_codec) 
{
    return;
}
//:

//:codec_config
void CodecConfig(unsigned samFreq, unsigned mClk, chanend ?c_codec)
{
    timer t;
    unsigned time;
    unsigned portVal;
    unsigned tmp;

    /* Put codec in reset and set master clock select appropriately */
    if ((samFreq % 22050) == 0)
    {
        portVal = P32A_USB_RST;
    }
    else if((samFreq % 24000) == 0)
    {
        portVal = (P32A_USB_RST | P32A_CLK_SEL);
    }
    else
    {
        if (samFreq == 1234)
          return;
        printintln(samFreq);
        printstr("Unrecognised sample freq in ConfigCodec\n");
    }

    tmp = port32A_peek();
    tmp &= (P32A_LED_A | P32A_LED_B);
    port32A_out(portVal | tmp);

    /* Hold in reset for 2ms */
    t :> time;
    time += 200000;
    t when timerafter(time) :> int _;


    /* Codec out of reset */
    portVal |= P32A_COD_RST;
    tmp = port32A_peek();
    tmp &= (P32A_LED_A | P32A_LED_B);
    port32A_out(portVal | tmp);
}
//:
