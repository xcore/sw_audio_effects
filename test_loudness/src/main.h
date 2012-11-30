/*****************************************************************************\
Include File: main.h
  Author(s): Mark Beaumont

Description: Definitions and types for main.c

\*****************************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>

#ifdef __XC__
// XC File

#else //if __XC__
// 'C' File

#endif // else __XC__

/*****************************************************************************/
void test_harness(); // Test Harness for BiQuad
/*****************************************************************************/

#endif /* ifndef _MAIN_H_ */
// main.h
