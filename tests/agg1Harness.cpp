/**
 * @file agg1Harness.cpp
 * Test Harness for aggregation functions.
 */
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <bitset>
#include <algorithm>
#include <vector>
#include <iostream>
#include "maxsum/DiscreteFunction.h"

using namespace maxsum;

/**
 * Utility function for checking that doubles are nearly equal.
 */
inline bool nearlyEqual_m
(
 double x,
 double y,
 double tol=maxsum::DEFAULT_VALUE_TOLERANCE
)
{
   double relDiff = 1-x/y;
   double absDiff = x-y;
   bool relOK = (tol>relDiff) && (-tol < relDiff);
   bool absOK = (tol>absDiff) && (-tol < absDiff);
   return relOK | absOK;
}

int testMarginals(const DiscreteFunction inFun)
{
   int errorCount = 0;

   //***************************************************************************
   // Calculate aggregates over entire domain of input function
   //***************************************************************************
   double mean=0;
   double max=-DBL_MAX;
   double maxnorm=-DBL_MAX;
   long argmax=-1;
   double domainSize = inFun.domainSize();
   for(int k=0; k<inFun.domainSize(); ++k)
   {
      double val = inFun(k);
      double absVal = std::fabs(val);
      mean += val / domainSize;

      if(maxnorm<absVal)
      {
         maxnorm=absVal;
      }

      if(max<val)
      {
         max=val;
         argmax=k;
      }

   } // for loop

   //***************************************************************************
   // Check results for consistency
   //***************************************************************************
   const double funMean = inFun.mean();
   const double funMax = inFun.max();
   const double funMaxnorm = inFun.maxnorm();
   const long funArgMax = inFun.argmax();

   if(!nearlyEqual_m(max,funMax))
   {
      std::cout << "max: " << funMax << " should be " << max << '\n';
      ++errorCount;
   }

   if(!nearlyEqual_m(mean,funMean))
   {
      std::cout << "mean: " << funMean << " should be " << mean << '\n';
      ++errorCount;
   }

   if(!nearlyEqual_m(maxnorm,funMaxnorm))
   {
      std::cout << "maxnorm: " << funMaxnorm << " should be "
         << maxnorm << '\n';
      ++errorCount;
   }

   if(funArgMax!=argmax)
   {
      std::cout << "argmax: " << funArgMax << " should be " << argmax << '\n';
      ++errorCount;
   }

   //***************************************************************************
   // Return number of errors
   //***************************************************************************
   return errorCount;

} // testMarginals

int testMath()
{
   //***************************************************************************
   // Construct test functions for different subsets of variables.
   //***************************************************************************
   std::vector<DiscreteFunction> funcs;
   funcs.reserve(0x20);
   VarID vars[] = {1,2,3,101,104};
   for(unsigned long v = 0x0; v < 0x20; v++)
   {
      std::bitset<5> inDomain(v);

      std::vector<VarID> selVars;
      selVars.reserve(inDomain.size());
      for(int k=0; k<5; k++)
      {
         if(inDomain[k])
         {
            selVars.push_back(vars[k]);
         }
      }
      std::sort(selVars.begin(),selVars.end());

      funcs.push_back(DiscreteFunction(selVars.begin(),selVars.end()));

   } // outer for loop

   //***************************************************************************
   // Populate functions with random values.
   //***************************************************************************
   for(std::vector<DiscreteFunction>::iterator it=funcs.begin();
         it!=funcs.end(); ++it)
   {
      for(int k=0; k<it->domainSize(); ++k)
      {
         int randInt = rand() % 1000 - 500;
         DiscreteFunction& curFunc = *it;
         double value = static_cast<ValType>(randInt);
         //value = value / 100;
         curFunc.at(k) = value;
      }
   }

   //***************************************************************************
   // Test each function by marginalising over different subsets of its
   // domain.
   //***************************************************************************
   int errorCount = 0;
   for(int k=0; k<funcs.size(); k++)
   {
      std::cout << "TESTING [" << k << ": A]\n";
      errorCount += testMarginals(funcs[k]);
      std::cout << "TESTING [" << k << ": B]\n";
      funcs[k] += funcs[k].maxnorm();
      errorCount += testMarginals(funcs[k]);
   }
   std::cout << "Number of failures: " << errorCount << std::endl;
   return errorCount;

} // testMath

int main()
{
   try
   {
      //************************************************************************
      // Register some variables for test purposes
      //************************************************************************
      VarID vars[] = {1,2,3,101,102,103,104};
      ValIndex sizes[] = {15,5,5,10,3,2,6};
      registerVariables(vars,vars+7,sizes,sizes+7);

      //************************************************************************
      // Quick test for argmax and argmax2
      //************************************************************************
      std::cout << "******************************************\n";
      std::cout << " Testing argmax and argmax 2\n";
      std::cout << "******************************************\n";
      DiscreteFunction test(2,0.0);
      test(0) = 32.0;
      test(1) = 62.0;
      test(2) = 42.0;
      test(3) = 61.0;
      test(4) = 32.0;

      ValIndex mx1 = test.argmax();
      ValIndex mx2 = test.argmax2(mx1);
      
      if(1!=mx1)
      {
         std::cout << "wrong argmax" << std::endl;
         return EXIT_FAILURE;
      }
      
      if(3!=mx2)
      {
         std::cout << "wrong argmax2" << std::endl;
         return EXIT_FAILURE;
      }
      
      //************************************************************************
      // Repeat test with different values
      //************************************************************************
      test(3) = 62.1;
      mx1 = test.argmax();
      mx2 = test.argmax2(mx1);

      if(3!=mx1)
      {
         std::cout << "wrong argmax" << std::endl;
         return EXIT_FAILURE;
      }
      
      if(1!=mx2)
      {
         std::cout << "wrong argmax2" << std::endl;
         return EXIT_FAILURE;
      }

      std::cout << "argmax appears OK" << std::endl;

      //************************************************************************
      // Test arithmetic operators
      //************************************************************************
      std::cout << "******************************************\n";
      std::cout << " Test Marginalisation\n";
      std::cout << "******************************************\n";
      int exitStatus = testMath();
      if(EXIT_SUCCESS!=exitStatus)
      {
         return EXIT_FAILURE;
      }
   }
   catch(std::exception& e)
   {
      std::cout << "Caught exception: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;

} // function main

