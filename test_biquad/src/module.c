/*****************************************************************************\
Program File: module.c
  Author(s): Mark Beaumont

Description: General messing around test program

\*****************************************************************************/

#include "module.h"

/*****************************************************************************/
int mod(
	int inp,
	int out
)
{
	static int flag = 0;

	if (0 == flag)
	{
		printf("Program module starts\n");
		flag = 1;
	} // if (flag = 0)

	printf("%3d --> %3d \n" ,inp ,out);

  return 0;
} // main
/*****************************************************************************/
// module.c
