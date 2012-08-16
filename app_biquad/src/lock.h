/**
 * Module:  app_usb_aud_l1
 * Version: 3v0
 * Build:   1f7b79ca20887343a33d1cd7bf8d08682f85d363
 * File:    lock.h
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
/** @file       lock.h
  * @brief      Functions for using hardware locks
  * @author     Ross Owen, XMOS Limited
  */

#ifndef _LOCK_H_
#define _LOCK_H_ 1

typedef unsigned lock

/* Allocates and returns a lock resource - returns 0 if out of lock */
lock GetLockResource() 

/* Claims the passed lock, this is a blocking call */
void ClaimLock(lock l)

/* Frees the passed lock */
void FreeLock(lock l)

/* De-allocated the passed lock resource */
void FreeLockResource(lock l)



#endif
