/******************************************************************************\
 * Header:  app_conf
 * File:    app_conf.h
 * Author: Mark Beaumont
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

#ifndef _APP_CONF_H_
#define _APP_CONF_H_

#define NUM_APP_CHANS 4

#define NUM_BIQUAD_CHANS NUM_APP_CHANS
//MB~ #define I2S_MASTER_NUM_CHANS_ADC NUM_APP_CHANS // Awaiting Ross fix
//MB~ #define I2S_MASTER_NUM_CHANS_DAC NUM_APP_CHANS // Awaiting Ross fix 

#endif // _APP_CONF_H_
/******************************************************************************/
// app_conf.h
