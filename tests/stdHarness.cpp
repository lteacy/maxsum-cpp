
#include <cmath>
#include <cstdlib>
#include <bitset>
#include <algorithm>
#include <vector>
#include <iostream>
#include "DiscreteFunction.h"

using namespace maxsum;

/**
 * Function type for vectorised DiscreteFunction operations.
 */
typedef DiscreteFunction(*VecFcn)(const DiscreteFunction&);

/**
 * Function type for element-wise operations.
 */
typedef ValType(*ScalarFcn)(ValType);

/**
 * Ensures that VecFcn(f1).at(k) is equal to ScalarFcn(f1(k)) for all k.
 * That is, ensures that applying the given vectorised operation to a
 * DiscreteFunction has the same result as applying the scalar operation
 * to each element of the function.
 */
template<VecFcn Vf, ScalarFcn Sf> int testUnaryFunction
(
 DiscreteFunction f1
)
{
   //***************************************************************************
   // Calculate vectorised function value
   //***************************************************************************
   DiscreteFunction result = Vf(f1);

   //***************************************************************************
   // Ensure that function domain remains unchanged
   //***************************************************************************
   if(!sameDomain(f1,result))
   {
      std::cout << "Wrong domain for result.\n";
      return EXIT_FAILURE;
   }

   //***************************************************************************
   // Check that result is correct across entire domain
   //***************************************************************************
   for(DomainIterator it(result); it.hasNext(); ++it)
   {
      ValType curInput = f1.at(it.getInd());
      ValType correctResult = Sf(curInput);
      ValType actualResult = result.at(it.getInd());
      ValType diff = std::fabs(1-actualResult/correctResult);
      if(diff > DEFAULT_VALUE_TOLERANCE)
      {
         std::cout << "Wrong result for element [" << it.getInd() << "]\n";
         return EXIT_FAILURE;
      }
   }

   return EXIT_SUCCESS;

} // testUnaryFunction

int testDualFunctions(DiscreteFunction f1, DiscreteFunction f2)
{
   //***************************************************************************
   // Calculate domain union for all three operands
   //***************************************************************************
   std::vector<VarID> domainUnion;
   domainUnion.reserve(f1.noVars()+f2.noVars());
   domainUnion.insert(domainUnion.end(),f1.varBegin(),f1.varEnd());
   domainUnion.insert(domainUnion.end(),f2.varBegin(),f2.varEnd());
   std::sort(domainUnion.begin(),domainUnion.end());
   std::vector<VarID>::iterator newEnd =
      std::unique(domainUnion.begin(),domainUnion.end());
   domainUnion.resize(newEnd-domainUnion.begin());

   //***************************************************************************
   // Calculate function values (at the moment - only pow)
   //***************************************************************************
   DiscreteFunction powFcn = std::pow(f1,f2);

   //***************************************************************************
   // Ensure that function domain remains unchanged
   //***************************************************************************
   if(!sameDomain(f1+f2,powFcn))
   {
      std::cout << "Wrong domain for result.\n";
      return EXIT_FAILURE;
   }

   //***************************************************************************
   // Check that add results are correct for each element
   //***************************************************************************
   for(DomainIterator it(domainUnion.begin(),domainUnion.end());
         it.hasNext(); ++it)
   {
      ValType f1_j = f1(domainUnion,it.getSubInd());
      ValType f2_j = f2(domainUnion,it.getSubInd());
      ValType correctPower = std::pow(f1_j,f2_j);
      ValType  actualPower = powFcn.at(it.getInd());
      ValType diff = std::fabs(1-actualPower/correctPower);
      if(diff > DEFAULT_VALUE_TOLERANCE)
      {
         std::cout << "Wrong result for pow[" << it.getInd() << "]\n";
         return EXIT_FAILURE;
      }

   } // for loop

   return EXIT_SUCCESS;

} // testDualFunctions

int testMath()
{
   //***************************************************************************
   // Construct test functions for different subsets of variables.
   //***************************************************************************
   std::vector<DiscreteFunction> funcs;
   funcs.reserve(0x20);
   VarID vars[] = {1,2,3,101,104};
   for(unsigned long v = 0x0; v < 0x20; v+=4)
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
   // Test all functions
   //***************************************************************************
   int errorCount = 0;
   for(int k=0; k<funcs.size(); k++)
   {
      for(int j=0; j<funcs.size(); j++)
      {
         std::cout << "TESTING POW [" << k << "," << j << "]\n";
         errorCount += testDualFunctions(funcs[k],funcs[j]);
      }
      std::cout << "TESTING LOG [" << k << "]\n";
      errorCount += testUnaryFunction<std::log,std::log>(funcs[k]);
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
      ValIndex sizes[] = {15,10,5,11,12,9,6};
      registerVariables(vars,vars+7,sizes,sizes+7);

      //************************************************************************
      // Test function results
      //************************************************************************
      std::cout << "******************************************\n";
      std::cout << " Test stdlib Functions\n";
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

