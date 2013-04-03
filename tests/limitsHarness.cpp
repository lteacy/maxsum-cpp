
#include <vector>
#include <iostream>
#include "maxsum/DiscreteFunction.h"

int main()
{
   std::cout << "******************************************\n";
   std::cout << "Numeric Limits \n";
   std::cout << "******************************************\n";

   int exitStatus = EXIT_SUCCESS;

   //***************************************************************************
   // Instantiate numeric limits for comparison
   //***************************************************************************
   typedef std::numeric_limits<maxsum::DiscreteFunction> funLimits;
   typedef std::numeric_limits<maxsum::ValType> valLimits;

   //***************************************************************************
   // Check limits
   //***************************************************************************
   if(funLimits::min()!=valLimits::min())
   {
      std::cout << "min mismatch\n";
      exitStatus = EXIT_FAILURE;
   }

   if(funLimits::max()!=valLimits::max())
   {
      std::cout << "max mismatch\n";
      exitStatus = EXIT_FAILURE;
   }

   if(funLimits::has_infinity!=valLimits::has_infinity)
   {
      std::cout << "infinity mismatch\n";
      exitStatus = EXIT_FAILURE;
   }

   if(funLimits::is_specialized!=valLimits::is_specialized)
   {
      std::cout << "specialisation mismatch\n";
      exitStatus = EXIT_FAILURE;
   }

   if(funLimits::is_signed!=valLimits::is_signed)
   {
      std::cout << "sign mismatch\n";
      exitStatus = EXIT_FAILURE;
   }

   if(funLimits::has_quiet_NaN!=valLimits::has_quiet_NaN)
   {
      std::cout << "quiet NaN mismatch\n";
      exitStatus = EXIT_FAILURE;
   }

   if(funLimits::digits!=valLimits::digits)
   {
      std::cout << "digit mismatch\n";
      exitStatus = EXIT_FAILURE;
   }

   if(funLimits::is_exact!=valLimits::is_exact)
   {
      std::cout << "exact mismatch\n";
      exitStatus = EXIT_FAILURE;
   }

   if(funLimits::max_exponent10!=valLimits::max_exponent10)
   {
      std::cout << "exponent mismatch\n";
      exitStatus = EXIT_FAILURE;
   }

   if(EXIT_SUCCESS!=exitStatus)
   {
      std::cout << "One or more limit tests failed.\n";
   }
   else
   {
      std::cout << "All limit tests passed.\n";
   }
   return exitStatus;

} // function main
