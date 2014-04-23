/**
 * @file cHarness.cpp
 * Test harness for C interface.
 */

#include "stdlib.h"
#include "stdio.h"
#include "maxsum_c.h"

/**
 * Test harness for C interface.
 */
int main()
{
   /****************************************************************************
   * Check that unregistered variable isn't registered!
   ****************************************************************************/
   printf("Hello World!\n");
   if(isRegistered_ms(2))
   {
      printf("True returned for unregistered variable!\n");
      return EXIT_FAILURE;
   }
   else
   {
      printf("Unregistered variable check - OK\n");
   }

   /****************************************************************************
   * Try registering
   ****************************************************************************/
   registerVariable_ms(2,5);
   if(5!=getDomainSize_ms(2))
   {
      printf("Wrong domain size reported!\n");
   }
   else
   {
      printf("Domain size - OK\n");
   }

} /* main function */
