/**
 * The copyrights, all other intellectual and industrial
 * property rights are retained by XMOS and/or its licensors.
 * Terms and conditions covering the use of this code can
 * be found in the Xmos End User License Agreement.
 *
 * Copyright XMOS Ltd 2013
 *
 * In the case where this code is a modification of existing code
 * under a separate license, the separate license terms are shown
 * below. The modifications to the code are still covered by the
 * copyright notice above.
 **/

#include "use_locks.h"

static hwlock_t hwlock; // Global hardware lock

/*****************************************************************************/
void init_locks(void) // Initialise Hardware locks (MutEx's)
{
  hwlock = hwlock_alloc();
} // init_lock
/*****************************************************************************/
void free_locks(void) // Free Hardware locks (MutEx's)
{
  hwlock_free(hwlock);
} // free_lock
/*****************************************************************************/
void acquire_lock(void) // Acquire Hardware lock (MutEx)
{
  hwlock_acquire(hwlock);
} // acquiree_lock
/*****************************************************************************/
void release_lock(void) // Release Hardware lock (MutEx)
{
  hwlock_release(hwlock);
} // release_lock
/*****************************************************************************/

