/**
 * @file agg2Harness.cpp
 * Test Harness for aggregation functions.
 */
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <bitset>
#include <algorithm>
#include <vector>
#include <iostream>
#include "DiscreteFunction.h"

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

int testMarginals(const DiscreteFunction inFun, DiscreteFunction outFun)
{
   int errorCount = 0;

   //***************************************************************************
   // Check that outFun domain is a subset of inFun domain, as expected by
   // the marginalisation functions.
   //***************************************************************************
   if(std::includes(inFun.varBegin(),inFun.varEnd(),
            outFun.varBegin(),outFun.varEnd()))
   {
      std::cout << "outFun domain is subset of inFun domain - OK\n";
   }
   //***************************************************************************
   // Otherwise outFun has a larger domain than inFun. In this case, we make
   // sure that exceptions are thrown from all marginalisation functions.
   //***************************************************************************
   else
   {
      std::cout << "outFun domain is not subset of inFun domain - "
         "checking for expected errors...";
      //************************************************************************
      // Check for correct exception from maxMarginal
      //************************************************************************
      try
      {
         maxMarginal(inFun,outFun);
         std::cout << "\nMissing exception from maxMarginal";
         ++errorCount;
      }
      catch(BadDomainException e){ /* this is suppose to happen */ }

      //************************************************************************
      // Check for correct exception from minMarginal
      //************************************************************************
      try
      {
         minMarginal(inFun,outFun);
         std::cout << "\nMissing exception from minMarginal";
         ++errorCount;
      }
      catch(BadDomainException e){ /* this is suppose to happen */ }

      //************************************************************************
      // Check for correct exception from meanMarginal
      //************************************************************************
      try
      {
         meanMarginal(inFun,outFun);
         std::cout << "\nMissing exception from meanMarginal\n";
         ++errorCount;
      }
      catch(BadDomainException e){ /* this is suppose to happen */ }
      
      if(0==errorCount)
      {
         std::cout << "OK\n";
      }

      return errorCount;

   }  // else block

   //***************************************************************************
   // Apply the marginalisation functions
   //***************************************************************************
   const DiscreteFunction inCopy(inFun);

   DiscreteFunction maxFun(outFun);
   maxMarginal(inCopy,maxFun);

   DiscreteFunction minFun(outFun);
   minMarginal(inCopy,minFun);

   DiscreteFunction meanFun(outFun);
   meanMarginal(inCopy,meanFun);

   //***************************************************************************
   // Ensure that the function domains have not changed
   //***************************************************************************
   std::cout << "Checking domains...";
   bool domainChanged = false;
   if(!sameDomain(inCopy,inFun))
   {
      std::cout << "\nInput function has somehow changed domain.\n";
      domainChanged = true;
      ++errorCount;
   }

   if(!sameDomain(outFun,maxFun))
   {
      std::cout << "\nMax function has somehow changed domain.\n";
      domainChanged = true;
      ++errorCount;
   }

   if(!sameDomain(outFun,minFun))
   {
      std::cout << "\nMin function has somehow changed domain.\n";
      domainChanged = true;
      ++errorCount;
   }

   if(!sameDomain(outFun,meanFun))
   {
      std::cout << "\nMean function has somehow changed domain.\n";
      domainChanged = true;
      ++errorCount;
   }

   if(!domainChanged)
   {
      std::cout << "OK\n";
   }

   //***************************************************************************
   // Calculate the correct size of the free domain and fixed domains
   //***************************************************************************
   const double fixedDomainSize = outFun.domainSize();
   const double  freeDomainSize = inFun.domainSize() / outFun.domainSize();

   //***************************************************************************
   // Check that aggregations are correct for all values of the output
   // function.
   //***************************************************************************
   bool correctValues = true;
   int fixedCount = 0;
   std::cout << "Checking values...";
   for(DomainIterator outIt(outFun); outIt.hasNext(); ++outIt)
   {
      //************************************************************************
      // Variables for storing correct aggregation for currrent position
      // in output function
      //************************************************************************
      double max = -DBL_MAX;
      double min = DBL_MAX;
      double sum = 0;

      //************************************************************************
      // Calculate correct aggregations by iterating over the free variables
      // in the input function's domain.
      //************************************************************************
      DomainIterator inIt(inFun);
      inIt.condition(outIt);
      double freeCount = 0;
      while(inIt.hasNext())
      {
         double val = inFun(inIt.getInd());
         sum += val;
         max = max>val ? max : val; 
         min = min<val ? min : val; 
         ++inIt;
         ++freeCount;

      } // while loop
      double avg = sum / freeCount;

      //************************************************************************
      // Check that size of free domain is what we expect
      //************************************************************************
      if(freeDomainSize != freeCount)
      {
         std::cout << "\nWrong size for free domain: "
            << freeCount << " should be " << freeDomainSize;
         correctValues = false;
         ++errorCount;
         break;
      }

      //************************************************************************
      // Check that the maxMarginal provides the correct value
      //************************************************************************
      if(!nearlyEqual_m(max,maxFun(outIt.getInd())))
      {
         std::cout << "\nWrong value for maxMarginal: "
            << maxFun(outIt.getInd()) << " should be " << max;
         correctValues = false;
         ++errorCount;
         break;
      }

      //************************************************************************
      // Check that the minMarginal provides the correct value
      //************************************************************************
      if(!nearlyEqual_m(min,minFun(outIt.getInd())))
      {
         std::cout << "\nWrong value for minMarginal: " 
            << minFun(outIt.getInd()) << " should be " << min;
         correctValues = false;
         ++errorCount;
         break;
      }

      //************************************************************************
      // Check that the meanMarginal provides the correct value
      //************************************************************************
      if(!nearlyEqual_m(avg,meanFun(outIt.getInd())))
      {
         meanMarginal(inCopy,meanFun);
         std::cout << "\nWrong value for meanMarginal: "
            << meanFun(outIt.getInd()) << " should be " << avg;
         correctValues = false;
         ++errorCount;
         break;
      }

      ++fixedCount;

   } // for loop

   //************************************************************************
   // Check that size of fixed (output) domain is what we expect
   // Here we condition on correctValues, because previous errors
   // may invalidate the count.
   //************************************************************************
   if(correctValues && (fixedDomainSize != fixedCount))
   {
      std::cout << "\nWrong size for fixed domain: " << fixedCount
         << " should be " << fixedDomainSize;
      ++errorCount;
   }

   //************************************************************************
   // If we have no errors at this point, print ok in test log
   //************************************************************************
   if(correctValues)
   {
      std::cout << "OK";
   }
   std::cout << std::endl;

   //************************************************************************
   // Return the number of errors.
   //************************************************************************
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
      for(int j=0; j<funcs.size(); j++)
      {
         std::cout << "TESTING [" << k << "," << j << "]\n";
         DiscreteFunction f1(funcs[k]), f2(funcs[j]);
         errorCount += testMarginals(f1,f2);
      }
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

