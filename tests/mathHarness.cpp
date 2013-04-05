
#include <cmath>
#include <cstdlib>
#include <bitset>
#include <algorithm>
#include <vector>
#include <iostream>
#include "maxsum/DiscreteFunction.h"

using namespace maxsum;

int testExpansion()
{

   int exitStatus = 0;
   //***************************************************************************
   // Create some functions that depend on variables
   //***************************************************************************
   VarID vars[] = {1,2,3};
   DiscreteFunction a(vars+0,vars+1), b(vars+1,vars+2), c(vars+2,vars+3);;

   //***************************************************************************
   // Populate variables
   //***************************************************************************
   const ValType SCALE_FACTOR = 1.2;
   DiscreteFunction* allFuncs[] = {&a,&b,&c};
   int allFuncsSize = 3;
   for(int k=0; k<allFuncsSize; k++)
   {
      for(int j=0; j<allFuncs[k]->domainSize(); j++)
      {
         allFuncs[k]->at(j) = static_cast<ValType>(j) * SCALE_FACTOR;
      }
   }

   //***************************************************************************
   // Make identical copies of variables (testing assignment operator)
   //***************************************************************************
   DiscreteFunction x, y, z;
   DiscreteFunction* copiedFuncs[] = {&x,&y,&z};
   x = a;
   y = b;
   z = c;

   //***************************************************************************
   // Check for equality
   //***************************************************************************
   for(int k=0; k<allFuncsSize; k++)
   {
      if(*allFuncs[k] != *copiedFuncs[k])
      {
         std::cout << "Equality after copy assignment failed" << std::endl;
         exitStatus = 1;
      }
   }

   //***************************************************************************
   // Now expand the original functions' domains so that they can depend
   // on additional values
   //***************************************************************************
   VarID newVars[] = {103,101,102}; // unsorted to check sorting
   for(int k=0; k<allFuncsSize; k++)
   {
      allFuncs[k]->expand(newVars,newVars+k+1);
   }

   //***************************************************************************
   // Check that domains are as expected
   //***************************************************************************
   for(int k=0; k<allFuncsSize; k++)
   {
      std::vector<VarID> oldDomain(copiedFuncs[k]->varBegin(),
                                   copiedFuncs[k]->varEnd());

      std::vector<VarID> correctDomain(newVars,newVars+k+1);
      correctDomain.push_back(vars[k]);
      std::sort(correctDomain.begin(),correctDomain.end());

      int varListSize = allFuncs[k]->varEnd() - allFuncs[k]->varBegin();
      if(allFuncs[k]->noVars() != varListSize)
      {
         std::cout << "Inconsistent domain size reported by allFunc["
                   << k << "]\n";
         exitStatus = 1;
      }

      if(correctDomain.size() != varListSize)
      {
         std::cout << "Wrong domain size reported by allFunc[" << k << "]\n";
         exitStatus = 1;
      }

      DiscreteFunction::VarIterator it1 = allFuncs[k]->varBegin();
      std::vector<VarID>::const_iterator it2 = correctDomain.begin();

      while( (allFuncs[k]->varEnd() != it1) && (correctDomain.end() != it2) )
      {
         if(*it1 != *it2)
         {
            std::cout << "Wrong domain reported by allFunc[" << k << "]\n";
            exitStatus = 1;
         }

         ++it1;
         ++it2;
      }

   } // for loop

   //***************************************************************************
   // Check that expansion does not affect equality
   //***************************************************************************
   for(int k=0; k<allFuncsSize; k++)
   {
      if(*allFuncs[k] != *copiedFuncs[k])
      {
         std::cout << "Equality after expansion failed\n\n";
         std::cout << *allFuncs[k] << "\nSHOULD EQUAL\n";
         std::cout << *copiedFuncs[k] << "\n\n\n";
         exitStatus = 1;
      }
   }

   //***************************************************************************
   // Change variables depending on new variable
   //***************************************************************************
   a(3,2) = 3004.12;
   a(3,4) = 3004.12;
   b(4,3,3) = 3414.12;
   b(3,2,3) = 3214.12;
   b(3,2,4) = 3013.12;
   c.at(3,2,3,4) = 3014.14;

   //***************************************************************************
   // Check that variables are now different
   //***************************************************************************
   for(int k=0; k<allFuncsSize; k++)
   {
      if(*allFuncs[k] == *copiedFuncs[k])
      {
         std::cout << k << ": Inequality after expansion failed\n\n";
         std::cout << *allFuncs[k] << "\nSHOULD DIFFER FROM\n";
         std::cout << *copiedFuncs[k] << "\n\n\n";
         exitStatus = 1;
      }
   }

   //***************************************************************************
   // Copy modified functions for later use
   //***************************************************************************
   DiscreteFunction modFuncs[] = {a,b,c};

   //***************************************************************************
   // Condition on 3rd variable so that equality is restored.
   //***************************************************************************
   ValIndex conditions[] = {1,0,6};
   std::cout << "TEST CONDITION : <103,101,102>=<1,0,6> \n";
   a.condition(newVars,newVars+3,conditions,conditions+3);
   b.condition(newVars,newVars+3,conditions,conditions+3);
   c.condition(newVars,newVars+3,conditions,conditions+3);


   //***************************************************************************
   // Check that equality is affected as expected
   //***************************************************************************
   for(int k=0; k<allFuncsSize; k++)
   {
      if(*allFuncs[k] != *copiedFuncs[k])
      {
         std::cout << k << ": Equality after conditioning failed\n\n";
         std::cout << "WAS:\n" << modFuncs[k] << "\nIS:\n";
         std::cout << *allFuncs[k] << "\nSHOULD EQUAL\n";
         std::cout << *copiedFuncs[k] << "\n\n\n";
         exitStatus = 1;
      }
   }

   //***************************************************************************
   // Repeat by conditioning in such a way that equality is not perserved.
   //***************************************************************************
   ValIndex newConditions[] = {4,2,3};
   std::cout << "TEST CONDITION : <103,101,102>=<4,2,3> \n";
   a = modFuncs[0];
   b = modFuncs[1];
   c = modFuncs[2];
   a.condition(newVars,newVars+3,newConditions,newConditions+3);
   b.condition(newVars,newVars+3,newConditions,newConditions+3);
   c.condition(newVars,newVars+3,newConditions,newConditions+3);

   //***************************************************************************
   // Check that equality is affected as expected
   //***************************************************************************
   for(int k=0; k<allFuncsSize; k++)
   {
      if(*allFuncs[k] == *copiedFuncs[k])
      {
         std::cout << k << ": Inequality after conditioning failed\n\n";
         std::cout << "WAS:\n" << modFuncs[k] << "\nIS:\n";
         std::cout << *allFuncs[k] << "\nSHOULD DIFFER FROM\n";
         std::cout << *copiedFuncs[k] << "\n\n\n";
         exitStatus = 1;
      }
   }
   
   //***************************************************************************
   // Return success or failure
   //***************************************************************************
   if(0==exitStatus)
   {
      std::cout << "Expansion tests all passed." << std::endl;
   }
   return exitStatus;

} // test expansion

int testOperators(DiscreteFunction f1, DiscreteFunction f2, DiscreteFunction f3)
{
   const ValType SCALE_FACTOR = 2.3;

   //***************************************************************************
   // Calculate domain union for all three operands
   //***************************************************************************
   std::vector<VarID> domainUnion;
   domainUnion.reserve(f1.noVars()+f2.noVars()+f3.noVars());
   domainUnion.insert(domainUnion.end(),f1.varBegin(),f1.varEnd());
   domainUnion.insert(domainUnion.end(),f2.varBegin(),f2.varEnd());
   domainUnion.insert(domainUnion.end(),f3.varBegin(),f3.varEnd());
   std::sort(domainUnion.begin(),domainUnion.end());
   std::vector<VarID>::iterator newEnd =
      std::unique(domainUnion.begin(),domainUnion.end());
   domainUnion.resize(newEnd-domainUnion.begin());

   //***************************************************************************
   // Apply addition operators
   //***************************************************************************
   DiscreteFunction add1 = f1 + f2 + (SCALE_FACTOR/2 + f3) + (SCALE_FACTOR/2);
   DiscreteFunction add2(f1);
   add2 += f3;
   add2 += SCALE_FACTOR;
   add2 += f2;
   DiscreteFunction allFun[] = {f1,f2,f3};
   DiscreteFunction add3 = SCALE_FACTOR;
   add3.add(allFun,allFun+3);
   DiscreteFunction add4 = f3 + SCALE_FACTOR + (+f1) + f2;

   DiscreteFunction allAdd[] = {add1,add2,add3,add4};

   //***************************************************************************
   // Check for addition consistency
   //***************************************************************************
   for(int k=0; k<4; ++k)
   {
      for(int j=0; j<4; ++j)
      {
         if(!strictlyEqualWithinTolerance(allAdd[k],allAdd[j]))
         {
            std::cout << "Addition operators are inconsistent.\n";
            return EXIT_FAILURE;
         }
      }
   }

   //***************************************************************************
   // Check that add results are correct
   //***************************************************************************
   for(int k=0; k<4; ++k)
   {
      for(DomainIterator it(domainUnion.begin(),domainUnion.end());
            it.hasNext(); ++it)
      {
         ValType f1_j = f1(domainUnion,it.getSubInd());
         ValType f2_j = f2(domainUnion,it.getSubInd());
         ValType f3_j = f3(domainUnion,it.getSubInd());
         ValType correctResult = f1_j + f2_j + f3_j + SCALE_FACTOR;
         ValType  actualResult = allAdd[k].at(it.getInd());

         ValType diff = std::fabs(1-actualResult/correctResult);
         if(diff > DEFAULT_VALUE_TOLERANCE)
         {
            std::cout << "Wrong result for add[" << k << "]\n";
            return EXIT_FAILURE;
         }

      } // inner for loop

   } // outer for loop

   //***************************************************************************
   // Apply minus operators
   //***************************************************************************
   DiscreteFunction minus1 = f3 + f1 - f2 - SCALE_FACTOR;
   DiscreteFunction minus2(f1);
   minus2 += f3;
   minus2 -= SCALE_FACTOR;
   minus2 -= f2;
   DiscreteFunction part1 = (f3+f1);
   DiscreteFunction part2 = -f2;
   DiscreteFunction part3 = part2 - SCALE_FACTOR;
   DiscreteFunction part4 = (-f2) - SCALE_FACTOR;
   DiscreteFunction part5 = part1 + part4;
   DiscreteFunction minus3 = (f3+f1) + (0-f2) - SCALE_FACTOR;
   DiscreteFunction allMinus[] = {minus1,minus2,minus3};

   //***************************************************************************
   // Check minus operators for consistency
   //***************************************************************************
   for(int k=0; k<3; ++k)
   {
      for(int j=0; j<3; ++j)
      {
         if(!strictlyEqualWithinTolerance(allMinus[k],allMinus[j]))
         {
            std::cout << "Minus operators are inconsistent.\n";
            return EXIT_FAILURE;
         }
      }
   }

   //***************************************************************************
   // Check that minus results are correct
   //***************************************************************************
   for(int k=0; k<3; ++k)
   {
      for(DomainIterator it(domainUnion.begin(),domainUnion.end());
            it.hasNext(); ++it)
      {
         ValType f1_j = f1(domainUnion,it.getSubInd());
         ValType f2_j = f2(domainUnion,it.getSubInd());
         ValType f3_j = f3(domainUnion,it.getSubInd());
         ValType correctResult = f1_j - f2_j + f3_j - SCALE_FACTOR;
         ValType  actualResult = allMinus[k].at(it.getInd());

         ValType diff = std::fabs(1-actualResult/correctResult);
         if(diff > DEFAULT_VALUE_TOLERANCE)
         {
            std::cout << "Wrong result for minus[" << k << "]\n";
            return EXIT_FAILURE;
         }

      } // inner for loop

   } // outer for loop

   //***************************************************************************
   // Apply product operators
   //***************************************************************************
   DiscreteFunction prod1 = 1 * f2 * f3 * (SCALE_FACTOR * f1);
   DiscreteFunction prod2(f3);
   prod2 *= f2;
   prod2 *= (f1*SCALE_FACTOR);
   DiscreteFunction allProd[] = {prod1,prod2};

   //***************************************************************************
   // Check for product operators for consistency
   //***************************************************************************
   if(!strictlyEqualWithinTolerance(prod1,prod2))
   {
      std::cout << "Product operators are inconsistent.\n";
      return EXIT_FAILURE;
   }

   //***************************************************************************
   // Check that product results are correct
   //***************************************************************************
   for(int k=0; k<2; ++k)
   {
      for(DomainIterator it(domainUnion.begin(),domainUnion.end());
            it.hasNext(); ++it)
      {
         ValType f1_j = f1(domainUnion,it.getSubInd());
         ValType f2_j = f2(domainUnion,it.getSubInd());
         ValType f3_j = f3(domainUnion,it.getSubInd());
         ValType correctResult = f1_j * f2_j * f3_j * SCALE_FACTOR;
         ValType  actualResult = allProd[k].at(it.getInd());

         ValType diff = std::fabs(1-actualResult/correctResult);
         if(diff > DEFAULT_VALUE_TOLERANCE)
         {
            std::cout << "Wrong result for prod[" << k << "]\n";
            return EXIT_FAILURE;
         }

      } // inner for loop

   } // outer for loop

   //***************************************************************************
   // Apply division operators
   //***************************************************************************
   DiscreteFunction div1 = 1 / f2 / f3 / SCALE_FACTOR / f1;
   DiscreteFunction div2(1/f3);
   div2 /= f2;
   div2 /= f1;
   div2 /= SCALE_FACTOR;
   DiscreteFunction div3 = 1 / f2 / (f3 * SCALE_FACTOR * f1);
   DiscreteFunction allDiv[] = {1/prod1,div1,div2,div3};

   //***************************************************************************
   // Check division operators for consistency
   //***************************************************************************
   for(int k=0; k<4; ++k)
   {
      for(int j=0; j<4; ++j)
      {
         if(!strictlyEqualWithinTolerance(allDiv[k],allDiv[j]))
         {
            std::cout << "Division results " << k << " and " << j <<
               " are inconsistent.\n";
            return EXIT_FAILURE;
         }

      } // inner loop

   } // outer loop

   //***************************************************************************
   // Check that division results are correct
   //***************************************************************************
   for(int k=0; k<4; ++k)
   {
      for(DomainIterator it(domainUnion.begin(),domainUnion.end());
            it.hasNext(); ++it)
      {
         ValType f1_j = f1(domainUnion,it.getSubInd());
         ValType f2_j = f2(domainUnion,it.getSubInd());
         ValType f3_j = f3(domainUnion,it.getSubInd());
         ValType correctResult = 1 / f1_j / f2_j / f3_j / SCALE_FACTOR;
         ValType  actualResult = allDiv[k].at(it.getInd());

         ValType diff = std::fabs(1-actualResult/correctResult);
         if(diff > DEFAULT_VALUE_TOLERANCE)
         {
            std::cout << "Wrong result for div[" << k << "]\n";
            return EXIT_FAILURE;
         }

      } // inner for loop

   } // outer for loop

   //***************************************************************************
   // Check that all results have correct domain.
   //***************************************************************************
   DiscreteFunction allResults[] =
      {add1,add2,add3,add4,minus1,minus2,minus3,prod1,prod2,div1,div2,div3};

   for(int k=0; k<12; ++k)
   {
      if(domainUnion.size()!=allResults[k].noVars())
      {
         std::cout << "Wrong domain size for result[" << k << "]\n";
         return EXIT_FAILURE;
      }

      if(!std::equal(domainUnion.begin(),domainUnion.end(),
               allResults[k].varBegin()))
      {
         std::cout << "Wrong domain for result[" << k << "]\n";
         return EXIT_FAILURE;
      }

   } // for loop

   return EXIT_SUCCESS;

} // testOperators

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
   // Test operators for each trio of functions
   //***************************************************************************
   int errorCount = 0;
   for(int k=0; k<funcs.size(); k++)
   {
      for(int j=0; j<funcs.size(); j++)
      {
         for(int i=0; i<funcs.size(); i++)
         {
            std::cout << "TESTING [" << k << "," << j << "," << i << "]\n";
            DiscreteFunction f1(funcs[k]), f2(funcs[j]), f3(funcs[i]);
            errorCount += testOperators(f1,f2,f3);
         }
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
      ValIndex sizes[] = {15,10,5,11,12,9,6};
      registerVariables(vars,vars+7,sizes,sizes+7);

      std::cout << "******************************************\n";
      std::cout << " Test Expansion \n";
      std::cout << "******************************************\n";
      int exitStatus = testExpansion();
      if(EXIT_SUCCESS!=exitStatus)
      {
         return EXIT_FAILURE;
      }

      //************************************************************************
      // Test arithmetic operators
      //************************************************************************
      std::cout << "******************************************\n";
      std::cout << " Test Arithmetic Operators\n";
      std::cout << "******************************************\n";
      exitStatus = testMath();
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

