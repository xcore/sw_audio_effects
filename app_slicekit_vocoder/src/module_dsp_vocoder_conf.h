/******************************************************************************\
 * Header:  module_dsp_vocoder_conf
 * File:    module_dsp_vocoder_conf.h
 *
 * Description: Definitions for configuration of application.
 *							NB These definitions will be passed to all modules used in application
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

#ifndef _MODULE_DSP_VOCODER_CONF_H_
#define _MODULE_DSP_VOCODER_CONF_H_

#include "app_conf.h"

/** No. of input audio channels, Carrier & Modulator on seperate  Stereo channels */
#define NUM_VOCODER_INP_CHANS NUM_APP_CHANS

/** No. of output audio channels, Mixed down onto Stereo output channels */
#define NUM_VOCODER_OUT_CHANS 2 

/** Number of Slave cores used in this application */
#define NUM_VOCODER_SLAVES NUM_APP_SLAVES

/** Number of Vocoder bands processed on each slave core */
#define NUM_VOCODER_SLAVE_BANDS NUM_APP_SLAVE_BANDS

#endif // _MODULE_DSP_VOCODER_CONF_H_
/******************************************************************************/
// module_dsp_vocoder_conf.h
