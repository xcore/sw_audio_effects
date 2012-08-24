/**
 * Module:  app_usb_aud_l1
 * Version: 3v0
 * Build:   1f7b79ca20887343a33d1cd7bf8d08682f85d363
 * File:    lock.S
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
/** lock.S
  * @brief     Functions for using hardware locks
  * @author    Ross Owen, XMOS Limited
  */

#include <xs1_user.h>

/////////////////////////////////////////////////////////////////////////
// void GetLockResource()
.extern GetLockResource
.globl GetLockResource.nstackwords
.linkset GetLockResource.nstackwords, 0
.globl GetLockResource
.text 
GetLockResource:
  getr		r0, XS1_RES_TYPE_LOCK
  retsp 	0


/////////////////////////////////////////////////////////////////////////
// void ClaimLock(lock l)
.extern ClaimLock
.globl ClaimLock.nstackwords
.linkset ClaimLock.nstackwords, 0
.globl ClaimLock
.text 
ClaimLock:
  in		r0, res[r0]
  retsp 	0


/////////////////////////////////////////////////////////////////////////
// void FreeLock(lock l)
.extern FreeLock
.globl FreeLock.nstackwords
.linkset FreeLock.nstackwords, 0
.globl FreeLock
.text 
FreeLock:
  out		res[r0], r0
  retsp 	0


/////////////////////////////////////////////////////////////////////////
// void FreeLockResource(lock l)
.extern FreeLockResource
.globl FreeLockResource.nstackwords
.linkset FreeLockResource.nstackwords, 0
.globl FreerLockResource
.text 
FreeLockResource:
  freer		res[r0]
  retsp 	0


