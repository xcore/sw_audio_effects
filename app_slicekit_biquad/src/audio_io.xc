#include <xclib.h>
#include <xs1.h>
#include <print.h>

#include "app_global.h"
#include "codec.h"

#include "i2s_master.h"


/*****************************************************************************/
void audio_io( // Audio I/O thread
	streaming chanend c_aud_dsp, // Channel between I/O and Processing threads
	struct r_i2s &r_i2s			 // Struture for I2S resources		
)
{
	
    /* Configure the clocking and CODECs */
    CodecInit();
    CodecConfig(SAMP_FREQ, MCLK_FREQ);
	  
	/* Call I2S master loop */
	i2s_master(r_i2s, c_aud_dsp, BCLK_DIV);

} // audio_io
/*****************************************************************************/
// audio_io.xc
