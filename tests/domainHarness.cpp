/**
 * @file domHarness.cpp
 * Test Harness for maxsum::DomainIterator class
 */

#include <cassert>
#include <algorithm>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <vector>
#include "DomainIterator.h"
#include "common.h"
#include "register.h"

using namespace maxsum;

int testIterator
(
 const std::vector<VarID>& vars,
 const std::vector<VarID>& condVars,
 const std::vector<ValIndex>& condVals,
 DomainIterator begin
)
{
   //***************************************************************************
   // Check that iterator as correct variables
   //***************************************************************************
   DomainIterator::VarList itVars = begin.getVars();
   if(itVars.size() != vars.size())
   {
      std::cout << "Incorrect number of variables in iterator.\n";
      return 1;
   }

   if(begin.fixedCount()!=condVars.size())
   {
      std::cout << "Incorrect number of conditioned variables in iterator: "
       << begin.fixedCount() << " should be " << condVars.size() << std::endl;
      return 1;
   }

   for(int k=0; k<vars.size(); k++)
   {
      if(itVars[k] != vars[k])
      {
         std::cout << "Incorrect variables in iterator.\n";
         return 1;
      }
   }

   //***************************************************************************
   // Construct size vector
   //***************************************************************************
   std::vector<ValIndex> sizes(vars.size());
   ValIndex totSize = 1;
   for(int k=0; k<vars.size(); k++)
   {
      sizes[k] = getDomainSize(vars[k]);
      totSize *= sizes[k];
   }

   //***************************************************************************
   // Construct set of unconditioned variables and their corresponding sizes.
   //***************************************************************************
   std::vector<VarID> freeVars(vars.size());
   std::vector<VarID>::iterator result = std::set_difference
     (vars.begin(),vars.end(),condVars.begin(),condVars.end(),freeVars.begin());

   freeVars.resize(result-freeVars.begin());

   assert(freeVars.size()<=vars.size());

   //***************************************************************************
   // Construct size array for free sizes, and also calculate the total
   // free domain size.
   //***************************************************************************
   ValIndex totFreeSize = 1;
   std::vector<ValIndex> freeSizes(freeVars.size());
   for(int k=0; k<freeSizes.size(); k++)
   {
      freeSizes[k] = getDomainSize(freeVars[k]);
      totFreeSize *= freeSizes[k];
   }

   //***************************************************************************
   // Iterate domain from start to finish, and check all results
   //***************************************************************************
   const int MAX_LOOPS = 150000; // used to avoid infinite loops
   int count = 0;
   DomainIterator it(begin);
   for(; it.hasNext(); it++)
   {
      //************************************************************************
      // Check that indices are consistent
      //************************************************************************
      DomainIterator::IndList subInd = it.getSubInd();

      const ValIndex linearInd = sub2ind(sizes.begin(),sizes.end(),
            subInd.begin(),subInd.end());

      if(it.getInd() != linearInd)
      {
         std::cout << "Inconsistent sub indices: " << it.getInd()
            << " should be " << linearInd << std::endl;
         return 1;
      }

      //************************************************************************
      // Split sub indices into free and unfree sets
      //************************************************************************
      std::vector<ValIndex> curFree, curFixed;
      for(int k=0; k<subInd.size(); k++)
      {
         //*********************************************************************
         // If current variable is not in conditioned variable list, add it
         // value to the free list
         //*********************************************************************
         if(condVars.end() ==
               std::find(condVars.begin(),condVars.end(),vars[k]))
         {
            curFree.push_back(subInd[k]);
         }
         //*********************************************************************
         // Otherwise, add it to the conditioned (fixed) list
         //*********************************************************************
         else
         {
            curFixed.push_back(subInd[k]);
         }
      }

      //************************************************************************
      // Check that we have the right number of fixed and free variables
      //************************************************************************
      if(curFixed.size() != condVars.size())
      {
         std::cout << "Incorrect number of fixed variables: "
            << curFixed.size() << " should be " << condVars.size() << std::endl;

         return 1;
      }

      if(curFree.size() != vars.size()-condVars.size())
      {
         std::cout << "Incorrect number of free variables: "
            << curFree.size() << " should be " << vars.size()-condVars.size()
            << std::endl;

         return 1;
      }

      //************************************************************************
      // Check that the conditioned variables are all correct
      //************************************************************************
      std::vector<ValIndex>::const_iterator fixedIt = curFixed.begin();
      std::vector<ValIndex>::const_iterator condIt = condVals.begin();
      while( (fixedIt != curFixed.end()) && (condIt != condVals.end()) )
      {
         if(*fixedIt != *condIt)
         {
            std::cout << "Incorrect value for condition variable.\n";
            return 1;
         }
         ++fixedIt;
         ++condIt;
      }

      //************************************************************************
      // Check that free variables result in correct linear index for 
      // free part of the domain
      //************************************************************************
      const ValIndex freeInd = sub2ind(freeSizes.begin(),freeSizes.end(),
            curFree.begin(),curFree.end());

      if(freeInd != count)
      {
         std::cout << "Incorrect linear index for free domain" << std::endl;
         return 1;
      }

      //************************************************************************
      // Check for infinite loops
      //************************************************************************
      if(MAX_LOOPS < count++)
      {
         std::cout << "Maximum number of iterations reached - infinite loop?\n";
         return 1;
      }

   } // for loop

   //***************************************************************************
   // Check that we iterated over the correct number of elements
   //***************************************************************************
   if(totFreeSize != count)
   {
      std::cout << "Incorrect iteration total: " << count << " should be "
         << totFreeSize << std::endl;
      return 1;
   }

   //***************************************************************************
   // Check that we get the correct exceptions if we try to access the
   // iterator once we've reached the end of the domain.
   //***************************************************************************
   try
   {
      it.getSubInd();
      std::cout << "Missing exception from getSubInd() at end of list.\n";
      return 1;
   }
   catch(OutOfRangeException e) { /* this is good */ }

   try
   {
      it.getInd();
      std::cout << "Missing exception from getInd() at end of list.\n";
      return 1;
   }
   catch(OutOfRangeException e) { /* this is good */ }

   //***************************************************************************
   // If we get here - everything passed - return success
   //***************************************************************************
   return 0;

} // function testIterator

int testDomain(std::vector<VarID> vars)
{
   int exitStatus = 0;  // return success by default

   //***************************************************************************
   // Try to create a domain iterator over specified variable domain
   //***************************************************************************
   std::cout << "Trying with no conditioned variables" << std::endl;
   std::vector<VarID> condVars;
   std::vector<ValIndex> condVals;
   DomainIterator it(vars.begin(),vars.end());

   it.condition(condVars.begin(),condVars.end(),
                condVals.begin(),condVals.end());

   exitStatus += testIterator(vars,condVars,condVals,it);

   //***************************************************************************
   // Repeat with iterator conditioned on subset of variables
   //***************************************************************************
   for(int k=0; k<vars.size(); k+=2)
   {
      condVars.push_back(vars[k]);
      condVals.push_back(getDomainSize(vars[k])/2);
   }

   std::cout << "Trying with " << condVars.size() << " condition variables.\n";

   it.condition(condVars.begin(),condVars.end(),
                condVals.begin(),condVals.end());

   exitStatus += testIterator(vars,condVars,condVals,it);

   //***************************************************************************
   // Repeat with iterator created from overlapping variable subsets
   //***************************************************************************
   int start2 = 1 * vars.size() / 3;  // 1/3 along the list
   int end1   = 2 * vars.size() / 3;  // 2/3 along the list
   std::vector<VarID> vars1(vars.begin(),vars.begin()+end1);
   std::vector<VarID> vars2(vars.begin()+start2,vars.end());
   DomainIterator it2(vars1.begin(),vars1.end());

   if(it2.getVars().size()!=vars1.size())
   {
      std::cout << "Unexpected number of variables after construction.\n";
      return ++exitStatus;
   }

   it2.addVars(vars2.begin(),vars2.end());
   it2.condition(condVars.begin(),condVars.end(),
                condVals.begin(),condVals.end());

   std::cout << "Trying again with conditioned variables" << std::endl;
   exitStatus +=testIterator(vars,condVars,condVals,it2);

   std::cout << "Trying again with no conditioned variables" << std::endl;

   condVars.clear();
   condVals.clear();

   DomainIterator it3(vars1.begin(),vars1.end());
   it3.addVars(vars2.begin(),vars2.end());

   exitStatus +=testIterator(vars,condVars,condVals,it3);

   //***************************************************************************
   // Return total number of failures.
   //***************************************************************************
   return exitStatus;

} // function testDomain

int main()
{
   int exitStatus = 0;  // return success by default

   std::cout << "Hello world!" << std::endl;

   //***************************************************************************
   // Register some variables for test purposes
   //***************************************************************************
   VarID vars[] = {1,2,3,11,101};
   ValIndex sizes[] = {15,10,5,2,3};
   registerVariables(vars,vars+5,sizes,sizes+5);

   //***************************************************************************
   // Run tests over the power set of all registered variables
   //***************************************************************************
   for(unsigned long v = 0x0; v < 0x20; v++)
   {
      std::cout << "*****************************************************\n";
      std::cout << "Testing variable set: " << v << std::endl;
      std::cout << "*****************************************************\n";

      std::bitset<5> inDomain(v);

      std::vector<VarID> selectedVars;
      selectedVars.reserve(inDomain.size());
      for(int k=0; k<5; k++)
      {
         if(inDomain[k])
         {
            selectedVars.push_back(vars[k]);
         }
      }
      std::sort(selectedVars.begin(),selectedVars.end());

      if(0!=(exitStatus+=testDomain(selectedVars)))
      {
         std::cout << "Failed for test set: " << v << "\n\n";
      }
      else
      {
         std::cout << "Passed test set: " << v << "\n\n";
      }

   } // outer for loop

   //***************************************************************************
   // Print summary and return exit status (0 if all tests passed).
   //***************************************************************************
   std::cout << "*****************************************************\n";
   if(0!=exitStatus)
   {
      std::cout << "Failed some domain tests.\n";
   }
   else
   {
      std::cout << "Passed all domain tests.\n";
   }   
   std::cout << "*****************************************************\n";

   return exitStatus;

} // function main
