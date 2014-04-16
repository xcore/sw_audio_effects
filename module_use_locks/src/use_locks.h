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
 *
 *****************************************************************************
 *
 * The use_locks functions work by accessing a global variable.
 * As multiple functions in multiple cores have access to this variable,
 * it breaks the .XC programming guidelines.
 * So the functions are accessed via a wrapper in a 'C' file with a static global variable.
 * In this way, the global variable is 'hidden' from the XC compiler.
 **/

#ifndef _USE_LOCKS_H_
#define _USE_LOCKS_H_

#include "hwlock.h"

/*****************************************************************************/
/** Initialise Hardware locks (MutEx's) */
void init_locks( void );
/*****************************************************************************/
/** Free Hardware locks (MutEx's) */
void free_locks( void );
/*****************************************************************************/
/** Acquire Hardware lock (MutEx) */
void acquire_lock( void );
/*****************************************************************************/
/** Release Hardware lock (MutEx) */
void release_lock( void );
/*****************************************************************************/

#endif /* _USE_LOCKS_H_ */
